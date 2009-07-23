/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */ 

#include "Window.h"

#include <QFile>
#include <QMessageBox>
#include <QRegExp>
#include <QTimeEdit>
#include <cmath>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

static std::vector<std::pair<int, int> > points;

bool comp(std::pair<int, int> a, std::pair<int, int> b)
{
    return a.first < b.first;
}

void Window::process()
{
    if(m_pTable->rowCount() < 2)
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Vous devez spécifier au "
            "moins deux points."));
        return ;
    }

    // Construit la liste de points
    {
        points.clear();
        int row = 0;
        int t1, t2;
        QTime zero;
        QTimeEdit *te;
        for(; row < m_pTable->rowCount(); row++)
        {
            te = qobject_cast<QTimeEdit*>(
                m_pTable->cellWidget(row, 0));
            t1 = zero.secsTo(te->time());
            te = qobject_cast<QTimeEdit*>(
                m_pTable->cellWidget(row, 1));
            t2 = zero.secsTo(te->time());
            points.push_back(std::pair<int, int>(t1, t2));
        }
        std::sort(points.begin(), points.end(), comp);
    }

    // Vérifie que les noms de fichier ont été donnés
    if( (m_pSourceFile->path() == "")
     || (m_pTargetFile->path() == "") )
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Vous devez spécifier les "
            "fichiers de départ et d'arrivée..."));
        return;
    }

    // Ouvre les fichiers
    QFile in(m_pSourceFile->path());
    if(!in.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le "
            "fichier de départ (%1) !").arg(m_pSourceFile->path()));
        return ;
    }
    QFile out(m_pTargetFile->path());
    if(!out.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Erreur"), tr("Impossible d'ouvrir le "
            "fichier cible (%1) !").arg(m_pTargetFile->path()));
        in.close();
        return ;
    }

    static char buffer[1024];

    // Indique le numéro de la ligne qui sera lue le coup d'après
    // 1 : première ligne, le numéro du sous-titre
    // 2 : deuxième ligne, le temps. Exemple : 00:00:35,463 --> 00:00:38,245
    // 3 et plus : lignes suivantes, à recopier sans changement
    int state = 1;
    int line = 1;
    double u; // unused
    while(in.readLine(buffer, 1024) > 0)
    {
        QString str = buffer;
        str.resize(str.size() - 1); // supprime le \n
        if(str == "")
            state = 1;
        else if(state == 1)
            state = 2;
        else if(state == 2)
        {
            QRegExp rx("^([0-9]{2}):([0-9]{2}):([0-9]{2}),([0-9]{2,3})"
                " --> "
                "([0-9]{2}):([0-9]{2}):([0-9]{2}),([0-9]{2,3})");
            if(!rx.exactMatch(str))
            {
                QMessageBox::critical(this, tr("Erreur de syntaxe"),
                    tr("Erreur de syntaxe à la ligne %1").arg(line));
                return ;
            }
            double t1 = rx.cap(1).toInt()*3600.0 + rx.cap(2).toInt()*60.0
                + rx.cap(3).toInt() + rx.cap(4).toInt()*0.001;
            double t2 = rx.cap(5).toInt()*3600.0 + rx.cap(6).toInt()*60.0
                + rx.cap(7).toInt() + rx.cap(8).toInt()*0.001;
            t1 = shift(t1); t2 = shift(t2);
            int ti1 = (int)t1, ti2 = (int)t2;
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(2) << ti1/3600 << ":"
                << std::setw(2) << (ti1%3600)/60 << ":" << std::setw(2)
                << ti1%60 << "," << std::setw(3) << (int)(modf(t1, &u)*1000.0)
                << " --> " << std::setw(2) << ti2/3600 << ":"
                << std::setw(2) << (ti2%3600)/60 << ":" << std::setw(2)
                << ti2%60 << "," << std::setw(3) << (int)(modf(t2, &u)*1000.0);
            str = QString::fromStdString(oss.str());
            state = 3;
        }
        out.write(str.toAscii());
        out.write("\n");
        line++;
    }

    out.close();
    in.close();
}

double Window::shift(double t)
{
    std::vector<std::pair<int, int> >::iterator it = points.begin(),
        last = it;
    it++;
    while(it->first < t)
    {
        std::vector<std::pair<int, int> >::iterator last2 = last;
        last = it;
        it++;
        if(it == points.end())
        {
            it = last;
            last = last2;
            break;
        }
    }

    // t est dans l'intervalle [last ; it]
    int x1 = last->first, x2 = it->first;
    int y1 = last->second, y2 = it->second;

    return y1 + (t - x1)*(y2 - y1)/(x2 - x1);
}

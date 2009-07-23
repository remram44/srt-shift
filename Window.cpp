/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */ 

#include "Window.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFrame>
#include <QTimeEdit>
#include <QHeaderView>

Window::Window()
{
    resize(680, -1);

    QHBoxLayout *hlayout = new QHBoxLayout;

    // Le tableau, à gauche
    {
        m_pTable = new QTableWidget;
        m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
        m_pTable->setColumnCount(3);
        QStringList labels;
        labels << tr("Position du sous-titre") << tr("Position voulue")
            << tr("Supprimer le point");
        m_pTable->setHorizontalHeaderLabels(labels);
        m_pTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
        addPoint(); addPoint();
        
        hlayout->addWidget(m_pTable);
    }

    // Le reste, à droite
    {
        QVBoxLayout *vlayout = new QVBoxLayout;
        QPushButton *add = new QPushButton(tr("Ajouter un point"));
        connect(add, SIGNAL(clicked()), this, SLOT(addPoint()));
        vlayout->addWidget(add);
        m_pSourceFile = new FileSelector(FileSelector::READ);
        vlayout->addWidget(m_pSourceFile);
        m_pTargetFile = new FileSelector(FileSelector::WRITE);
        vlayout->addWidget(m_pTargetFile);
        QPushButton *go = new QPushButton(tr("Appliquer"));
        connect(go, SIGNAL(clicked()), this, SLOT(process()));
        vlayout->addWidget(go);

        hlayout->addLayout(vlayout);
    }

    setLayout(hlayout);
    show();
}

void Window::addPoint()
{
    // Agrandit le tableau
    int row = m_pTable->rowCount();
    m_pTable->setRowCount(row + 1);

    // Remplit la nouvelle ligne
    m_pTable->setCellWidget(row, 0, new QTimeEdit);
    m_pTable->setCellWidget(row, 1, new QTimeEdit);
    QPushButton *r = new RemoveButton(row);
    connect(r, SIGNAL(removeRow(int)), this, SLOT(removePoint(int)));
    m_pTable->setCellWidget(row, 2, r);
}

void Window::removePoint(int row)
{
    m_pTable->removeRow(row);
    for(; row < m_pTable->rowCount(); row++)
        qobject_cast<RemoveButton*>(m_pTable->cellWidget(row, 2))->setRow(row);
}

RemoveButton::RemoveButton(int row)
  : QPushButton::QPushButton(tr("Supprimer")), m_iRow(row)
{
    connect(this, SIGNAL(clicked()), this, SLOT(onclick()));
}

void RemoveButton::setRow(int row)
{
    m_iRow = row;
}

void RemoveButton::onclick()
{
    emit removeRow(m_iRow);
}

FileSelector::FileSelector(EOpenType type, QString text, QWidget *parent)
  : QFrame::QFrame(parent), m_eType(type)
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    m_pLineEdit = new QLineEdit;
    hlayout->addWidget(m_pLineEdit);
    m_pLineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    QPushButton *searchButton = new QPushButton(tr("Parcourir"));
    connect(searchButton, SIGNAL(clicked()), this, SLOT(search()));
    hlayout->addWidget(searchButton);

    setLayout(hlayout);
}

void FileSelector::search()
{
    QString ret;
    if(m_eType == FileSelector::READ)
        ret = QFileDialog::getOpenFileName(this, tr("Choisissez le  fichier "
            ".srt de départ"), "",
            tr("Sous-titre au format SubRip (*.srt);;Tous les fichiers (*.*)"));
    else
        ret = QFileDialog::getSaveFileName(this, tr("Choisissez le fichier "
            ".srt de départ"), "",
            tr("Sous-titre au format SubRip (*.srt);;Tous les fichiers (*.*)"));
    if(!ret.isNull())
        m_pLineEdit->setText(ret);
}

QString FileSelector::path() const
{
    return m_pLineEdit->text();
}

void FileSelector::setPath(QString text)
{
    m_pLineEdit->setText(text);
}

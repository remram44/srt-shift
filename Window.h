/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */ 

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>

class FileSelector : public QFrame {

    Q_OBJECT

public:
    enum EOpenType {
        READ, // Fichier déjà existant
        WRITE // Fichier non-existant
    };

private:
    EOpenType m_eType;
    QLineEdit *m_pLineEdit;

private slots:
    void search();

public:
    FileSelector(EOpenType type, QString text = "", QWidget *parent = NULL);
    QString path() const;

public slots:
    void setPath(QString text);

};

class RemoveButton : public QPushButton {

    Q_OBJECT

private:
    int m_iRow;

public:
    RemoveButton(int row);
    void setRow(int row);

private slots:
    void onclick();

signals:
    void removeRow(int);

};

class Window : public QWidget {

    Q_OBJECT

private:
    QTableWidget *m_pTable;
    FileSelector *m_pSourceFile;
    FileSelector *m_pTargetFile;

private:
    double shift(double t);

public:
    Window();

public slots:
    void process();
    void addPoint();
    void removePoint(int row);

};

#endif

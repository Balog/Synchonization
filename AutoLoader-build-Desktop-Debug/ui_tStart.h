/********************************************************************************
** Form generated from reading UI file 'tStart.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TSTART_H
#define UI_TSTART_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_tStart
{
public:
    QPushButton *pushButton;

    void setupUi(QDialog *tStart)
    {
        if (tStart->objectName().isEmpty())
            tStart->setObjectName(QString::fromUtf8("tStart"));
        tStart->resize(92, 42);
        pushButton = new QPushButton(tStart);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(10, 10, 75, 23));

        retranslateUi(tStart);

        QMetaObject::connectSlotsByName(tStart);
    } // setupUi

    void retranslateUi(QDialog *tStart)
    {
        tStart->setWindowTitle(QApplication::translate("tStart", "tStart", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("tStart", "\320\241\321\202\320\260\321\200\321\202", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class tStart: public Ui_tStart {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TSTART_H

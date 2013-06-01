/********************************************************************************
** Form generated from reading UI file 'Zast.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZAST_H
#define UI_ZAST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_Zast
{
public:
    QLabel *label;

    void setupUi(QDialog *Zast)
    {
        if (Zast->objectName().isEmpty())
            Zast->setObjectName(QString::fromUtf8("Zast"));
        Zast->resize(320, 238);
        label = new QLabel(Zast);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 0, 320, 238));

        retranslateUi(Zast);

        QMetaObject::connectSlotsByName(Zast);
    } // setupUi

    void retranslateUi(QDialog *Zast)
    {
        Zast->setWindowTitle(QApplication::translate("Zast", "\320\227\320\260\321\201\321\202\320\260\320\262\320\272\320\260", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Zast: public Ui_Zast {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZAST_H

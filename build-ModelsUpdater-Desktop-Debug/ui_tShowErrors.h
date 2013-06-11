/********************************************************************************
** Form generated from reading UI file 'tShowErrors.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TSHOWERRORS_H
#define UI_TSHOWERRORS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>

QT_BEGIN_NAMESPACE

class Ui_tShowErrors
{
public:
    QListView *listView;

    void setupUi(QDialog *tShowErrors)
    {
        if (tShowErrors->objectName().isEmpty())
            tShowErrors->setObjectName(QString::fromUtf8("tShowErrors"));
        tShowErrors->resize(500, 243);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tShowErrors->sizePolicy().hasHeightForWidth());
        tShowErrors->setSizePolicy(sizePolicy);
        tShowErrors->setMinimumSize(QSize(500, 243));
        tShowErrors->setMaximumSize(QSize(500, 243));
        listView = new QListView(tShowErrors);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setGeometry(QRect(2, 2, 497, 240));
        QPalette palette;
        QBrush brush(QColor(255, 0, 4, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        QBrush brush1(QColor(120, 120, 120, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush1);
        listView->setPalette(palette);
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        listView->setFont(font);
        listView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listView->setSelectionMode(QAbstractItemView::NoSelection);
        listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

        retranslateUi(tShowErrors);

        QMetaObject::connectSlotsByName(tShowErrors);
    } // setupUi

    void retranslateUi(QDialog *tShowErrors)
    {
        tShowErrors->setWindowTitle(QApplication::translate("tShowErrors", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class tShowErrors: public Ui_tShowErrors {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TSHOWERRORS_H

/********************************************************************************
** Form generated from reading UI file 'tNewPath.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TNEWPATH_H
#define UI_TNEWPATH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_tNewPath
{
public:
    QPushButton *pbOk;
    QPushButton *pbCancel;
    QToolButton *tbProjectFolder;
    QToolButton *tbTempFolder;
    QLineEdit *leProjectFolder;
    QLineEdit *leTempFolder;
    QLabel *label;
    QLabel *label_2;
    QTextEdit *teMessage;

    void setupUi(QDialog *tNewPath)
    {
        if (tNewPath->objectName().isEmpty())
            tNewPath->setObjectName(QString::fromUtf8("tNewPath"));
        tNewPath->resize(416, 139);
        tNewPath->setMinimumSize(QSize(416, 139));
        tNewPath->setMaximumSize(QSize(416, 139));
        pbOk = new QPushButton(tNewPath);
        pbOk->setObjectName(QString::fromUtf8("pbOk"));
        pbOk->setGeometry(QRect(330, 20, 75, 23));
        pbCancel = new QPushButton(tNewPath);
        pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
        pbCancel->setGeometry(QRect(330, 60, 75, 23));
        tbProjectFolder = new QToolButton(tNewPath);
        tbProjectFolder->setObjectName(QString::fromUtf8("tbProjectFolder"));
        tbProjectFolder->setGeometry(QRect(10, 20, 25, 19));
        tbTempFolder = new QToolButton(tNewPath);
        tbTempFolder->setObjectName(QString::fromUtf8("tbTempFolder"));
        tbTempFolder->setGeometry(QRect(10, 60, 25, 19));
        leProjectFolder = new QLineEdit(tNewPath);
        leProjectFolder->setObjectName(QString::fromUtf8("leProjectFolder"));
        leProjectFolder->setGeometry(QRect(40, 20, 281, 20));
        leTempFolder = new QLineEdit(tNewPath);
        leTempFolder->setObjectName(QString::fromUtf8("leTempFolder"));
        leTempFolder->setGeometry(QRect(40, 60, 281, 20));
        label = new QLabel(tNewPath);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(43, 2, 81, 16));
        label_2 = new QLabel(tNewPath);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(42, 42, 141, 16));
        teMessage = new QTextEdit(tNewPath);
        teMessage->setObjectName(QString::fromUtf8("teMessage"));
        teMessage->setGeometry(QRect(10, 90, 391, 41));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(255, 0, 0, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        QBrush brush2(QColor(120, 120, 120, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        teMessage->setPalette(palette);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        teMessage->setFont(font);
        teMessage->setReadOnly(true);

        retranslateUi(tNewPath);

        QMetaObject::connectSlotsByName(tNewPath);
    } // setupUi

    void retranslateUi(QDialog *tNewPath)
    {
        tNewPath->setWindowTitle(QApplication::translate("tNewPath", "\320\243\320\272\320\260\320\266\320\270 \321\200\320\260\320\261\320\276\321\207\320\270\320\265 \320\277\320\260\320\277\320\272\320\270", 0, QApplication::UnicodeUTF8));
        pbOk->setText(QApplication::translate("tNewPath", "Ok", 0, QApplication::UnicodeUTF8));
        pbCancel->setText(QApplication::translate("tNewPath", "\320\236\321\202\320\274\320\265\320\275\320\260", 0, QApplication::UnicodeUTF8));
        tbProjectFolder->setText(QApplication::translate("tNewPath", "...", 0, QApplication::UnicodeUTF8));
        tbTempFolder->setText(QApplication::translate("tNewPath", "...", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("tNewPath", "\320\237\320\260\320\277\320\272\320\260 \320\277\321\200\320\276\320\265\320\272\321\202\320\260", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("tNewPath", "\320\237\320\260\320\277\320\272\320\260 \320\262\321\200\320\265\320\274\320\265\320\275\320\275\321\213\321\205 \321\204\320\260\320\271\320\273\320\276\320\262", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class tNewPath: public Ui_tNewPath {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TNEWPATH_H

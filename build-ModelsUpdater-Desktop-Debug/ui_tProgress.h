/********************************************************************************
** Form generated from reading UI file 'tProgress.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TPROGRESS_H
#define UI_TPROGRESS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>

QT_BEGIN_NAMESPACE

class Ui_tProgress
{
public:
    QProgressBar *progressBar;
    QLabel *label;
    QFrame *line;
    QFrame *line_2;
    QFrame *line_3;
    QFrame *line_4;

    void setupUi(QDialog *tProgress)
    {
        if (tProgress->objectName().isEmpty())
            tProgress->setObjectName(QString::fromUtf8("tProgress"));
        tProgress->setWindowModality(Qt::NonModal);
        tProgress->setEnabled(true);
        tProgress->resize(707, 55);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tProgress->sizePolicy().hasHeightForWidth());
        tProgress->setSizePolicy(sizePolicy);
        tProgress->setMinimumSize(QSize(707, 55));
        tProgress->setMaximumSize(QSize(707, 55));
        tProgress->setMouseTracking(false);
        progressBar = new QProgressBar(tProgress);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setEnabled(true);
        progressBar->setGeometry(QRect(4, 27, 701, 23));
        progressBar->setValue(24);
        label = new QLabel(tProgress);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(236, 3, 231, 20));
        QFont font;
        font.setPointSize(14);
        label->setFont(font);
        line = new QFrame(tProgress);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(2, 0, 707, 3));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(tProgress);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(1, 52, 705, 3));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(tProgress);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(1, 0, 3, 55));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);
        line_4 = new QFrame(tProgress);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(704, 1, 3, 53));
        line_4->setFrameShape(QFrame::VLine);
        line_4->setFrameShadow(QFrame::Sunken);

        retranslateUi(tProgress);

        QMetaObject::connectSlotsByName(tProgress);
    } // setupUi

    void retranslateUi(QDialog *tProgress)
    {
        tProgress->setWindowTitle(QApplication::translate("tProgress", "\320\237\320\276\320\264\320\276\320\266\320\264\320\270\321\202\320\265...", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("tProgress", "\320\237\320\276\320\264\320\276\320\266\320\264\320\270\321\202\320\265 \320\277\320\276\320\266\320\260\320\273\321\203\320\271\321\201\321\202\320\260...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class tProgress: public Ui_tProgress {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TPROGRESS_H

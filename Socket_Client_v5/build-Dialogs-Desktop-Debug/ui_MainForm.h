/********************************************************************************
** Form generated from reading UI file 'MainForm.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINFORM_H
#define UI_MAINFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_MainForm
{
public:
    QLineEdit *leRoot;
    QLabel *label;
    QPushButton *pbConnect;
    QLineEdit *leAddr;
    QSpinBox *sbPort;
    QPushButton *pbDisconnect;
    QListView *lvSendingFiles;
    QPushButton *pbAddSend;
    QPushButton *pbStartSend;
    QPushButton *pbClearAll;
    QListView *lvDeletingFiles;
    QPushButton *pbAddDelete;
    QPushButton *pbClearDelete;
    QPushButton *pbListFiles;
    QListView *lvListFiles;
    QLabel *label_2;
    QLineEdit *leTemp;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QPushButton *pbClearReceiveAll;
    QPushButton *pbClearDeleteLocal;
    QPushButton *pbAddLocalDelete;
    QListView *lvLocalDeletingFiles;
    QListView *lvLocalListFiles;
    QPushButton *pbAddReceive;
    QListView *lvReceiveFiles;
    QLabel *label_8;
    QPushButton *pbStartReceive;
    QPushButton *pbLocalListFiles;
    QPushButton *pbUpdateStruct;
    QPushButton *pushButton;
    QListView *lwListModels;
    QListView *lwLocalListModels;
    QLabel *label_9;
    QLabel *label_10;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;

    void setupUi(QDialog *MainForm)
    {
        if (MainForm->objectName().isEmpty())
            MainForm->setObjectName(QString::fromUtf8("MainForm"));
        MainForm->resize(1059, 809);
        leRoot = new QLineEdit(MainForm);
        leRoot->setObjectName(QString::fromUtf8("leRoot"));
        leRoot->setGeometry(QRect(600, 755, 141, 20));
        label = new QLabel(MainForm);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(540, 757, 51, 16));
        pbConnect = new QPushButton(MainForm);
        pbConnect->setObjectName(QString::fromUtf8("pbConnect"));
        pbConnect->setGeometry(QRect(200, 10, 75, 23));
        leAddr = new QLineEdit(MainForm);
        leAddr->setObjectName(QString::fromUtf8("leAddr"));
        leAddr->setGeometry(QRect(10, 10, 113, 20));
        sbPort = new QSpinBox(MainForm);
        sbPort->setObjectName(QString::fromUtf8("sbPort"));
        sbPort->setGeometry(QRect(130, 10, 61, 22));
        sbPort->setMaximum(9999);
        sbPort->setValue(1234);
        pbDisconnect = new QPushButton(MainForm);
        pbDisconnect->setObjectName(QString::fromUtf8("pbDisconnect"));
        pbDisconnect->setGeometry(QRect(280, 10, 81, 23));
        lvSendingFiles = new QListView(MainForm);
        lvSendingFiles->setObjectName(QString::fromUtf8("lvSendingFiles"));
        lvSendingFiles->setGeometry(QRect(10, 60, 421, 181));
        pbAddSend = new QPushButton(MainForm);
        pbAddSend->setObjectName(QString::fromUtf8("pbAddSend"));
        pbAddSend->setGeometry(QRect(970, 582, 81, 23));
        pbStartSend = new QPushButton(MainForm);
        pbStartSend->setObjectName(QString::fromUtf8("pbStartSend"));
        pbStartSend->setGeometry(QRect(440, 780, 81, 23));
        pbClearAll = new QPushButton(MainForm);
        pbClearAll->setObjectName(QString::fromUtf8("pbClearAll"));
        pbClearAll->setGeometry(QRect(440, 60, 81, 23));
        lvDeletingFiles = new QListView(MainForm);
        lvDeletingFiles->setObjectName(QString::fromUtf8("lvDeletingFiles"));
        lvDeletingFiles->setGeometry(QRect(10, 260, 421, 141));
        pbAddDelete = new QPushButton(MainForm);
        pbAddDelete->setObjectName(QString::fromUtf8("pbAddDelete"));
        pbAddDelete->setGeometry(QRect(440, 613, 81, 23));
        pbClearDelete = new QPushButton(MainForm);
        pbClearDelete->setObjectName(QString::fromUtf8("pbClearDelete"));
        pbClearDelete->setGeometry(QRect(440, 260, 81, 23));
        pbListFiles = new QPushButton(MainForm);
        pbListFiles->setObjectName(QString::fromUtf8("pbListFiles"));
        pbListFiles->setGeometry(QRect(440, 750, 81, 23));
        lvListFiles = new QListView(MainForm);
        lvListFiles->setObjectName(QString::fromUtf8("lvListFiles"));
        lvListFiles->setGeometry(QRect(10, 584, 421, 161));
        label_2 = new QLabel(MainForm);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(540, 782, 51, 16));
        leTemp = new QLineEdit(MainForm);
        leTemp->setObjectName(QString::fromUtf8("leTemp"));
        leTemp->setGeometry(QRect(600, 780, 141, 20));
        label_3 = new QLabel(MainForm);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(13, 40, 111, 16));
        label_4 = new QLabel(MainForm);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 240, 101, 16));
        label_5 = new QLabel(MainForm);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 405, 101, 16));
        label_6 = new QLabel(MainForm);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(543, 40, 111, 16));
        label_7 = new QLabel(MainForm);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(540, 405, 111, 16));
        pbClearReceiveAll = new QPushButton(MainForm);
        pbClearReceiveAll->setObjectName(QString::fromUtf8("pbClearReceiveAll"));
        pbClearReceiveAll->setGeometry(QRect(970, 60, 81, 23));
        pbClearDeleteLocal = new QPushButton(MainForm);
        pbClearDeleteLocal->setObjectName(QString::fromUtf8("pbClearDeleteLocal"));
        pbClearDeleteLocal->setGeometry(QRect(970, 260, 81, 23));
        pbAddLocalDelete = new QPushButton(MainForm);
        pbAddLocalDelete->setObjectName(QString::fromUtf8("pbAddLocalDelete"));
        pbAddLocalDelete->setGeometry(QRect(970, 612, 81, 23));
        lvLocalDeletingFiles = new QListView(MainForm);
        lvLocalDeletingFiles->setObjectName(QString::fromUtf8("lvLocalDeletingFiles"));
        lvLocalDeletingFiles->setGeometry(QRect(540, 260, 421, 141));
        lvLocalListFiles = new QListView(MainForm);
        lvLocalListFiles->setObjectName(QString::fromUtf8("lvLocalListFiles"));
        lvLocalListFiles->setGeometry(QRect(540, 584, 421, 161));
        pbAddReceive = new QPushButton(MainForm);
        pbAddReceive->setObjectName(QString::fromUtf8("pbAddReceive"));
        pbAddReceive->setGeometry(QRect(440, 583, 81, 23));
        lvReceiveFiles = new QListView(MainForm);
        lvReceiveFiles->setObjectName(QString::fromUtf8("lvReceiveFiles"));
        lvReceiveFiles->setGeometry(QRect(540, 60, 421, 181));
        label_8 = new QLabel(MainForm);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(540, 240, 121, 16));
        pbStartReceive = new QPushButton(MainForm);
        pbStartReceive->setObjectName(QString::fromUtf8("pbStartReceive"));
        pbStartReceive->setGeometry(QRect(970, 780, 81, 23));
        pbLocalListFiles = new QPushButton(MainForm);
        pbLocalListFiles->setObjectName(QString::fromUtf8("pbLocalListFiles"));
        pbLocalListFiles->setGeometry(QRect(970, 750, 81, 23));
        pbUpdateStruct = new QPushButton(MainForm);
        pbUpdateStruct->setObjectName(QString::fromUtf8("pbUpdateStruct"));
        pbUpdateStruct->setGeometry(QRect(10, 770, 75, 23));
        pushButton = new QPushButton(MainForm);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(90, 770, 75, 23));
        lwListModels = new QListView(MainForm);
        lwListModels->setObjectName(QString::fromUtf8("lwListModels"));
        lwListModels->setGeometry(QRect(10, 420, 421, 141));
        lwLocalListModels = new QListView(MainForm);
        lwLocalListModels->setObjectName(QString::fromUtf8("lwLocalListModels"));
        lwLocalListModels->setGeometry(QRect(540, 420, 421, 141));
        label_9 = new QLabel(MainForm);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 564, 141, 16));
        label_10 = new QLabel(MainForm);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(540, 562, 141, 16));
        pushButton_2 = new QPushButton(MainForm);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(440, 420, 81, 23));
        pushButton_3 = new QPushButton(MainForm);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(970, 420, 75, 23));

        retranslateUi(MainForm);
        QObject::connect(pbConnect, SIGNAL(clicked()), MainForm, SLOT(OnConnect()));
        QObject::connect(pbDisconnect, SIGNAL(clicked()), MainForm, SLOT(OnDisconnecting()));
        QObject::connect(pbAddSend, SIGNAL(clicked()), MainForm, SLOT(OnAddSend()));
        QObject::connect(pbStartSend, SIGNAL(clicked()), MainForm, SLOT(OnStartSend()));
        QObject::connect(pbClearAll, SIGNAL(clicked()), MainForm, SLOT(OnClearSendAll()));
        QObject::connect(pbAddDelete, SIGNAL(clicked()), MainForm, SLOT(OnAddDelete()));
        QObject::connect(pbClearDelete, SIGNAL(clicked()), MainForm, SLOT(OnClearDelete()));
        QObject::connect(pbListFiles, SIGNAL(clicked()), MainForm, SLOT(OnListFiles()));
        QObject::connect(pbAddReceive, SIGNAL(clicked()), MainForm, SLOT(OnAddReceive()));
        QObject::connect(pbClearReceiveAll, SIGNAL(clicked()), MainForm, SLOT(OnReceiveClear()));
        QObject::connect(pbAddLocalDelete, SIGNAL(clicked()), MainForm, SLOT(OnDeleteLocal()));
        QObject::connect(pbClearDeleteLocal, SIGNAL(clicked()), MainForm, SLOT(OnClearDeleteLocal()));
        QObject::connect(pbStartReceive, SIGNAL(clicked()), MainForm, SLOT(OnStartReceive()));
        QObject::connect(pbLocalListFiles, SIGNAL(clicked()), MainForm, SLOT(OnListFilesLocal()));
        QObject::connect(pbUpdateStruct, SIGNAL(clicked()), MainForm, SLOT(OnUpdateStruct()));
        QObject::connect(pushButton, SIGNAL(clicked()), MainForm, SLOT(OnClearModels()));
        QObject::connect(lwLocalListModels, SIGNAL(clicked(QModelIndex)), MainForm, SLOT(OnLocalModelClick(QModelIndex)));
        QObject::connect(lwListModels, SIGNAL(clicked(QModelIndex)), MainForm, SLOT(OnServerModelClick(QModelIndex)));

        QMetaObject::connectSlotsByName(MainForm);
    } // setupUi

    void retranslateUi(QDialog *MainForm)
    {
        MainForm->setWindowTitle(QApplication::translate("MainForm", "Dialog", 0, QApplication::UnicodeUTF8));
        leRoot->setText(QString());
        label->setText(QApplication::translate("MainForm", "Root path", 0, QApplication::UnicodeUTF8));
        pbConnect->setText(QApplication::translate("MainForm", "Connect", 0, QApplication::UnicodeUTF8));
        leAddr->setText(QApplication::translate("MainForm", "127.0.0.1", 0, QApplication::UnicodeUTF8));
        pbDisconnect->setText(QApplication::translate("MainForm", "Disconnect", 0, QApplication::UnicodeUTF8));
        pbAddSend->setText(QApplication::translate("MainForm", "Add Send", 0, QApplication::UnicodeUTF8));
        pbStartSend->setText(QApplication::translate("MainForm", "Start Send&&Del", 0, QApplication::UnicodeUTF8));
        pbClearAll->setText(QApplication::translate("MainForm", "Clear All", 0, QApplication::UnicodeUTF8));
        pbAddDelete->setText(QApplication::translate("MainForm", "Add Delete", 0, QApplication::UnicodeUTF8));
        pbClearDelete->setText(QApplication::translate("MainForm", "Clear Delete", 0, QApplication::UnicodeUTF8));
        pbListFiles->setText(QApplication::translate("MainForm", "List models", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainForm", "Temp path", 0, QApplication::UnicodeUTF8));
        leTemp->setText(QString());
        label_3->setText(QApplication::translate("MainForm", "\320\237\320\265\321\200\320\265\320\264\320\260\321\202\321\214 \320\275\320\260 \321\201\320\265\321\200\320\262\320\265\321\200", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainForm", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214 \321\201 \321\201\320\265\321\200\320\262\320\265\321\200\320\260", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainForm", "\320\234\320\276\320\264\320\265\320\273\320\270 \320\275\320\260 \321\201\320\265\321\200\320\262\320\265\321\200\320\265", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainForm", "\320\237\321\200\320\276\321\207\320\270\321\202\320\260\321\202\321\214 \321\201 \321\201\320\265\321\200\320\262\320\265\321\200\320\260", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainForm", "\320\234\320\276\320\264\320\265\320\273\320\270 \320\275\320\260 \320\273\320\276\320\272\320\260\320\273\321\214\320\275\320\276\320\274", 0, QApplication::UnicodeUTF8));
        pbClearReceiveAll->setText(QApplication::translate("MainForm", "Clear All", 0, QApplication::UnicodeUTF8));
        pbClearDeleteLocal->setText(QApplication::translate("MainForm", "Clear Delete", 0, QApplication::UnicodeUTF8));
        pbAddLocalDelete->setText(QApplication::translate("MainForm", "Add Delete", 0, QApplication::UnicodeUTF8));
        pbAddReceive->setText(QApplication::translate("MainForm", "Add Receive", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainForm", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214 \321\201 \320\273\320\276\320\272\320\260\320\273\321\214\320\275\320\276\320\263\320\276", 0, QApplication::UnicodeUTF8));
        pbStartReceive->setText(QApplication::translate("MainForm", "Start Rec&&Del", 0, QApplication::UnicodeUTF8));
        pbLocalListFiles->setText(QApplication::translate("MainForm", "List models", 0, QApplication::UnicodeUTF8));
        pbUpdateStruct->setText(QApplication::translate("MainForm", "\320\236\320\261\320\275\320\276\320\262\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainForm", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainForm", "\320\244\320\260\320\271\320\273\321\213 \320\274\320\276\320\264\320\265\320\273\320\270 \320\275\320\260 \321\201\320\265\321\200\320\262\320\265\321\200\320\265", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainForm", "\320\244\320\260\320\271\320\273\321\213 \320\274\320\276\320\264\320\265\320\273\320\270 \320\275\320\260 \320\272\320\273\320\270\320\265\320\275\321\202\320\265", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MainForm", "Receive Model", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("MainForm", "Send Model", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainForm: public Ui_MainForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFORM_H

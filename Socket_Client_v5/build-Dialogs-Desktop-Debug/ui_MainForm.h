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
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

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
    QLabel *label_2;
    QLineEdit *leTemp;
    QPushButton *pbUpdateStruct;
    QPushButton *pushButton;
    QTabWidget *tabWidget;
    QWidget *tab;
    QPushButton *pbLocalListFiles;
    QLabel *label_9;
    QLabel *label_7;
    QListView *lvLocalListFiles;
    QListView *lwListModels;
    QPushButton *pbAddDelete;
    QPushButton *pbListFiles;
    QLabel *label_5;
    QPushButton *pbClearDelete;
    QPushButton *pushButton_3;
    QListView *lvDeletingFiles;
    QListView *lvReceiveFiles;
    QPushButton *pushButton_2;
    QPushButton *pbClearAll;
    QPushButton *pbAddReceive;
    QLabel *label_4;
    QPushButton *pbStartReceive;
    QListView *lwLocalListModels;
    QPushButton *pbAddSend;
    QLabel *label_10;
    QListView *lvLocalDeletingFiles;
    QLabel *label_8;
    QPushButton *pbAddLocalDelete;
    QListView *lvListFiles;
    QPushButton *pbStartSend;
    QLabel *label_3;
    QPushButton *pbClearDeleteLocal;
    QListView *lvSendingFiles;
    QPushButton *pbClearReceiveAll;
    QLabel *label_6;
    QWidget *tab_2;
    QWidget *tab_3;

    void setupUi(QDialog *MainForm)
    {
        if (MainForm->objectName().isEmpty())
            MainForm->setObjectName(QString::fromUtf8("MainForm"));
        MainForm->resize(1076, 906);
        leRoot = new QLineEdit(MainForm);
        leRoot->setObjectName(QString::fromUtf8("leRoot"));
        leRoot->setGeometry(QRect(610, 845, 141, 20));
        label = new QLabel(MainForm);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(550, 847, 51, 16));
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
        label_2 = new QLabel(MainForm);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(550, 872, 51, 16));
        leTemp = new QLineEdit(MainForm);
        leTemp->setObjectName(QString::fromUtf8("leTemp"));
        leTemp->setGeometry(QRect(610, 870, 141, 20));
        pbUpdateStruct = new QPushButton(MainForm);
        pbUpdateStruct->setObjectName(QString::fromUtf8("pbUpdateStruct"));
        pbUpdateStruct->setGeometry(QRect(20, 860, 75, 23));
        pushButton = new QPushButton(MainForm);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(100, 860, 75, 23));
        tabWidget = new QTabWidget(MainForm);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 40, 1061, 801));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        pbLocalListFiles = new QPushButton(tab);
        pbLocalListFiles->setObjectName(QString::fromUtf8("pbLocalListFiles"));
        pbLocalListFiles->setGeometry(QRect(967, 720, 81, 23));
        label_9 = new QLabel(tab);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(7, 534, 141, 16));
        label_7 = new QLabel(tab);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(537, 375, 111, 16));
        lvLocalListFiles = new QListView(tab);
        lvLocalListFiles->setObjectName(QString::fromUtf8("lvLocalListFiles"));
        lvLocalListFiles->setGeometry(QRect(537, 554, 421, 161));
        lwListModels = new QListView(tab);
        lwListModels->setObjectName(QString::fromUtf8("lwListModels"));
        lwListModels->setGeometry(QRect(7, 390, 421, 141));
        pbAddDelete = new QPushButton(tab);
        pbAddDelete->setObjectName(QString::fromUtf8("pbAddDelete"));
        pbAddDelete->setGeometry(QRect(437, 583, 81, 23));
        pbListFiles = new QPushButton(tab);
        pbListFiles->setObjectName(QString::fromUtf8("pbListFiles"));
        pbListFiles->setGeometry(QRect(437, 720, 81, 23));
        label_5 = new QLabel(tab);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(7, 375, 101, 16));
        pbClearDelete = new QPushButton(tab);
        pbClearDelete->setObjectName(QString::fromUtf8("pbClearDelete"));
        pbClearDelete->setGeometry(QRect(437, 230, 81, 23));
        pushButton_3 = new QPushButton(tab);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(967, 390, 75, 23));
        lvDeletingFiles = new QListView(tab);
        lvDeletingFiles->setObjectName(QString::fromUtf8("lvDeletingFiles"));
        lvDeletingFiles->setGeometry(QRect(7, 230, 421, 141));
        lvReceiveFiles = new QListView(tab);
        lvReceiveFiles->setObjectName(QString::fromUtf8("lvReceiveFiles"));
        lvReceiveFiles->setGeometry(QRect(537, 30, 421, 181));
        pushButton_2 = new QPushButton(tab);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(437, 390, 81, 23));
        pbClearAll = new QPushButton(tab);
        pbClearAll->setObjectName(QString::fromUtf8("pbClearAll"));
        pbClearAll->setGeometry(QRect(437, 30, 81, 23));
        pbAddReceive = new QPushButton(tab);
        pbAddReceive->setObjectName(QString::fromUtf8("pbAddReceive"));
        pbAddReceive->setGeometry(QRect(437, 553, 81, 23));
        label_4 = new QLabel(tab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(7, 210, 101, 16));
        pbStartReceive = new QPushButton(tab);
        pbStartReceive->setObjectName(QString::fromUtf8("pbStartReceive"));
        pbStartReceive->setGeometry(QRect(967, 750, 81, 23));
        lwLocalListModels = new QListView(tab);
        lwLocalListModels->setObjectName(QString::fromUtf8("lwLocalListModels"));
        lwLocalListModels->setGeometry(QRect(537, 390, 421, 141));
        pbAddSend = new QPushButton(tab);
        pbAddSend->setObjectName(QString::fromUtf8("pbAddSend"));
        pbAddSend->setGeometry(QRect(967, 552, 81, 23));
        label_10 = new QLabel(tab);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(537, 532, 141, 16));
        lvLocalDeletingFiles = new QListView(tab);
        lvLocalDeletingFiles->setObjectName(QString::fromUtf8("lvLocalDeletingFiles"));
        lvLocalDeletingFiles->setGeometry(QRect(537, 230, 421, 141));
        label_8 = new QLabel(tab);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(537, 210, 121, 16));
        pbAddLocalDelete = new QPushButton(tab);
        pbAddLocalDelete->setObjectName(QString::fromUtf8("pbAddLocalDelete"));
        pbAddLocalDelete->setGeometry(QRect(967, 582, 81, 23));
        lvListFiles = new QListView(tab);
        lvListFiles->setObjectName(QString::fromUtf8("lvListFiles"));
        lvListFiles->setGeometry(QRect(7, 554, 421, 161));
        pbStartSend = new QPushButton(tab);
        pbStartSend->setObjectName(QString::fromUtf8("pbStartSend"));
        pbStartSend->setGeometry(QRect(437, 750, 81, 23));
        label_3 = new QLabel(tab);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 10, 111, 16));
        pbClearDeleteLocal = new QPushButton(tab);
        pbClearDeleteLocal->setObjectName(QString::fromUtf8("pbClearDeleteLocal"));
        pbClearDeleteLocal->setGeometry(QRect(967, 230, 81, 23));
        lvSendingFiles = new QListView(tab);
        lvSendingFiles->setObjectName(QString::fromUtf8("lvSendingFiles"));
        lvSendingFiles->setGeometry(QRect(7, 30, 421, 181));
        pbClearReceiveAll = new QPushButton(tab);
        pbClearReceiveAll->setObjectName(QString::fromUtf8("pbClearReceiveAll"));
        pbClearReceiveAll->setGeometry(QRect(967, 30, 81, 23));
        label_6 = new QLabel(tab);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(540, 10, 111, 16));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        tabWidget->addTab(tab_3, QString());

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

        tabWidget->setCurrentIndex(0);


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
        label_2->setText(QApplication::translate("MainForm", "Temp path", 0, QApplication::UnicodeUTF8));
        leTemp->setText(QString());
        pbUpdateStruct->setText(QApplication::translate("MainForm", "\320\236\320\261\320\275\320\276\320\262\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainForm", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        pbLocalListFiles->setText(QApplication::translate("MainForm", "List models", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainForm", "\320\244\320\260\320\271\320\273\321\213 \320\274\320\276\320\264\320\265\320\273\320\270 \320\275\320\260 \321\201\320\265\321\200\320\262\320\265\321\200\320\265", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainForm", "\320\234\320\276\320\264\320\265\320\273\320\270 \320\275\320\260 \320\273\320\276\320\272\320\260\320\273\321\214\320\275\320\276\320\274", 0, QApplication::UnicodeUTF8));
        pbAddDelete->setText(QApplication::translate("MainForm", "Add Delete", 0, QApplication::UnicodeUTF8));
        pbListFiles->setText(QApplication::translate("MainForm", "List models", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainForm", "\320\234\320\276\320\264\320\265\320\273\320\270 \320\275\320\260 \321\201\320\265\321\200\320\262\320\265\321\200\320\265", 0, QApplication::UnicodeUTF8));
        pbClearDelete->setText(QApplication::translate("MainForm", "Clear Delete", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("MainForm", "Send Model", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MainForm", "Receive Model", 0, QApplication::UnicodeUTF8));
        pbClearAll->setText(QApplication::translate("MainForm", "Clear All", 0, QApplication::UnicodeUTF8));
        pbAddReceive->setText(QApplication::translate("MainForm", "Add Receive", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainForm", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214 \321\201 \321\201\320\265\321\200\320\262\320\265\321\200\320\260", 0, QApplication::UnicodeUTF8));
        pbStartReceive->setText(QApplication::translate("MainForm", "Start Rec&&Del", 0, QApplication::UnicodeUTF8));
        pbAddSend->setText(QApplication::translate("MainForm", "Add Send", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainForm", "\320\244\320\260\320\271\320\273\321\213 \320\274\320\276\320\264\320\265\320\273\320\270 \320\275\320\260 \320\272\320\273\320\270\320\265\320\275\321\202\320\265", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainForm", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214 \321\201 \320\273\320\276\320\272\320\260\320\273\321\214\320\275\320\276\320\263\320\276", 0, QApplication::UnicodeUTF8));
        pbAddLocalDelete->setText(QApplication::translate("MainForm", "Add Delete", 0, QApplication::UnicodeUTF8));
        pbStartSend->setText(QApplication::translate("MainForm", "Start Send&&Del", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainForm", "\320\237\320\265\321\200\320\265\320\264\320\260\321\202\321\214 \320\275\320\260 \321\201\320\265\321\200\320\262\320\265\321\200", 0, QApplication::UnicodeUTF8));
        pbClearDeleteLocal->setText(QApplication::translate("MainForm", "Clear Delete", 0, QApplication::UnicodeUTF8));
        pbClearReceiveAll->setText(QApplication::translate("MainForm", "Clear All", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainForm", "\320\237\321\200\320\276\321\207\320\270\321\202\320\260\321\202\321\214 \321\201 \321\201\320\265\321\200\320\262\320\265\321\200\320\260", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainForm", "\320\247\321\202\320\265\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainForm", "\320\227\320\260\320\277\320\270\321\201\321\214", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainForm", "\320\220\320\264\320\274\320\270\320\275\320\270\321\201\321\202\321\200\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainForm: public Ui_MainForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFORM_H

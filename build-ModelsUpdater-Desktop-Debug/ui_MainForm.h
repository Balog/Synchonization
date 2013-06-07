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
#include <QtGui/QGraphicsView>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTableView>
#include <QtGui/QToolButton>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainForm
{
public:
    QLineEdit *leRoot;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *leTemp;
    QTabWidget *tabWidget;
    QWidget *tab_4;
    QTreeView *tvRead;
    QPushButton *pbRead;
    QPushButton *pbRefreshRead;
    QTabWidget *tabWidget_2;
    QWidget *tab_5;
    QPlainTextEdit *pteDesRead;
    QWidget *tab_6;
    QGraphicsView *graphicsView;
    QLineEdit *leNumPreview;
    QToolButton *tbPriorPreview;
    QToolButton *tbFirstPreview;
    QToolButton *tbLastPreview;
    QToolButton *tbNextPreview;
    QPushButton *pbLoadPreviews;
    QTableView *tabvListFiles;
    QGroupBox *groupBox;
    QRadioButton *rbSourseLoc;
    QRadioButton *rbSourceServ;
    QLineEdit *leModelTitle;
    QWidget *tab_2;
    QPushButton *pbWrite;
    QTreeView *tvWrite;
    QPushButton *pbRefresh_Write;
    QTabWidget *tabWidget_3;
    QWidget *tab_7;
    QPlainTextEdit *pteDesRead_2;
    QWidget *tab_8;
    QGraphicsView *graphicsView_2;
    QLineEdit *leNumPreview_2;
    QToolButton *tbPriorPreview_2;
    QToolButton *tbFirstPreview_2;
    QToolButton *tbLastPreview_2;
    QToolButton *tbNextPreview_2;
    QPushButton *pbLoadPreviews_2;
    QLineEdit *leModelTitle_2;
    QTableView *tabvListFiles_2;
    QGroupBox *groupBox_2;
    QRadioButton *rbSourseLoc_2;
    QRadioButton *rbSourceServ_2;
    QWidget *tab_3;
    QListView *lvLogins;
    QLineEdit *le_ver_pass;
    QPushButton *pbVerPassword;
    QLabel *label_11;
    QPushButton *pbAddUser;
    QPushButton *pbEditUser;
    QPushButton *pbDelUser;
    QTreeView *tvAdminTree;
    QPushButton *pbExit;
    QSpinBox *sbPort;
    QPushButton *pbConnect;
    QLineEdit *leAddr;
    QPushButton *pbDisconnect;

    void setupUi(QDialog *MainForm)
    {
        if (MainForm->objectName().isEmpty())
            MainForm->setObjectName(QString::fromUtf8("MainForm"));
        MainForm->resize(1184, 846);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainForm->sizePolicy().hasHeightForWidth());
        MainForm->setSizePolicy(sizePolicy);
        MainForm->setMinimumSize(QSize(1184, 846));
        MainForm->setMaximumSize(QSize(1184, 846));
        leRoot = new QLineEdit(MainForm);
        leRoot->setObjectName(QString::fromUtf8("leRoot"));
        leRoot->setGeometry(QRect(720, 822, 141, 20));
        leRoot->setReadOnly(true);
        label = new QLabel(MainForm);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(660, 824, 51, 16));
        label_2 = new QLabel(MainForm);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(870, 824, 51, 16));
        leTemp = new QLineEdit(MainForm);
        leTemp->setObjectName(QString::fromUtf8("leTemp"));
        leTemp->setGeometry(QRect(930, 822, 141, 20));
        leTemp->setReadOnly(true);
        tabWidget = new QTabWidget(MainForm);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(3, 3, 1171, 815));
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setMinimumSize(QSize(1171, 815));
        tabWidget->setMaximumSize(QSize(1171, 815));
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        tvRead = new QTreeView(tab_4);
        tvRead->setObjectName(QString::fromUtf8("tvRead"));
        tvRead->setGeometry(QRect(10, 10, 541, 741));
        tvRead->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        pbRead = new QPushButton(tab_4);
        pbRead->setObjectName(QString::fromUtf8("pbRead"));
        pbRead->setGeometry(QRect(10, 760, 101, 23));
        pbRefreshRead = new QPushButton(tab_4);
        pbRefreshRead->setObjectName(QString::fromUtf8("pbRefreshRead"));
        pbRefreshRead->setGeometry(QRect(120, 760, 75, 23));
        tabWidget_2 = new QTabWidget(tab_4);
        tabWidget_2->setObjectName(QString::fromUtf8("tabWidget_2"));
        tabWidget_2->setGeometry(QRect(560, 280, 601, 471));
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        pteDesRead = new QPlainTextEdit(tab_5);
        pteDesRead->setObjectName(QString::fromUtf8("pteDesRead"));
        pteDesRead->setGeometry(QRect(3, 3, 591, 441));
        tabWidget_2->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
        graphicsView = new QGraphicsView(tab_6);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setGeometry(QRect(3, 3, 591, 411));
        leNumPreview = new QLineEdit(tab_6);
        leNumPreview->setObjectName(QString::fromUtf8("leNumPreview"));
        leNumPreview->setGeometry(QRect(270, 420, 51, 20));
        leNumPreview->setAlignment(Qt::AlignCenter);
        leNumPreview->setReadOnly(true);
        tbPriorPreview = new QToolButton(tab_6);
        tbPriorPreview->setObjectName(QString::fromUtf8("tbPriorPreview"));
        tbPriorPreview->setGeometry(QRect(240, 420, 25, 19));
        tbFirstPreview = new QToolButton(tab_6);
        tbFirstPreview->setObjectName(QString::fromUtf8("tbFirstPreview"));
        tbFirstPreview->setGeometry(QRect(210, 420, 25, 19));
        tbLastPreview = new QToolButton(tab_6);
        tbLastPreview->setObjectName(QString::fromUtf8("tbLastPreview"));
        tbLastPreview->setGeometry(QRect(360, 420, 25, 19));
        tbNextPreview = new QToolButton(tab_6);
        tbNextPreview->setObjectName(QString::fromUtf8("tbNextPreview"));
        tbNextPreview->setGeometry(QRect(330, 420, 25, 19));
        pbLoadPreviews = new QPushButton(tab_6);
        pbLoadPreviews->setObjectName(QString::fromUtf8("pbLoadPreviews"));
        pbLoadPreviews->setEnabled(false);
        pbLoadPreviews->setGeometry(QRect(518, 420, 75, 23));
        tabWidget_2->addTab(tab_6, QString());
        tabvListFiles = new QTableView(tab_4);
        tabvListFiles->setObjectName(QString::fromUtf8("tabvListFiles"));
        tabvListFiles->setGeometry(QRect(560, 40, 601, 231));
        tabvListFiles->verticalHeader()->setVisible(false);
        groupBox = new QGroupBox(tab_4);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(560, 754, 201, 32));
        rbSourseLoc = new QRadioButton(groupBox);
        rbSourseLoc->setObjectName(QString::fromUtf8("rbSourseLoc"));
        rbSourseLoc->setGeometry(QRect(10, 8, 82, 18));
        rbSourseLoc->setChecked(true);
        rbSourceServ = new QRadioButton(groupBox);
        rbSourceServ->setObjectName(QString::fromUtf8("rbSourceServ"));
        rbSourceServ->setGeometry(QRect(108, 8, 82, 18));
        leModelTitle = new QLineEdit(tab_4);
        leModelTitle->setObjectName(QString::fromUtf8("leModelTitle"));
        leModelTitle->setGeometry(QRect(560, 10, 601, 20));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        leModelTitle->setFont(font);
        leModelTitle->setReadOnly(true);
        tabWidget->addTab(tab_4, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        pbWrite = new QPushButton(tab_2);
        pbWrite->setObjectName(QString::fromUtf8("pbWrite"));
        pbWrite->setGeometry(QRect(10, 760, 101, 23));
        tvWrite = new QTreeView(tab_2);
        tvWrite->setObjectName(QString::fromUtf8("tvWrite"));
        tvWrite->setGeometry(QRect(10, 10, 541, 741));
        pbRefresh_Write = new QPushButton(tab_2);
        pbRefresh_Write->setObjectName(QString::fromUtf8("pbRefresh_Write"));
        pbRefresh_Write->setGeometry(QRect(120, 760, 75, 23));
        tabWidget_3 = new QTabWidget(tab_2);
        tabWidget_3->setObjectName(QString::fromUtf8("tabWidget_3"));
        tabWidget_3->setGeometry(QRect(560, 280, 601, 471));
        tab_7 = new QWidget();
        tab_7->setObjectName(QString::fromUtf8("tab_7"));
        pteDesRead_2 = new QPlainTextEdit(tab_7);
        pteDesRead_2->setObjectName(QString::fromUtf8("pteDesRead_2"));
        pteDesRead_2->setGeometry(QRect(3, 3, 591, 441));
        tabWidget_3->addTab(tab_7, QString());
        tab_8 = new QWidget();
        tab_8->setObjectName(QString::fromUtf8("tab_8"));
        graphicsView_2 = new QGraphicsView(tab_8);
        graphicsView_2->setObjectName(QString::fromUtf8("graphicsView_2"));
        graphicsView_2->setGeometry(QRect(3, 3, 591, 411));
        leNumPreview_2 = new QLineEdit(tab_8);
        leNumPreview_2->setObjectName(QString::fromUtf8("leNumPreview_2"));
        leNumPreview_2->setGeometry(QRect(270, 420, 51, 20));
        leNumPreview_2->setAlignment(Qt::AlignCenter);
        leNumPreview_2->setReadOnly(true);
        tbPriorPreview_2 = new QToolButton(tab_8);
        tbPriorPreview_2->setObjectName(QString::fromUtf8("tbPriorPreview_2"));
        tbPriorPreview_2->setGeometry(QRect(240, 420, 25, 19));
        tbFirstPreview_2 = new QToolButton(tab_8);
        tbFirstPreview_2->setObjectName(QString::fromUtf8("tbFirstPreview_2"));
        tbFirstPreview_2->setGeometry(QRect(210, 420, 25, 19));
        tbLastPreview_2 = new QToolButton(tab_8);
        tbLastPreview_2->setObjectName(QString::fromUtf8("tbLastPreview_2"));
        tbLastPreview_2->setGeometry(QRect(360, 420, 25, 19));
        tbNextPreview_2 = new QToolButton(tab_8);
        tbNextPreview_2->setObjectName(QString::fromUtf8("tbNextPreview_2"));
        tbNextPreview_2->setGeometry(QRect(330, 420, 25, 19));
        pbLoadPreviews_2 = new QPushButton(tab_8);
        pbLoadPreviews_2->setObjectName(QString::fromUtf8("pbLoadPreviews_2"));
        pbLoadPreviews_2->setEnabled(false);
        pbLoadPreviews_2->setGeometry(QRect(518, 420, 75, 23));
        tabWidget_3->addTab(tab_8, QString());
        leModelTitle_2 = new QLineEdit(tab_2);
        leModelTitle_2->setObjectName(QString::fromUtf8("leModelTitle_2"));
        leModelTitle_2->setGeometry(QRect(560, 10, 601, 20));
        leModelTitle_2->setFont(font);
        leModelTitle_2->setReadOnly(true);
        tabvListFiles_2 = new QTableView(tab_2);
        tabvListFiles_2->setObjectName(QString::fromUtf8("tabvListFiles_2"));
        tabvListFiles_2->setGeometry(QRect(560, 40, 601, 231));
        tabvListFiles_2->verticalHeader()->setVisible(false);
        groupBox_2 = new QGroupBox(tab_2);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(560, 754, 201, 32));
        rbSourseLoc_2 = new QRadioButton(groupBox_2);
        rbSourseLoc_2->setObjectName(QString::fromUtf8("rbSourseLoc_2"));
        rbSourseLoc_2->setGeometry(QRect(10, 8, 82, 18));
        rbSourseLoc_2->setChecked(true);
        rbSourceServ_2 = new QRadioButton(groupBox_2);
        rbSourceServ_2->setObjectName(QString::fromUtf8("rbSourceServ_2"));
        rbSourceServ_2->setGeometry(QRect(108, 8, 82, 18));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        lvLogins = new QListView(tab_3);
        lvLogins->setObjectName(QString::fromUtf8("lvLogins"));
        lvLogins->setGeometry(QRect(10, 40, 271, 671));
        QFont font1;
        font1.setPointSize(11);
        lvLogins->setFont(font1);
        le_ver_pass = new QLineEdit(tab_3);
        le_ver_pass->setObjectName(QString::fromUtf8("le_ver_pass"));
        le_ver_pass->setGeometry(QRect(60, 10, 141, 20));
        pbVerPassword = new QPushButton(tab_3);
        pbVerPassword->setObjectName(QString::fromUtf8("pbVerPassword"));
        pbVerPassword->setGeometry(QRect(208, 8, 75, 23));
        label_11 = new QLabel(tab_3);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(13, 12, 46, 13));
        pbAddUser = new QPushButton(tab_3);
        pbAddUser->setObjectName(QString::fromUtf8("pbAddUser"));
        pbAddUser->setGeometry(QRect(10, 717, 75, 23));
        pbEditUser = new QPushButton(tab_3);
        pbEditUser->setObjectName(QString::fromUtf8("pbEditUser"));
        pbEditUser->setEnabled(false);
        pbEditUser->setGeometry(QRect(100, 717, 91, 23));
        pbDelUser = new QPushButton(tab_3);
        pbDelUser->setObjectName(QString::fromUtf8("pbDelUser"));
        pbDelUser->setEnabled(false);
        pbDelUser->setGeometry(QRect(206, 717, 75, 23));
        tvAdminTree = new QTreeView(tab_3);
        tvAdminTree->setObjectName(QString::fromUtf8("tvAdminTree"));
        tvAdminTree->setGeometry(QRect(290, 40, 871, 671));
        tabWidget->addTab(tab_3, QString());
        pbExit = new QPushButton(MainForm);
        pbExit->setObjectName(QString::fromUtf8("pbExit"));
        pbExit->setGeometry(QRect(1100, 821, 75, 23));
        sbPort = new QSpinBox(MainForm);
        sbPort->setObjectName(QString::fromUtf8("sbPort"));
        sbPort->setGeometry(QRect(130, 822, 61, 22));
        sbPort->setReadOnly(true);
        sbPort->setMaximum(9999);
        sbPort->setValue(1234);
        pbConnect = new QPushButton(MainForm);
        pbConnect->setObjectName(QString::fromUtf8("pbConnect"));
        pbConnect->setEnabled(false);
        pbConnect->setGeometry(QRect(200, 822, 75, 23));
        leAddr = new QLineEdit(MainForm);
        leAddr->setObjectName(QString::fromUtf8("leAddr"));
        leAddr->setGeometry(QRect(10, 822, 113, 20));
        leAddr->setReadOnly(true);
        pbDisconnect = new QPushButton(MainForm);
        pbDisconnect->setObjectName(QString::fromUtf8("pbDisconnect"));
        pbDisconnect->setGeometry(QRect(280, 822, 81, 23));

        retranslateUi(MainForm);
        QObject::connect(lvLogins, SIGNAL(clicked(QModelIndex)), MainForm, SLOT(OnLoginsClicked(QModelIndex)));
        QObject::connect(pbAddUser, SIGNAL(clicked()), MainForm, SLOT(OnNewLogin()));
        QObject::connect(pbDelUser, SIGNAL(clicked()), MainForm, SLOT(OnDelLogin()));
        QObject::connect(pbVerPassword, SIGNAL(clicked()), MainForm, SLOT(OnVerPassword()));
        QObject::connect(pbEditUser, SIGNAL(clicked()), MainForm, SLOT(OnEditLogin()));
        QObject::connect(pbConnect, SIGNAL(clicked()), MainForm, SLOT(OnConnect()));
        QObject::connect(pbDisconnect, SIGNAL(clicked()), MainForm, SLOT(OnDisconnecting()));

        tabWidget->setCurrentIndex(2);
        tabWidget_2->setCurrentIndex(1);
        tabWidget_3->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainForm);
    } // setupUi

    void retranslateUi(QDialog *MainForm)
    {
        MainForm->setWindowTitle(QApplication::translate("MainForm", "\320\223\320\273\320\260\320\262\320\275\320\260\321\217 \321\204\320\276\321\200\320\274\320\260", 0, QApplication::UnicodeUTF8));
        leRoot->setText(QString());
        label->setText(QApplication::translate("MainForm", "Root path", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainForm", "Temp path", 0, QApplication::UnicodeUTF8));
        leTemp->setText(QString());
        pbRead->setText(QApplication::translate("MainForm", "\320\247\320\270\321\202\320\260\321\202\321\214 \321\201 \321\201\320\265\321\200\320\262\320\265\321\200\320\260", 0, QApplication::UnicodeUTF8));
        pbRefreshRead->setText(QApplication::translate("MainForm", "\320\237\321\200\320\276\320\262\320\265\321\200\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_5), QApplication::translate("MainForm", "\320\236\320\277\320\270\321\201\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        tbPriorPreview->setText(QApplication::translate("MainForm", "<", 0, QApplication::UnicodeUTF8));
        tbFirstPreview->setText(QApplication::translate("MainForm", "|<", 0, QApplication::UnicodeUTF8));
        tbLastPreview->setText(QApplication::translate("MainForm", ">|", 0, QApplication::UnicodeUTF8));
        tbNextPreview->setText(QApplication::translate("MainForm", ">", 0, QApplication::UnicodeUTF8));
        pbLoadPreviews->setText(QApplication::translate("MainForm", "\320\237\320\276\320\273\321\203\321\207\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_6), QApplication::translate("MainForm", "\320\237\321\200\320\265\320\262\321\214\321\216", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        rbSourseLoc->setText(QApplication::translate("MainForm", "\320\233\320\276\320\272\320\260\320\273\321\214\320\275\321\213\320\271", 0, QApplication::UnicodeUTF8));
        rbSourceServ->setText(QApplication::translate("MainForm", "\320\241\320\265\321\200\320\262\320\265\321\200", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("MainForm", "\320\247\321\202\320\265\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        pbWrite->setText(QApplication::translate("MainForm", "\320\237\320\270\321\201\320\260\321\202\321\214 \320\275\320\260 \321\201\320\265\321\200\320\262\320\265\321\200", 0, QApplication::UnicodeUTF8));
        pbRefresh_Write->setText(QApplication::translate("MainForm", "\320\237\321\200\320\276\320\262\320\265\321\200\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_7), QApplication::translate("MainForm", "\320\236\320\277\320\270\321\201\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        tbPriorPreview_2->setText(QApplication::translate("MainForm", "<", 0, QApplication::UnicodeUTF8));
        tbFirstPreview_2->setText(QApplication::translate("MainForm", "|<", 0, QApplication::UnicodeUTF8));
        tbLastPreview_2->setText(QApplication::translate("MainForm", ">|", 0, QApplication::UnicodeUTF8));
        tbNextPreview_2->setText(QApplication::translate("MainForm", ">", 0, QApplication::UnicodeUTF8));
        pbLoadPreviews_2->setText(QApplication::translate("MainForm", "\320\237\320\276\320\273\321\203\321\207\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_8), QApplication::translate("MainForm", "\320\237\321\200\320\265\320\262\321\214\321\216", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QString());
        rbSourseLoc_2->setText(QApplication::translate("MainForm", "\320\233\320\276\320\272\320\260\320\273\321\214\320\275\321\213\320\271", 0, QApplication::UnicodeUTF8));
        rbSourceServ_2->setText(QApplication::translate("MainForm", "\320\241\320\265\321\200\320\262\320\265\321\200", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainForm", "\320\227\320\260\320\277\320\270\321\201\321\214", 0, QApplication::UnicodeUTF8));
        pbVerPassword->setText(QApplication::translate("MainForm", "\320\237\321\200\320\276\320\262\320\265\321\200\320\272\320\260", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainForm", "\320\237\320\260\321\200\320\276\320\273\321\214", 0, QApplication::UnicodeUTF8));
        pbAddUser->setText(QApplication::translate("MainForm", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        pbEditUser->setText(QApplication::translate("MainForm", "\320\240\320\265\320\264\320\260\320\272\321\202\320\270\321\200\320\276\320\262\320\260\321\202\321\214", 0, QApplication::UnicodeUTF8));
        pbDelUser->setText(QApplication::translate("MainForm", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainForm", "\320\220\320\264\320\274\320\270\320\275\320\270\321\201\321\202\321\200\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        pbExit->setText(QApplication::translate("MainForm", "\320\222\321\213\321\205\320\276\320\264", 0, QApplication::UnicodeUTF8));
        pbConnect->setText(QApplication::translate("MainForm", "Connect", 0, QApplication::UnicodeUTF8));
        leAddr->setText(QApplication::translate("MainForm", "127.0.0.1", 0, QApplication::UnicodeUTF8));
        pbDisconnect->setText(QApplication::translate("MainForm", "Disconnect", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainForm: public Ui_MainForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFORM_H

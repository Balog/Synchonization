#ifndef MAINFORM_H
#define MAINFORM_H

#include <QDialog>

#include "tSettings.h"
//#include "tConveyor.h"
#include "tModelsConveyor.h"
#include <QMessageBox>
#include <QStringListModel>
#include "tDatabaseOp.h"
#include "tEditLoginPass.h"
#include "tConstructModelTree.h"
#include <QStandardItemModel>
#include "tNewPath.h"
#include <vector>


using namespace std;



//

namespace Ui {
class MainForm;
}


class MainForm : public QDialog
{
    Q_OBJECT
    
public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();
    void LocalListFile(const QStringList &list);
    void CancelAllOperations();
    //    void ServerListModels(QStringList &list);

    void SaveServerModelFiles(QByteArray &_block);
    QStringListModel *slm_server_list_models;//модель списка серверных моделей
    int NumCelServModel;

    void CorrectLastSynch(bool _server);
    bool GetIsTransaction()
    {
        bool tr=mod_conv->GetIsTransaction();
        return tr;
    }

    void RegisterUser(qlonglong _s_num);
    void UpdateLogins();
    void DeleteUser(qlonglong _s_num);

    void UpdateLoginsTable(QByteArray &_block);
    void TreesBuildings(QString &_login);

    void StartAutoriz();
    void UpdateModelRead(QByteArray &_block);

    bool IsRequeryServerModel;
    void OnListFiles();

    bool VerifyUserFolders();

    void BuildingReadTree(const QString &user_login);
    void VerifyLastTable(const QString& user_login);

    void ConstructTreeModel(QStandardItemModel *_tree_model, bool _read);
    void EndUpdateServerModel();

private:
    QStringListModel *sLM_loc_list_models;//модель локальных моделей
    QStringListModel *sLM_loc_list_files;//модель локальных файлов

    QStringListModel *slm_list;
    QStringListModel *sLM_server_list_files;//модель серверного списка файлов


    Ui::MainForm *ui;

    void StartServer();
    vector <QByteArray> pr;

    //    tConveyor* conv;
    tModelsConveyor *mod_conv;
    tDatabaseOp* db_op;

    QStringList listSend;
    QStringListModel *sLM_Send;

    QStringList listDel;
    QStringListModel *sLM_Del;

    QStringListModel *sLM_list;

    QStringList listRec;
    QStringListModel *sLM_Rec;

    QStringList listDelLoc;
    QStringListModel *sLM_DelLoc;

    void CreateFileList(const QString &start_folder, QStringList& list);
    void SearchFiles(const QDir &dir, QStringList &list);

    void SearchModelsOnDatabase(QStringList &_list);

    tEditLoginPass *login_pass;

    QStringList listLogins;
    QStringListModel *sLM_Logins;

    QModelIndex mi;

    QStandardItemModel *adm_tree_model;

    QModelIndex admin_logins_index;

//    tConstructModelTree *constr_mod_tree;
    void UpToParent(QModelIndex index, Qt::CheckState _state);
    void DownToChildrens(QModelIndex index, Qt::CheckState _state);



    QString user_login;
    bool modify_folder;

    tNewPath* form_new_path;

    QList<CompareTableRec> list_compare;

    typedef enum{Read, Write} tTreeMod;

    void ConstructTree(tTreeMod _tree_mod, QList<CompareTableRec> _comp_table);

    QList<CompareTableRec> tree_data;


    QStandardItemModel *read_tree_model;
    QStandardItemModel *write_tree_model;

    void UpToParentFiles(QStandardItemModel *model, const QModelIndex &index, Qt::CheckState _state);
    void DownToChildrensFiles(QStandardItemModel *model, QModelIndex index, Qt::CheckState _state);

private slots:
    void Autorization(QString &_login, QString &_password, bool _modify_folder);

    void OnConnect();
    void OnDisconnect();

    //    void EndConveyor();
    void EndTransactions();
    //    void ErrorConveyor();

    void OnAutorizStart();
    void OnSetVisible(const bool vis);
    void OnDisconnecting();

    void OnAddSend();
    void OnStartSend();
    void OnClearSendAll();

    void OnAddDelete();
    void OnClearDelete();



    void OnAddReceive();
    void OnReceiveClear();
    void OnDeleteLocal();
    void OnClearDeleteLocal();
    void OnStartReceive();

    void OnUpdateStruct();

    void OnClearModels();

    void OnLocalModelClick(const QModelIndex Ind);

    void OnNewLogin();
    void OnEditLogin();
    void OnDelLogin();

    void OnEndEditLoginPassword(QString& _login, QString& _password, int _row, bool _new_user);

    void OnLoginsClicked(QModelIndex Ind);


    void on_tvAdminTree_clicked(const QModelIndex &index);

    void on_lvLogins_clicked(const QModelIndex &index);

    void on_pbListFiles_clicked();


    void on_pbExit_clicked();

    void on_pbRefresh_clicked();

    void on_pbRead_clicked();

    void on_pbBuildRead_clicked();

    void on_tvRead_clicked(const QModelIndex &index);
    void TreeCustomCollapsed(QStandardItem *item);

public slots:
    void OnServerModelClick(const QModelIndex Ind);
    void OnListFilesLocal();
    void OnReceiveLoginsTable();
    void OnVerPassword();
    void OnContinueStart();

signals:
    void RunGui(QByteArray& block);

    void NextCommand();
    void FindServer();
    void AutorizStart();
    void Disconnecting();

    void StartAutorizForm();

};

#endif // MAINFORM_H

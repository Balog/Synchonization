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
//#include "qivbcheckindicatordelegate.h"
#include "tProgress.h"
#include <QImage>
#include <vector>


using namespace std;

//class tLoginRow
//{
//public:
//    tLoginRow();
//    QString login;
//    bool writable;
//};


//class tLoginRowModel : public QAbstractTableModel
//{
//    Q_OBJECT
//public:
//    explicit tLoginRowModel(QObject *parent=0);

//    QList<tLoginRow*> list;
//    QStringList header_data;

//    QVariant data(const QModelIndex &index, int role) const;
//    bool setData(const QModelIndex &index, const QVariant &value, int role);

//    int rowCount(const QModelIndex &parent=QModelIndex()) const;
//    int columnCount(const QModelIndex &parent) const;

//    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
//    Qt::ItemFlags flags(const QModelIndex &index) const;

//signals:

//public slots:

//};


class TableModel : public QStandardItemModel
{
public:
TableModel(){}
virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
};







//

namespace Ui {
class MainForm;
}


class MainForm : public QDialog
{
    Q_OBJECT
private:
typedef enum{Read, Write} tTreeMod;

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

    void BuildingTree(const QString &user_login);
    void VerifyLastTable(const QString& user_login);

    void ConstructTreeModel(QStandardItemModel *_tree_model, bool _read);
    void EndUpdateServerModel();

    void InternalCallBuildingTree();

    void ViewError(int num_error, QString& error, QString &detail, QString& client_detail);
    void  MainForm::DisplayInfo(qlonglong loc_num, qlonglong serv_num);
//    QModelIndex CurrentReadIndex;
//    QModelIndex CurrentWriteIndex;


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

    QStandardItemModel* tableModel;

//    QStandardItemModel *table_logins_model;
//    tLoginRowModel *tab_log_model;

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



    void ConstructTree(tTreeMod _tree_mod, QList<CompareTableRec> _comp_table);

    QList<CompareTableRec> tree_data;


    QStandardItemModel *read_tree_model;

    QStandardItemModel *write_tree_model;


    void UpToParentFiles(QStandardItemModel *model, const QModelIndex &index, Qt::CheckState _state);
    void DownToChildrensFiles(QStandardItemModel *model, QModelIndex index, Qt::CheckState _state, tTreeMod _direction);
    void SetToModelsTreeData(qlonglong loc_num_model, qlonglong _serv_num_model, bool _to_read, bool _choice);
    void StartReadModeles(const QString &_root, qlonglong _server_num_model,const bool _is_preview);
    void ShowContextMenu(QPoint pos, bool _read);
    void DisplayModelInfo(qlonglong loc_num, qlonglong serv_num);

    int max_models;

    tProgress *fProgress;

    QImage Image;

    QStandardItemModel *table_files_model;
    int num_preview;
    void DisplayPreview(const QStringList &_previews);
    QStringList previews;
    qlonglong current_local_model;
    void GetLoadedPreview(const qlonglong serv_num, QStringList &previews);
    bool removeFolder(const QDir & _dir, const bool _del_dir);
    void EditingModelFile(QString &file_name, QString &text);

    qlonglong current_local_num;
    qlonglong current_server_num;

    void SearchIndex(qlonglong current_local_num, qlonglong current_server_num, QModelIndex &ReadIndex, QModelIndex &WriteIndex);
//    QModelIndex SearchIndexToModel(qlonglong current_local_num, qlonglong current_server_num, QStandardItemModel &read_model);
    QModelIndex SearchIndexToModel(qlonglong current_local_num, qlonglong current_server_num, QStandardItemModel *_model);
    QStandardItem* SearchItemToModel(qlonglong current_local_num, qlonglong current_server_num, QStandardItem *item, bool &stop);

    void RecoveryTreeIndex();

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

//    void on_pbRefresh_clicked();

    void on_pbRead_clicked();

    void on_pbBuildRead_clicked();

    void on_tvRead_clicked(const QModelIndex &index);
    void TreeCustomCollapsed(QStandardItem *item, tTreeMod _tree_mod);

//    void on_pbRefresh_2_clicked();

    void on_pbRefreshRead_clicked();

    void on_pbRefresh_Write_clicked();

    void on_pbBuildWrite_clicked();

    void on_tvWrite_clicked(const QModelIndex &index);

    void on_pbWrite_clicked();

//    void showContextMenuRead(QPoint pos);
//    void showContextMenuWrite(QPoint pos);



    void on_tvRead_customContextMenuRequested(const QPoint &pos);

    void on_tvWrite_customContextMenuRequested(const QPoint &pos);

//    void OnClickAddPicture();

    void on_tbFirstPreview_clicked();

    void on_tbPriorPreview_clicked();

    void on_tbNextPreview_clicked();


    void on_tbLastPreview_clicked();

    void on_pteDesRead_textChanged();

    void on_rbSourseLoc_clicked();

    void on_rbSourceServ_clicked();

    void on_pbLoadPreviews_clicked();




    void on_tbFirstPreview_2_clicked();

    void on_tbPriorPreview_2_clicked();

    void on_tbNextPreview_2_clicked();

    void on_tbLastPreview_2_clicked();

    void on_pbLoadPreviews_2_clicked();

    void on_rbSourseLoc_2_clicked();

    void on_rbSourceServ_2_clicked();

    void on_pteDesRead_2_textChanged();

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

    void ProgressStart(int _max, int x, int y, int w, int h);
    void ProgressStop();

};

#endif // MAINFORM_H

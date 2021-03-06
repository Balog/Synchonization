#ifndef MAINFORM_H
#define MAINFORM_H

#include <QDialog>

#include "tSettings.h"
#include "tModelsConveyor.h"
#include <QMessageBox>
#include <QStringListModel>
#include "tDatabaseOp.h"
#include "tEditLoginPass.h"
#include "tConstructModelTree.h"
#include <QStandardItemModel>
#include "tNewPath.h"
#include "tProgress.h"
#include <QImage>
#include <vector>


using namespace std;

class TableModel : public QStandardItemModel
{
public:
    TableModel(){}
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
};
//*************************************************************
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
    void CancelAllOperations();

    void SaveServerModelFiles(QByteArray &_block);
    int NumCelServModel;

    void CorrectLastSynch(const bool _server);
    bool GetIsTransaction()
    {
        bool tr=mod_conv->GetIsTransaction();
        return tr;
    }

    void RegisterUser(const qlonglong _s_num);
    void UpdateLogins();
    void DeleteUser(const qlonglong _s_num);

    void UpdateLoginsTable(QByteArray &_block);
    void TreesBuildings(const QString &_login);

    void StartAutoriz();
    void UpdateModelRead(QByteArray &_block);

    bool IsRequeryServerModel;
    void OnListFiles();

    bool VerifyUserFolders() const;

    void BuildingTree(const QString &_user_login);
    void VerifyLastTable(const QString& _user_login);

    void ConstructTreeModel(const QStandardItemModel *_tree_model, const bool _read);
    void EndUpdateServerModel(const bool _rebuild);

    void InternalCallBuildingTree();

    void ViewError(const int _num_error, const QString &_error, const QString &_detail, const QString &_client_detail);
    void  MainForm::DisplayInfo(const qlonglong _loc_num, const qlonglong _serv_num);

private:
    QStringListModel *sLM_loc_list_files;//модель локальных файлов
    Ui::MainForm *ui;

    void StartServer();
    vector <QByteArray> pr;

    tModelsConveyor *mod_conv;
    tDatabaseOp* db_op;

    QStringList listSend;

    QStringList listDel;

    QStringListModel *sLM_list;

    QStringList listRec;

    QStringList listDelLoc;

    void CreateFileList(const QString &_start_folder, QStringList& _list);
    void SearchFiles(const QDir &_dir, QStringList &_list);

    void SearchModelsOnDatabase(QStringList &_list);

    tEditLoginPass *login_pass;

    QStringList listLogins;
    QStringListModel *sLM_Logins;

    QStandardItemModel* tableModel;

    QModelIndex mi;

    QStandardItemModel *adm_tree_model;

    QModelIndex admin_logins_index;

    void UpToParent(const QModelIndex _index, const Qt::CheckState _state);
    void DownToChildrens(const QModelIndex _index, const Qt::CheckState _state);

    QString _user_login;
    bool modify_folder;

    tNewPath* form_new_path;

    QList<CompareTableRec> list_compare;

    void ConstructTree(const tTreeMod _tree_mod, QList<CompareTableRec> _comp_table);

    QList<CompareTableRec> tree_data;


    QStandardItemModel *read_tree_model;

    QStandardItemModel *write_tree_model;


    void UpToParentFiles(const QStandardItemModel *_model, const QModelIndex &_index, const Qt::CheckState _state);
    void DownToChildrensFiles(const QStandardItemModel *_model, const QModelIndex _index, const Qt::CheckState _state, const tTreeMod _direction);
    void SetToModelsTreeData(const qlonglong _loc_num_model, const qlonglong _serv_num_model, const bool _to_read, const bool _choice);
    void StartReadModeles(const QString &_root, const qlonglong _server_num_model, const bool _is_preview);
    void ShowContextMenu(const QPoint _pos, const bool _read);
    void DisplayModelInfo(const qlonglong _loc_num, const qlonglong _serv_num);

    int max_models;

    tProgress *fProgress;

    QImage Image;

    QStandardItemModel *table_files_model;
    int num_preview;
    void DisplayPreview(const QStringList &_previews);
    QStringList previews;
    qlonglong current_local_model;
    void GetLoadedPreview(const qlonglong _serv_num, QStringList &_previews);
    bool removeFolder(const QDir & _dir, const bool _del_dir);
    void EditingModelFile(const QString &_file_name, const QString &_text, QString& _hash, QDateTime &_dt, qint64 &_size);

    qlonglong _current_local_num;
    qlonglong _current_server_num;

    void SearchIndex(const qlonglong _current_local_num, const qlonglong _current_server_num, QModelIndex &_read_index, QModelIndex &_write_index);

    QModelIndex SearchIndexToModel(const qlonglong _current_local_num, const qlonglong _current_server_num, const QStandardItemModel *_model);
    QStandardItem* SearchItemToModel(const qlonglong _current_local_num, const qlonglong _current_server_num, QStandardItem *_item, bool &_stop);

    void RecoveryTreeIndex();
    void SaveDescriptionModel(const QString &_text);

private slots:
    void Autorization(QString &_login, QString &_password, bool _modify_folder);

    void OnConnect();
    void OnDisconnect();

    void EndTransactions();

    void OnAutorizStart();
    void OnSetVisible(const bool _vis);
    void OnDisconnecting();

    void OnStartSend();
    void OnClearSendAll();

    void OnClearDelete();

    void OnReceiveClear();
    void OnClearDeleteLocal();
    void OnStartReceive();

    void OnClearModels();

    void OnNewLogin();
    void OnEditLogin();
    void OnDelLogin();

    void OnEndEditLoginPassword(QString &_login, QString &_password, int _row, bool _new_user);

    void OnLoginsClicked(const QModelIndex _ind);


    void on_tvAdminTree_clicked(const QModelIndex &index);



//    void on_pbListFiles_clicked();


    void on_pbExit_clicked();

    void on_pbRead_clicked();

    void on_tvRead_clicked(const QModelIndex &_index);
    void TreeCustomCollapsed(const QStandardItem *_item, const tTreeMod _tree_mod);

    void on_pbRefreshRead_clicked();

    void on_pbRefresh_Write_clicked();

    void on_tvWrite_clicked(const QModelIndex &_index);

    void on_pbWrite_clicked();

    void on_tvRead_customContextMenuRequested(const QPoint &_pos);

    void on_tvWrite_customContextMenuRequested(const QPoint &_pos);

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

void on_lvLogins_customContextMenuRequested(const QPoint &_pos);

public slots:

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

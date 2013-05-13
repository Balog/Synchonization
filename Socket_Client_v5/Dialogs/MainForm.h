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

    void UpfateLoginsTable(QByteArray &_block);
    void TreesBuildings();

    void StartAutoriz();

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

//    tConstructModelTree *constr_mod_tree;

private slots:
    void Autorization(QString &_login, QString &_password);

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

    void OnListFiles();

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


public slots:
    void OnServerModelClick(const QModelIndex Ind);
    void OnListFilesLocal();
    void OnReceiveLoginsTable();
    void OnVerPassword();

signals:
    void RunGui(QByteArray& block);

    void NextCommand();
    void FindServer();
    void AutorizStart();
    void Disconnecting();

    void StartAutorizForm();

};

#endif // MAINFORM_H

#ifndef MAINMODULE_H
#define MAINMODULE_H

#include <QObject>
#include "tModelsConveyor.h"
#include "tDatabaseOp.h"
#include "tModels.h"
//#include "tZastModule.h"


class MainModule : public QObject
{
    Q_OBJECT
private:

    tDatabaseOp* db_op;
    tModelsConveyor* mod_conv;
//    tZastModule* zast_mod;
    QString user_login;
    bool modify_folder;
    QStringList listSend;
    QStringList listDel;
    QStringList listRec;
    QStringList listDelLoc;
    QList<CompareTableRec> list_compare;
    tLog log;

    QTimer* timer;
    QTimer* timer1;

    QString login;//login_pass->GetLogin();
    QString password;//login_pass->GetPassword();
    bool new_user;//login_pass->new_user;

    QStringList ListErrors;
    void ParsingServerModels(QByteArray &_block);
    QList<tServerModel> Models;


public:
    MainModule(QObject *parent = 0);
    ~MainModule();
    void StartDatabase(bool start);
    bool GetIsTransaction();
    void ViewError(const int, const QString &, const QString &, const QString &);
    void UpdateLogins();
    bool VerifyUserFolders();
    bool VerifyUserFolders(QString& _login, QString& _pr_folder, QString& _temp_folder, QString& _message);
    void CancelAllOperations();
    void OnListFiles();
    void OnListFiles(const QStringList &models);
    void VerifyLastTable(const QString& user_login);
    void SaveServerModelFiles(QByteArray &_block);
    bool IsRequeryServerModel;
    void CorrectLastSynch(const bool _server);
    void EndUpdateServerModel(const bool _rebuild);
    void BuildingTree(const QString &user_login);
    void RegisterUser(const qlonglong _s_num);
    void DeleteUser(const qlonglong _s_num);
    void UpdateLoginsTable(QByteArray &_block);
    void StartAutoriz();
    void UpdateModelRead(QByteArray &_block);
    void InternalCallBuildingTree();
//    tDatabaseOp* GetDatabase();
    QString GetTemp();
    void GetModelInfo(const qlonglong _loc_num, QString& _title_model, QString &_description, QList<tFile> &_files_model, QStringList& _previews);
    QString LoginFromNum(const qlonglong _num_login) const;
    QString GetServerModelPath(const qlonglong _num_server) const;
    void GetServerModelInfo(const qlonglong _serv_num, QString& _title_model, QString &_description, QList<tFile> &_files_model);
    void SaveDescription(const qlonglong _num, const QString &_text, QString &_file_name);
    void UpdateInfoData(const qlonglong _num_model, const QString &_file_name, const QString &_hash, const QDateTime &_last_mod);
    QString GetRoot();
    void WriteToCompareTablesToTree(const QString& _login);
    QList<CompareTableRec> AnalyzeCompareAll();
    void AddFilesToModelsStruct(QList<CompareTableRec> &comp_models);
    void SetTransactionFlag(const bool _flag);
    void ClearConveyor();
    void DeletingLocalFile(const QString& _file_name);
    void ReceiveFile(const QString& _file_name);
    void GetServerListPreviews(const qlonglong _server_num_model, QStringList &_list);
    void GetServerListFiles(const qlonglong _num_server_model, QStringList &_list);
    int GetCountRecDelModels();
    void StartReceiveDeleteFiles(const QString &_root, int _custom_copy, int max_model);
    void DeletingServerFile(const QString &_file_name);
    void SendFile(const QString &_file_name);
    int GetCountSendDelModels();
    void StartSendDeleteFiles(const int _max_model);
    void ActualiseModel(const QString &_login, const qlonglong _num_model, const bool _from_server);
    QString VerifyCustomCopyPath(const QString& path) const;
    void OnRunGui(QByteArray& block);

    void SetServerParameters(const QString& _addr, const int _port);
    void StartServer();
    void GetServerParameters(QString &_addr, int& _port);
    void OnDisconnectingFromClient();
    void GetFolderParameters(const QString& login, QString& roor_folder, QString& temp_folder, QString& mess);
    void SaveFoldersToLoginsTable(const QString &_login, const QString &_pr_folder, const QString &_temp_folder);
    void SaveFoldersToSettings(const QString &_user_login);
    void GetPermissionsUser(const QString& _login, bool &_is_admin_user, bool &_is_writable_user);
    void RefreshModelsFiles();
    bool removeFolder(const QDir & _dir, const bool _del_dir);
    void SendDeleteLogin(int _num);
    void SaveLoginWritable(const QStandardItemModel *_model, const int _row);
    void GetLoginsModel(QStandardItemModel *model);
    bool VerPassword(const QString &login, const QString &_pass);
    void SavePermissionsToServer(const qlonglong _num_login);
    qlonglong GetNumLogin(const int _row) const;
    qlonglong GetNumLogin(const QString &_login) const;
    void SaveReadPermission(const QString &_login, const qlonglong _mod_num, const bool _state);
    void ResetFoundModelAdmin();
    bool NextModelAdmin() const;
    QStringList NextStructListModelAdmin(const QString &_login, bool &_read, qlonglong &_server_num) const;
    void SendLoginPassword(const QString &_login, const QString &_password, const int _row, const bool _new_user);

    QStringList GetListErrors() const;
    void ClearListErrors();
    void AddError(const QString &error);


    void GetAutorizationInfo(QString& _login, QString& _password);
    QStringList ReadAutoUserModels();

signals:
    void Update_Logins();//сигнал в главную форму на обновление модели представления логинов
    void ErrorUserFolders(QString& user_login, QString& message);//сигнал форме задания путей что пути у пользователя неверны
    void RunGui(QByteArray& block);//Запуск команды конвейера из ГУИ
    void EndUpdatingFromServer(QList<CompareTableRec> list_compare, bool _rebuild);//Окончание обновлений с сервера, можно перестраивать дерево разницы моделей
    void ShowEditLogin(bool visible, bool modal);//сигнал в форму редактирования логинов на очистку и скрытие формы
    void StartAutorizForm();//сигнал о том что можно показывать форму авторизации при запуске
    void VisibleMain(bool visible);//Сигнал о том что нужно изменить видимость главной формы
    void RebuildTrees(QList<CompareTableRec> list_compare);//Сигнал о том что нужно перестроить деревья разницы
    void FindServer(bool ok);//Сигнал о том что сервер обнаружен или нет
    void SignalCountFiles(int _value);
    void EndTransactions();
    void Disconnecting();
    void DisconnectingFromServer();
    void ErrorFolders(QString& error);
    void FoldersOk();
    void SendModels(QList<tServerModel> Models);

    
public slots:
    void OnContinueStart();
    void OnListFilesLocal();
    void OnAutorizStart();
    void OnSendAutorization(QString& _login, QString& _password, bool _mod_folder);
    void setValue(int _value);


private slots:
    void OnClearSendAll();
    void OnClearDelete();
    void OnReceiveClear();
    void OnClearDeleteLocal();
    void OnFindServerFalse();
    void ContinueStart();
    void OnEndTransactions();

    void OnDisconnectingFromServer();
};

#endif // MAINMODULE_H

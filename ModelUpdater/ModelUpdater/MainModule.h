#ifndef MAINMODULE_H
#define MAINMODULE_H

#include <QObject>
#include"tModelsConveyor.h"
#include "tDatabaseOp.h"
#include "tZastModule.h"


class MainModule : public QObject
{
    Q_OBJECT
private:

    tDatabaseOp* db_op;
    tModelsConveyor* mod_conv;
    tZastModule* zast_mod;
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


public:
    MainModule(QObject *parent = 0);
    ~MainModule();
    bool GetIsTransaction();
    void ViewError(const int _num_error, const QString &_error, const QString &_detail, const QString &_client_detail);
    void UpdateLogins();
    bool VerifyUserFolders();
    void CancelAllOperations();
    void OnListFiles();
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




signals:
    void Update_Logins();//сигнал в главную форму на обновление модели представления логинов
    void ErrorUserFolders(tDatabaseOp* db_op, QString& user_login, QString& message, bool visible);//сигнал форме задания путей что пути у пользователя неверны
    void RunGui(QByteArray& block);//Запуск команды конвейера из ГУИ
    void EndUpdatingFromServer(QList<CompareTableRec> list_compare, bool _rebuild);//Окончание обновлений с сервера, можно перестраивать дерево разницы моделей
    void ShowEditLogin(bool visible, bool modal);//сигнал в форму редактирования логинов на очистку и скрытие формы
    void StartAutorizForm();//сигнал о том что можно показывать форму авторизации при запуске
    void VisibleMain(bool visible);//Сигнал о том что нужно изменить видимость главной формы
    void RebuildTrees(QList<CompareTableRec> list_compare);//Сигнал о том что нужно перестроить деревья разницы
    void FindServer(bool ok);//Сигнал о том что сервер обнаружен или нет

    
public slots:
    void OnContinueStart();
    void OnListFilesLocal();
    void OnAutorizStart();
    void OnSendAutorization(QString& _login, QString& _password, bool _mod_folder);

private slots:
    void OnClearSendAll();
    void OnClearDelete();
    void OnReceiveClear();
    void OnClearDeleteLocal();
    void OnFindServerFalse();
    void ContinueStart();
};

#endif // MAINMODULE_H

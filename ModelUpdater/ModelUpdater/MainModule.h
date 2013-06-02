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
    QString _user_login;
    bool modify_folder;
    QStringList listSend;
    QStringList listDel;
    QStringList listRec;
    QStringList listDelLoc;
    QList<CompareTableRec> list_compare;
    tLog log;


public:
    MainModule(QObject *parent = 0);
    ~MainModule();
    bool GetIsTransaction();
    void ViewError(const int _num_error, const QString &_error, const QString &_detail, const QString &_client_detail);
    void UpdateLogins();
    bool VerifyUserFolders();
    void CancelAllOperations();
    void OnListFiles();
    void VerifyLastTable(const QString& _user_login);
    void SaveServerModelFiles(QByteArray &_block);
    bool IsRequeryServerModel;
    void CorrectLastSynch(const bool _server);
    void EndUpdateServerModel(const bool _rebuild);
    void BuildingTree(const QString &_user_login);
    void RegisterUser(const qlonglong _s_num);
    void DeleteUser(const qlonglong _s_num);
    void UpdateLoginsTable(QByteArray &_block);
    void StartAutoriz();
    void UpdateModelRead(QByteArray &_block);
    void InternalCallBuildingTree();




signals:
    void Update_Logins();//сигнал в главную форму на обновление модели представления логинов
    void ErrorUserFolders(tDatabaseOp* db_op, QString& _user_login, QString& message, bool visible);//сигнал форме задания путей что пути у пользователя неверны
    void RunGui(QByteArray& block);//Запуск команды конвейера из ГУИ
    void EndUpdatingFromServer(QList<CompareTableRec> list_compare, bool _rebuild);//Окончание обновлений с сервера, можно перестраивать дерево разницы моделей
    void ShowEditLogin(bool visible, bool modal);//сигнал в форму редактирования логинов на очистку и скрытие формы
    void StartAutorizForm();//сигнал о том что можно показывать форму авторизации при запуске
    void VisibleMain(bool visible);//Сигнал о том что нужно изменить видимость главной формы
    void RebuildTrees(QList<CompareTableRec> list_compare);//Сигнал о том что нужно перестроить деревья разницы
    void FindServerTrue();//Сигнал о том что сервер обнаружен

    
public slots:
    void OnContinueStart();
    void OnListFilesLocal();
    void OnAutorizStart();

private slots:
    void OnClearSendAll();
    void OnClearDelete();
    void OnReceiveClear();
    void OnClearDeleteLocal();
};

#endif // MAINMODULE_H

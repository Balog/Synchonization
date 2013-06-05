#ifndef MAINFORM_H
#define MAINFORM_H

#include <QDialog>
#include "Zast.h"
#include "../ModelUpdater/ModelUpdater/tExportMain.h"
#include "Autoriz.h"
#include "tNewPath.h"
#include "tProgress.h"

namespace Ui {
class MainForm;
}

class MainForm : public QDialog
{
    Q_OBJECT


public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();

private:
    typedef enum{Read, Write} tTreeMod;

    Ui::MainForm *ui;
    tExportMain *main;
    Zast *zast;
    Autoriz* autoriz;
    QString user_login;
    bool modify_folder;
    tNewPath* form_new_path;
    tProgress *fProgress;

    void ConstructTree(const tTreeMod _tree_mod, QList<CompareTableRec> _comp_table);
    void RecoveryTreeIndex();
    void DisplayModelInfo(const qlonglong _loc_num, const qlonglong _serv_num);
    QList<CompareTableRec> tree_data;
    QStandardItemModel *read_tree_model;

    QStandardItemModel *write_tree_model;
    void ConstructTreeModel(const QStandardItemModel *_tree_model, const bool _read);
    void TreeCustomCollapsed(const QStandardItem *_item, const tTreeMod _tree_mod);
    qlonglong _current_local_num;
    qlonglong _current_server_num;
    void SearchIndex(const qlonglong _current_local_num, const qlonglong _current_server_num, QModelIndex &_read_index, QModelIndex &_write_index);
    qlonglong current_local_model;
    QStringList previews;
    QStandardItemModel *table_files_model;
    int num_preview;
    void DisplayPreview(const QStringList &_previews);
    void GetLoadedPreview(const qlonglong _serv_num, QStringList &_previews);
    QModelIndex SearchIndexToModel(const qlonglong _current_local_num, const qlonglong _current_server_num, const QStandardItemModel *_model);
    QImage Image;
    QStandardItem* SearchItemToModel(const qlonglong _current_local_num, const qlonglong _current_server_num, QStandardItem *_item, bool &_stop);
    void UpToParentFiles(const QStandardItemModel *_model, const QModelIndex &_index, const Qt::CheckState _state);
    void DownToChildrensFiles(const QStandardItemModel *_model, const QModelIndex _index, const Qt::CheckState _state, const tTreeMod _direction);
    void SaveDescriptionModel(const QString &_text);
    void  DisplayInfo(const qlonglong _loc_num, const qlonglong _serv_num);
    void SetToModelsTreeData(const qlonglong _loc_num_model, const qlonglong _serv_num_model, const bool _to_read, const bool _choice);
    void EditingModelFile(const QString &_file_name, const QString &_text, QString& _hash, QDateTime &_dt, qint64 &_size);
    void EndUpdateServerModel(const bool _rebuild);
    void BuildingTree(const QString &user_login);
    QList<CompareTableRec> list_compare;
    int max_models;
    void StartReadModeles(const QString &_root, const qlonglong _server_num_model, const bool _is_preview);
    bool IsRequeryServerModel;
    void OnListFiles();
    void ShowContextMenu(const QPoint _pos, const bool _read);


private slots:
    void OnFindServer(bool ok);
    void OnSendAutorization(QString& _login, QString& _password, bool _modify_folder);
    void OnEndUpdatingFromServer(QList<CompareTableRec> _list_compare,bool _rebuild);

    void on_tvRead_clicked(const QModelIndex &_index);

    void on_tvWrite_clicked(const QModelIndex &_index);

    void on_pbRead_clicked();
    void EndTransactions();

    void on_pbWrite_clicked();

    void on_tvWrite_customContextMenuRequested(const QPoint &_pos);

    void on_tvRead_customContextMenuRequested(const QPoint &_pos);

    void on_pbRefreshRead_clicked();

    void on_pbRefresh_Write_clicked();
    void OnRebuildTrees(QList<CompareTableRec> _list);

public slots:
//    void OnretEndUpdateServerModel(bool _rebuild);



signals:
    void FindServer(bool ok);
    void ZastVisible(bool vis);
    void SendAutorization(QString& _login, QString& _password, bool _mod_folder);
    void ProgressStart(int _max, int x, int y, int w, int h);
    void ProgressStop();
    void RunGui(QByteArray& block);

};

#endif // MAINFORM_H

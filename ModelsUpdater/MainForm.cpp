#include "MainForm.h"
#include "ui_MainForm.h"
#include <QMessageBox>

MainForm::MainForm(QWidget *parent) :
    ui(new Ui::MainForm), QDialog(parent,Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint),
    main(new tExportMain), zast( new Zast), autoriz(new Autoriz), form_new_path(new tNewPath),
    read_tree_model(new QStandardItemModel()), write_tree_model(new QStandardItemModel()),
    table_files_model(NULL), previews(NULL)
{

    ui->setupUi(this);
    this->setVisible(false);
    connect(this, SIGNAL(ZastVisible(bool)), zast, SLOT(FormVisible(bool)));
    connect(main, SIGNAL(FindServer(bool)), this, SLOT(OnFindServer(bool)));
    connect(this, SIGNAL(FindServer(bool)), zast, SLOT(OnFindServer(bool)));
    connect(main, SIGNAL(StartAutorizForm()), autoriz, SLOT(OnStartAutorizForm()));
    connect(autoriz, SIGNAL(SendAutorization(QString&,QString&,bool)),this, SLOT(OnSendAutorization(QString&,QString&,bool)));
    connect(this, SIGNAL(SendAutorization(QString&,QString&,bool)),main, SLOT(OnSendAutorization(QString&,QString&,bool)));
    connect(main, SIGNAL(EndUpdatingFromServer(QList<CompareTableRec>,bool)), this, SLOT(OnEndUpdatingFromServer(QList<CompareTableRec>,bool)));

    emit ZastVisible(true);
}
//--------------------------------------------------------------------------------
MainForm::~MainForm()
{
    delete zast;
    delete main;
    delete ui;

}
//---------------------------------------------------------------------
void MainForm::OnFindServer(bool ok)
{

    emit FindServer(ok);
}
//---------------------------------------------------------------------
void MainForm::OnSendAutorization(QString& _login, QString& _password, bool _modify_folder)
{
    user_login=_login;
    this->setWindowTitle(QString::fromUtf8(QString("Главная форма (Пользователь '"+user_login+"')").toAscii()));
    modify_folder=_modify_folder;
    emit SendAutorization(_login, _password, _modify_folder);
}
//---------------------------------------------------------------------
void MainForm::OnEndUpdatingFromServer(QList<CompareTableRec> _list_compare,bool _rebuild)
{
    qDebug() << "Возврат в главную с таблицей разницы моделей";

    this->setVisible(true);
    ConstructTree(Read, _list_compare);
    ConstructTree(Write, _list_compare);
    if(_rebuild)
    {

        RecoveryTreeIndex();
    }
}
//---------------------------------------------------------------------
void MainForm::ConstructTree(const tTreeMod _tree_mod, QList<CompareTableRec> _comp_table)
{
    tree_data=_comp_table;

    switch (_tree_mod)
    {
    case Read:
    {
        read_tree_model->clear();

        ConstructTreeModel(read_tree_model, true);

        ui->tvRead->setModel(read_tree_model);
        ui->tvRead->setHeaderHidden(true);
        ui->tvRead->collapseAll();

        TreeCustomCollapsed(read_tree_model->invisibleRootItem(), _tree_mod);

        ui->tvRead->setRootIsDecorated(true);
        ui->tvRead->setAnimated(true);
        break;
    }
    case Write:
    {
        write_tree_model->clear();

        ConstructTreeModel(write_tree_model, false);

        ui->tvWrite->setModel(write_tree_model);
        ui->tvWrite->setHeaderHidden(true);
        ui->tvWrite->collapseAll();

        TreeCustomCollapsed(write_tree_model->invisibleRootItem(), _tree_mod);

        ui->tvWrite->setRootIsDecorated(true);
        ui->tvWrite->setAnimated(true);
        break;
    }
    }


}
//----------------------------------------------------------
void MainForm::RecoveryTreeIndex()
{
    QModelIndex ReadIndex=QModelIndex();
    QModelIndex WriteIndex=QModelIndex();

    SearchIndex(_current_local_num, _current_server_num, ReadIndex, WriteIndex);

    ui->tvRead->setCurrentIndex(ReadIndex);
    ui->tvWrite->setCurrentIndex(WriteIndex);

    if(ui->tabWidget->currentIndex()==0)
    {
        if(ReadIndex==QModelIndex())
        {
            DisplayModelInfo(0,0);
        }
        else
        {
            DisplayModelInfo(_current_local_num, _current_server_num);
        }
    }
    else
    {
        if(WriteIndex==QModelIndex())
        {
            DisplayModelInfo(0,0);
        }
        else
        {
            DisplayModelInfo(_current_local_num, _current_server_num);
        }
    }

}
//----------------------------------------------------------
void MainForm::DisplayModelInfo(const qlonglong _loc_num, const qlonglong _serv_num)
{
    //мне понадобятся:
    //Название модели
    //Description
    //Список файлов (структура Название, размер, дата модификации, кто модифицировал)
    //список путей к превью


    current_local_model=_loc_num;
    QString title_model="";
    QString description="";
    QList<tFile> files_model;

    if(ui->rbSourseLoc->isChecked())
    {
        //локальные данные
        if(_loc_num!=0)
        {
            ui->pteDesRead->setReadOnly(false);
            ui->pteDesRead_2->setReadOnly(false);
        }
        else
        {
            ui->pteDesRead->setReadOnly(true);
            ui->pteDesRead_2->setReadOnly(true);
        }

        main->GetModelInfo(_loc_num, title_model, description, files_model, previews);

        ui->leModelTitle->setText(QString("Модель: ")+title_model);
        ui->leModelTitle_2->setText(QString("Модель: ")+title_model);
        ui->pteDesRead->setPlainText(description);
        ui->pteDesRead_2->setPlainText(description);


        delete table_files_model;
        table_files_model= new QStandardItemModel(files_model.size(),3,this);
        for(int i=0; i<files_model.size();i++)
        {
            QModelIndex i_col_name=table_files_model->index(i, 0, QModelIndex());
            QModelIndex i_col_size=table_files_model->index(i, 1, QModelIndex());
            QModelIndex i_col_last_mod=table_files_model->index(i, 2, QModelIndex());

            table_files_model->setData(i_col_name, files_model[i].file_name);

            table_files_model->setData(i_col_size, QString::number(files_model[i].size)+" b");

            table_files_model->setData(i_col_last_mod, files_model[i].last_mod);

            table_files_model->item(i,0)->setEditable(false);
            table_files_model->item(i,1)->setEditable(false);
            table_files_model->item(i,2)->setEditable(false);
        }
        QStringList header;
        header << QString("Имя файла") << QString("Размер") << QString("Изменен");
        table_files_model->setHorizontalHeaderLabels(header);

        ui->tabvListFiles->setModel(table_files_model);
        ui->tabvListFiles->setColumnWidth(0,(ui->tabvListFiles->width()-20)*0.5);
        ui->tabvListFiles->setColumnWidth(1,(ui->tabvListFiles->width()-20)*0.2);
        ui->tabvListFiles->setColumnWidth(2,(ui->tabvListFiles->width()-20)*0.3);
        ui->tabvListFiles->setSelectionBehavior(QAbstractItemView::SelectRows);

        ui->tabvListFiles_2->setModel(table_files_model);
        ui->tabvListFiles_2->setColumnWidth(0,(ui->tabvListFiles->width()-20)*0.5);
        ui->tabvListFiles_2->setColumnWidth(1,(ui->tabvListFiles->width()-20)*0.2);
        ui->tabvListFiles_2->setColumnWidth(2,(ui->tabvListFiles->width()-20)*0.3);
        ui->tabvListFiles_2->setSelectionBehavior(QAbstractItemView::SelectRows);

        if(previews.size()>0)
        {
            //есть превью
            num_preview=0;
            ui->tbFirstPreview->setEnabled(true);
            ui->tbPriorPreview->setEnabled(true);
            ui->tbNextPreview->setEnabled(true);
            ui->tbLastPreview->setEnabled(true);
            ui->leNumPreview->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
            ui->leNumPreview->setEnabled(true);

            ui->tbFirstPreview_2->setEnabled(true);
            ui->tbPriorPreview_2->setEnabled(true);
            ui->tbNextPreview_2->setEnabled(true);
            ui->tbLastPreview_2->setEnabled(true);
            ui->leNumPreview_2->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
            ui->leNumPreview_2->setEnabled(true);

            DisplayPreview(previews);
        }
        else
        {
            //нет превью
            ui->tbFirstPreview->setEnabled(false);
            ui->tbPriorPreview->setEnabled(false);
            ui->tbNextPreview->setEnabled(false);
            ui->tbLastPreview->setEnabled(false);
            ui->leNumPreview->setText("");
            ui->leNumPreview->setEnabled(false);

            ui->tbFirstPreview_2->setEnabled(false);
            ui->tbPriorPreview_2->setEnabled(false);
            ui->tbNextPreview_2->setEnabled(false);
            ui->tbLastPreview_2->setEnabled(false);
            ui->leNumPreview_2->setText("");
            ui->leNumPreview_2->setEnabled(false);

            DisplayPreview(previews);
        }
    }
    else
    {
        //серверные данные
        ui->pteDesRead->setReadOnly(true);
        ui->pteDesRead_2->setReadOnly(true);

        main->GetServerModelInfo(_serv_num, title_model, description, files_model);

        ui->leModelTitle->setText(QString("Модель: ")+title_model);
        ui->pteDesRead->setPlainText(description);

        ui->leModelTitle_2->setText(QString("Модель: ")+title_model);
        ui->pteDesRead_2->setPlainText(description);


        delete table_files_model;
        table_files_model= new QStandardItemModel(files_model.size(),4,this);
        for(int i=0; i<files_model.size();i++)
        {
            QModelIndex i_col_name=table_files_model->index(i, 0, QModelIndex());
            QModelIndex i_col_size=table_files_model->index(i, 1, QModelIndex());
            QModelIndex i_col_last_mod=table_files_model->index(i, 2, QModelIndex());
            QModelIndex i_col_who=table_files_model->index(i, 3, QModelIndex());

            table_files_model->setData(i_col_name, files_model[i].file_name);

            table_files_model->setData(i_col_size, QString::number(files_model[i].size)+" b");

            table_files_model->setData(i_col_last_mod, files_model[i].last_mod);

            table_files_model->setData(i_col_who, main->LoginFromNum(files_model[i].NumLoginMod));

            table_files_model->item(i,0)->setEditable(false);
            table_files_model->item(i,1)->setEditable(false);
            table_files_model->item(i,2)->setEditable(false);
            table_files_model->item(i,3)->setEditable(false);
        }
        QStringList header;
        header << QString("Имя файла") << QString("Размер") << QString("Изменен") << QString("Кем изменен");
        table_files_model->setHorizontalHeaderLabels(header);

        ui->tabvListFiles->setModel(table_files_model);
        ui->tabvListFiles->setColumnWidth(0,(ui->tabvListFiles->width()-20)*0.5);
        ui->tabvListFiles->setColumnWidth(1,(ui->tabvListFiles->width()-20)*0.15);
        ui->tabvListFiles->setColumnWidth(2,(ui->tabvListFiles->width()-20)*0.2);
        ui->tabvListFiles->setColumnWidth(3,(ui->tabvListFiles->width()-20)*0.15);
        ui->tabvListFiles->setSelectionBehavior(QAbstractItemView::SelectRows);

        ui->tabvListFiles_2->setModel(table_files_model);
        ui->tabvListFiles_2->setColumnWidth(0,(ui->tabvListFiles->width()-20)*0.5);
        ui->tabvListFiles_2->setColumnWidth(1,(ui->tabvListFiles->width()-20)*0.15);
        ui->tabvListFiles_2->setColumnWidth(2,(ui->tabvListFiles->width()-20)*0.2);
        ui->tabvListFiles_2->setColumnWidth(3,(ui->tabvListFiles->width()-20)*0.15);
        ui->tabvListFiles_2->setSelectionBehavior(QAbstractItemView::SelectRows);

        GetLoadedPreview(_serv_num, previews);

        if(previews.size()==0)
        {
            ui->tbFirstPreview->setEnabled(false);
            ui->tbPriorPreview->setEnabled(false);
            ui->tbNextPreview->setEnabled(false);
            ui->tbLastPreview->setEnabled(false);
            ui->leNumPreview->setText("");
            ui->leNumPreview->setEnabled(false);

            ui->tbFirstPreview_2->setEnabled(false);
            ui->tbPriorPreview_2->setEnabled(false);
            ui->tbNextPreview_2->setEnabled(false);
            ui->tbLastPreview_2->setEnabled(false);
            ui->leNumPreview_2->setText("");
            ui->leNumPreview_2->setEnabled(false);
        }
        else
        {
            num_preview=0;
            ui->tbFirstPreview->setEnabled(true);
            ui->tbPriorPreview->setEnabled(true);
            ui->tbNextPreview->setEnabled(true);
            ui->tbLastPreview->setEnabled(true);
            ui->leNumPreview->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
            ui->leNumPreview->setEnabled(true);

            ui->tbFirstPreview_2->setEnabled(true);
            ui->tbPriorPreview_2->setEnabled(true);
            ui->tbNextPreview_2->setEnabled(true);
            ui->tbLastPreview_2->setEnabled(true);
            ui->leNumPreview_2->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
            ui->leNumPreview_2->setEnabled(true);
        }
        DisplayPreview(previews);
    }
}
//----------------------------------------------------------
void MainForm::ConstructTreeModel(const QStandardItemModel *_tree_model, const bool _read)
{
    for(int i=0; i<tree_data.size(); i++)
    {
        int res=tree_data[i].result;
        qlonglong loc_num_mod=tree_data[i].model_local;
        qlonglong serv_num_mod=tree_data[i].model_server;
        QString mod_struct=tree_data[i].mod_struct;
        QStringList list_model=mod_struct.split("/");

        QStandardItem *item=_tree_model->invisibleRootItem();


        for(int j=0; j<list_model.size(); j++)
        {
            QString txt=item->text();
            //ветви уже есть
            //проверить всех потомков текущей ветви
            //если есть совпадение то перейти к этому потомку
            //если нет - создать нового потомка
            bool find=false;
            int row_count=item->rowCount();
            for(int k=0; k<row_count; k++)
            {
                QString ch_text=item->child(k)->text();
                QString mod_text=list_model[j];

                if(ch_text==mod_text)
                {
                    item=item->child(k);
                    QString t=item->text();
                    find=true;
                    break;
                }
            }
            if(!find)
            {
                QStandardItem *new_item=new QStandardItem(list_model[j]);
                new_item->setSelectable(false);
                QFont font;
                if(j==list_model.size()-1)
                {

                    font.setBold(true);

                    if(res>0)
                    {
                        new_item->setData(res,Qt::UserRole+1);
                        new_item->setData(loc_num_mod,Qt::UserRole+2);
                        new_item->setData(serv_num_mod,Qt::UserRole+3);
                        if(res>0)
                        {
                            switch(res)
                            {
                            case 1:
                            {
                                new_item->setCheckState(Qt::Unchecked);
                                new_item->setCheckable(true);
                                new_item->setTristate(true);
                                new_item->setSelectable(true);
                                new_item->setEditable(false);
                                QIcon icon(":/Icons/Tree/Triangle_right.ico");
                                new_item->setIcon(icon);
                                break;
                            }
                            case 2:
                            {
                                new_item->setCheckState(Qt::Unchecked);
                                new_item->setCheckable(true);
                                new_item->setTristate(true);
                                new_item->setSelectable(true);
                                new_item->setEditable(false);
                                QIcon icon(":/Icons/Tree/Triangle_left.ico");
                                new_item->setIcon(icon);
                                break;
                            }
                            case 3:
                            {
                                new_item->setCheckState(Qt::Unchecked);
                                new_item->setCheckable(true);
                                new_item->setTristate(true);
                                new_item->setSelectable(true);
                                new_item->setEditable(false);
                                if(_read)
                                {
                                    QIcon icon(":/Icons/Tree/New_sm.ico");
                                    new_item->setIcon(icon);
                                }
                                else
                                {
                                    QIcon icon(":/Icons/Tree/Del_sm.ico");
                                    new_item->setIcon(icon);
                                }

                                break;
                            }
                            case 4:
                            {
                                new_item->setCheckState(Qt::Unchecked);
                                new_item->setCheckable(true);
                                new_item->setTristate(true);
                                new_item->setSelectable(true);
                                new_item->setEditable(false);
                                if(_read)
                                {
                                    QIcon icon(":/Icons/Tree/Del_sm.ico");
                                    new_item->setIcon(icon);
                                }
                                else
                                {
                                    QIcon icon(":/Icons/Tree/New_sm.ico");
                                    new_item->setIcon(icon);
                                }

                                break;
                            }
                            }
                        }

                        QList<tFile> tf=tree_data[i].file;
                        QStandardItem *file_group=new QStandardItem(QString::fromUtf8("Файлы"));
                        file_group->setSelectable(false);
                        file_group->setEditable(false);
                        for(int l=0; l<tf.size();l++)
                        {
                            int is_founded=tree_data[i].file[l].IsFounded;
                            QString name=tree_data[i].file[l].file_name;

                            QString ext=name.right(5);
                            if(ext!=".info" || ext!="files")
                            {
                                QString last_mod1=tree_data[i].file[l].last_mod;
                                int char_t=last_mod1.indexOf("T");
                                QString last_mod=last_mod1.left(char_t)+" "+last_mod1.right(last_mod1.length()-char_t-1);
                                qlonglong size=tree_data[i].file[l].size;

                                QStandardItem *file=new QStandardItem(name);
                                file->setSelectable(true);
                                file->setEditable(false);
                                QFont file_font;
                                file_font.setItalic(true);
                                file_font.setBold(true);

                                int size_font=file_font.pointSize();

                                file->setFont(file_font);

                                QFont file_prop_font;
                                file_prop_font.setPointSize(size_font*0.95);
                                QList<QStandardItem *> columns;
                                QStandardItem *col1=new QStandardItem("Размер: "+QString::number(size)+" bytes");
                                file_prop_font.setItalic(true);
                                col1->setFont(file_prop_font);
                                col1->setSelectable(false);
                                col1->setEditable(false);

                                QStandardItem *col2=new QStandardItem(QString::fromUtf8("Изменено: ")+last_mod);
                                col2->setFont(file_prop_font);
                                col2->setSelectable(false);
                                col2->setEditable(false);

                                col1->setBackground(QBrush(QColor(255,255,0, 64)));
                                col2->setBackground(QBrush(QColor(255,255,0, 64)));

                                file_group->setData(-3, Qt::UserRole+1);
                                file->setData(-3, Qt::UserRole+1);
                                col1->setData(-3, Qt::UserRole+1);
                                col2->setData(-3, Qt::UserRole+1);

                                columns.push_back(col1);
                                columns.push_back(col2);

                                file->appendColumn(columns);

                                if(_read)
                                {
                                    //режим чтения
                                    //флаг IsFounded (1 - есть только на локальном, 2 - есть только на серверном, 0 - есть и там и там)
                                    //при чтении если файл только на локальном то при чтении он будет удален и значит иконка удаления
                                    //если только на сервере то иконка добавления
                                    //при записи наоборот

                                    switch(is_founded)
                                    {
                                    case 1:
                                    {

                                        QIcon icon(":/Icons/Tree/Del_sm.ico");
                                        file->setIcon(icon);
                                        break;
                                    }
                                    case 2:
                                    {
                                        QIcon icon(":/Icons/Tree/New_sm.ico");
                                        file->setIcon(icon);
                                        break;
                                    }
                                    }

                                }
                                else
                                {
                                    //режим записи
                                    switch(is_founded)
                                    {
                                    case 1:
                                    {


                                        QIcon icon(":/Icons/Tree/New_sm.ico");
                                        file->setIcon(icon);
                                        break;
                                    }
                                    case 2:
                                    {
                                        QIcon icon(":/Icons/Tree/Del_sm.ico");
                                        file->setIcon(icon);
                                        break;
                                    }
                                    }
                                }

                                file_group->appendRow(file);

                            }
                        }
                        new_item->appendRow(file_group);
                    }
                    else
                    {
                        new_item->setData(-2,Qt::UserRole+1);
                        new_item->setData(loc_num_mod,Qt::UserRole+2);
                        new_item->setData(serv_num_mod,Qt::UserRole+3);
                        new_item->setBackground(QBrush(QColor(255,0,0, 192)));
                    }
                }
                else
                {
                    new_item->setCheckState(Qt::Unchecked);
                    new_item->setCheckable(true);
                    new_item->setTristate(true);
                    new_item->setEditable(false);
                    new_item->setData(-1, Qt::UserRole+1);
                    font.setBold(false);
                }
                new_item->setFont(font);

                new_item->setTristate(true);

                new_item->setEditable(false);

                item->appendRow(new_item);

                item=new_item;
            }
            else
            {
                QFont font;
                if(j==list_model.size()-1)
                {
                    font.setBold(true);

                    if(res>0)
                    {
                        item->setData(res,Qt::UserRole+1);
                        item->setData(loc_num_mod,Qt::UserRole+2);
                        item->setData(serv_num_mod,Qt::UserRole+3);
                        if(res>0)
                        {
                            switch(res)
                            {
                            case 1:
                            {
                                item->setCheckState(Qt::Unchecked);
                                item->setCheckable(true);
                                item->setTristate(true);
                                item->setSelectable(true);
                                item->setEditable(false);
                                QIcon icon(":/Icons/Tree/Triangle_right.ico");
                                item->setIcon(icon);
                                break;
                            }
                            case 2:
                            {
                                item->setCheckState(Qt::Unchecked);
                                item->setCheckable(true);
                                item->setTristate(true);
                                item->setSelectable(true);
                                item->setEditable(false);
                                QIcon icon(":/Icons/Tree/Triangle_left.ico");
                                item->setIcon(icon);
                                break;
                            }
                            case 3:
                            {
                                item->setCheckState(Qt::Unchecked);
                                item->setCheckable(true);
                                item->setTristate(true);
                                item->setSelectable(true);
                                item->setEditable(false);
                                if(_read)
                                {
                                    QIcon icon(":/Icons/Tree/New_sm.ico");
                                    item->setIcon(icon);
                                }
                                else
                                {
                                    QIcon icon(":/Icons/Tree/Del_sm.ico");
                                    item->setIcon(icon);
                                }

                                break;
                            }
                            case 4:
                            {
                                item->setCheckState(Qt::Unchecked);
                                item->setCheckable(true);
                                item->setTristate(true);
                                item->setSelectable(true);
                                item->setEditable(false);
                                if(_read)
                                {
                                    QIcon icon(":/Icons/Tree/Del_sm.ico");
                                    item->setIcon(icon);
                                }
                                else
                                {
                                    QIcon icon(":/Icons/Tree/New_sm.ico");
                                    item->setIcon(icon);
                                }

                                break;
                            }
                            }
                        }

                        QList<tFile> tf=tree_data[i].file;
                        QStandardItem *file_group=new QStandardItem(QString::fromUtf8("Файлы"));
                        file_group->setSelectable(false);
                        file_group->setEditable(false);
                        for(int l=0; l<tf.size();l++)
                        {
                            int is_founded=tree_data[i].file[l].IsFounded;
                            QString name=tree_data[i].file[l].file_name;

                            QString ext=name.right(5);
                            if(ext!=".info" || ext!="files")
                            {
                                QString last_mod1=tree_data[i].file[l].last_mod;
                                int char_t=last_mod1.indexOf("T");
                                QString last_mod=last_mod1.left(char_t)+" "+last_mod1.right(last_mod1.length()-char_t-1);
                                qlonglong size=tree_data[i].file[l].size;

                                QStandardItem *file=new QStandardItem(name);
                                QFont file_font;
                                file_font.setItalic(true);
                                file_font.setBold(true);

                                int size_font=file_font.pointSize();

                                file->setFont(file_font);

                                QFont file_prop_font;
                                file_prop_font.setPointSize(size_font*0.95);
                                QList<QStandardItem *> columns;
                                QStandardItem *col1=new QStandardItem("Размер: "+QString::number(size)+" bytes");
                                file_prop_font.setItalic(true);
                                col1->setFont(file_prop_font);
                                col1->setSelectable(false);
                                col1->setEditable(false);

                                QStandardItem *col2=new QStandardItem(QString::fromUtf8("Изменено: ")+last_mod);
                                col2->setFont(file_prop_font);
                                col2->setSelectable(false);
                                col2->setEditable(false);

                                col1->setBackground(QBrush(QColor(255,255,0, 64)));
                                col2->setBackground(QBrush(QColor(255,255,0, 64)));

                                file_group->setData(-3, Qt::UserRole+1);
                                file->setData(-3, Qt::UserRole+1);
                                col1->setData(-3, Qt::UserRole+1);
                                col2->setData(-3, Qt::UserRole+1);

                                columns.push_back(col1);
                                columns.push_back(col2);

                                file->appendColumn(columns);
                                if(_read)
                                {
                                    //режим чтения
                                    //флаг IsFounded (1 - есть только на локальном, 2 - есть только на серверном, 0 - есть и там и там)
                                    //при чтении если файл только на локальном то при чтении он будет удален и значит иконка удаления
                                    //если только на сервере то иконка добавления
                                    //при записи наоборот

                                    switch(is_founded)
                                    {
                                    case 1:
                                    {

                                        QIcon icon(":/Icons/Tree/Del_sm.ico");
                                        file->setIcon(icon);
                                        break;
                                    }
                                    case 2:
                                    {
                                        QIcon icon(":/Icons/Tree/New_sm.ico");
                                        file->setIcon(icon);
                                        break;
                                    }
                                    }

                                }
                                else
                                {
                                    //режим записи
                                    switch(is_founded)
                                    {
                                    case 1:
                                    {


                                        QIcon icon(":/Icons/Tree/New_sm.ico");
                                        file->setIcon(icon);
                                        break;
                                    }
                                    case 2:
                                    {
                                        QIcon icon(":/Icons/Tree/Del_sm.ico");
                                        file->setIcon(icon);
                                        break;
                                    }
                                    }
                                }

                                file_group->appendRow(file);
                            }
                        }
                        item->appendRow(file_group);
                    }
                    else
                    {
                        item->setData(-2,Qt::UserRole+1);
                        item->setData(loc_num_mod,Qt::UserRole+2);
                        item->setData(serv_num_mod,Qt::UserRole+3);
                        item->setBackground(QBrush(QColor(255,0,0, 192)));
                    }
                }
            }
        }
    }

}
//----------------------------------------------------------
void MainForm::TreeCustomCollapsed(const QStandardItem *_item, const tTreeMod _tree_mod)
{
    int row_count=_item->rowCount();
    QString  txt=_item->text();

    for(int k=0; k<row_count; k++)
    {
        int prop=_item->child(k)->data(Qt::UserRole+1).toLongLong();

        if((prop<0 || txt=="") && prop!=-3)
        {
            QModelIndex index=_item->child(k)->index();
            switch (_tree_mod)
            {
            case Read:
            {
                ui->tvRead->expand(index);
                break;
            }
            case Write:
            {
                ui->tvWrite->expand(index);
                break;
            }
            }
        }

        int ch_row_count=_item->child(k)->rowCount();
        bool is_submod=false;
        for(int j=0; j<ch_row_count; j++)
        {
            int prop_ch=_item->child(k)->child(j)->data(Qt::UserRole+1).toLongLong();
            if(prop_ch>0)
            {
                is_submod=true;
                break;
            }
        }
        if(is_submod)
        {
            if(prop!=-3)
            {
                switch(_tree_mod)
                {
                case Read:
                {
                    ui->tvRead->expand(_item->child(k)->index());
                    break;
                }
                case Write:
                {
                    ui->tvWrite->expand(_item->child(k)->index());
                    break;
                }
                }
            }

        }

        TreeCustomCollapsed(_item->child(k), _tree_mod);
    }
}
//----------------------------------------------------------
void MainForm::SearchIndex(const qlonglong _current_local_num, const qlonglong _current_server_num, QModelIndex &_read_index, QModelIndex &_write_index)
{
    QStandardItemModel *read_model=(QStandardItemModel*)ui->tvRead->model();

    _read_index=SearchIndexToModel(_current_local_num, _current_server_num, read_model);

    QStandardItemModel *write_model=(QStandardItemModel*)ui->tvWrite->model();

    _write_index=SearchIndexToModel(_current_local_num, _current_server_num, write_model);
}
//----------------------------------------------------------
void MainForm::DisplayPreview(const QStringList &_previews)
{
    if(_previews.size()==0)
    {
        Image = QImage();
    }
    else
    {
        Image = QImage(_previews[num_preview]);
    }

    int view_width=ui->graphicsView->width()-10;
    int view_height=ui->graphicsView->height()-10;

    QGraphicsScene *scene = new QGraphicsScene(this);

    scene->addPixmap(QPixmap::fromImage(Image));

    ui->graphicsView->setScene(scene);
    ui->graphicsView_2->setScene(scene);

    qreal dx=((qreal)view_width)/((qreal)Image.width());
    qreal dy=((qreal)view_height)/((qreal)Image.height());

    qreal res=0;

    if(dx>dy)
    {
        res=dy;
    }
    else
    {
        res=dx;
    }
    ui->graphicsView->resetMatrix();
    ui->graphicsView->scale(res,res);

    ui->graphicsView_2->resetMatrix();
    ui->graphicsView_2->scale(res,res);
}
//----------------------------------------------------------
void MainForm::GetLoadedPreview(const qlonglong _serv_num, QStringList &_previews)
{
    _previews.clear();


    QString path=main->GetTemp()+"Previews/"+QString::number(_serv_num)+"/"+main->GetServerModelPath(_serv_num)+"/";
    QDir dir(path);
    QStringList all_files=dir.entryList(QDir::Files | QDir::Hidden);
    for(int i=0; i<all_files.size(); i++)
    {
        QFileInfo fi(all_files[i]);
        QString suffix=fi.suffix();
        if(suffix.toLower()=="bmp" | suffix.toLower()=="jpg" | suffix.toLower()=="jpeg" | suffix.toLower()=="gif" | suffix.toLower()=="png")
        {
            _previews.push_back(path+all_files[i]);
        }
    }
}
//----------------------------------------------------------
QModelIndex MainForm::SearchIndexToModel(const qlonglong _current_local_num, const qlonglong _current_server_num, const QStandardItemModel *_model)
{

    QStandardItem *res_item=NULL;
    QStandardItem *item=_model->invisibleRootItem();
    bool b=false;
    res_item=SearchItemToModel(_current_local_num, _current_server_num, item, b);

    if(!b)
    {
        return QModelIndex();
    }
    else
    {
        return res_item->index();
    }
}
//----------------------------------------------------------
QStandardItem* MainForm::SearchItemToModel(const qlonglong _current_local_num, const qlonglong _current_server_num, QStandardItem *_item, bool &_stop)
{
QStandardItem *res=NULL;


if(_item==NULL)
{
    return res;
}
    qlonglong loc_num=_item->data(Qt::UserRole+2).toLongLong();
    qlonglong serv_num=_item->data(Qt::UserRole+3).toLongLong();
    QString item_text=_item->data(Qt::EditRole).toString();
    if(loc_num==_current_local_num && serv_num==_current_server_num)
    {
        _stop=true;
        res=_item;
        return res;
    }


int count_children=_item->rowCount();
for(int i=0; i<count_children; i++)
{
    if(_stop)
    {
        return res;
    }

    res=SearchItemToModel(_current_local_num, _current_server_num, _item->child(i), _stop);
}
return res;

}
//----------------------------------------------------------
void MainForm::on_tvRead_clicked(const QModelIndex &_index)
{
    UpToParentFiles(read_tree_model, _index, read_tree_model->itemFromIndex(_index)->checkState());
    DownToChildrensFiles(read_tree_model, _index, read_tree_model->itemFromIndex(_index)->checkState(), Read);

    SaveDescriptionModel(ui->pteDesRead->toPlainText());

    QStandardItem *item=read_tree_model->itemFromIndex(_index);
    if(item!=NULL)
    {
        qlonglong d=item->data(Qt::UserRole+1).toLongLong();

        if(d>0 || d==-2)
        {
            _current_local_num=item->data(Qt::UserRole+2).toLongLong();
            _current_server_num=item->data(Qt::UserRole+3).toLongLong();
        }
        else
        {
            _current_local_num=-1;
            _current_server_num=-1;
        }

        ui->pbLoadPreviews->setEnabled( ui->rbSourceServ->isChecked() && (d>0 || d==-2));
        ui->pbLoadPreviews_2->setEnabled( ui->rbSourceServ_2->isChecked() && (d>0 || d==-2));

        switch (d)
        {
        case 3:
        {
            //есть только на сервере
            ui->rbSourseLoc->setEnabled(false);
            ui->rbSourseLoc_2->setEnabled(false);

            ui->rbSourceServ->setEnabled(true);
            ui->rbSourceServ_2->setEnabled(true);

            ui->rbSourceServ->setChecked(true);
            ui->rbSourceServ_2->setChecked(true);

            ui->pbLoadPreviews->setEnabled(true);
            ui->pbLoadPreviews_2->setEnabled(true);

            break;
        }

        case 4:
        {
            //есть только на клиенте
            ui->rbSourseLoc->setEnabled(true);
            ui->rbSourseLoc_2->setEnabled(true);

            ui->rbSourceServ->setEnabled(false);
            ui->rbSourceServ_2->setEnabled(false);

            ui->rbSourseLoc->setChecked(true);
            ui->rbSourseLoc_2->setChecked(true);

            ui->pbLoadPreviews->setEnabled(false);
            ui->pbLoadPreviews_2->setEnabled(false);

            break;
        }
        default:
        {
            ui->rbSourseLoc->setEnabled(true);
            ui->rbSourseLoc_2->setEnabled(true);

            ui->rbSourceServ->setEnabled(true);
            ui->rbSourceServ_2->setEnabled(true);

            break;
        }
        }
        DisplayInfo(_current_local_num, _current_server_num);
    }
    else
    {
        DisplayInfo(0,0);
    }
}
//----------------------------------------------------------
void MainForm::on_tvWrite_clicked(const QModelIndex &_index)
{
    UpToParentFiles(write_tree_model, _index, write_tree_model->itemFromIndex(_index)->checkState());
    DownToChildrensFiles(write_tree_model, _index, write_tree_model->itemFromIndex(_index)->checkState(), Write);

    SaveDescriptionModel(ui->pteDesRead_2->toPlainText());

    QStandardItem *item=write_tree_model->itemFromIndex(_index);
    if(item!=NULL)
    {
        qlonglong d=item->data(Qt::UserRole+1).toLongLong();

        if(d>0 || d==-2)
        {
            _current_local_num=item->data(Qt::UserRole+2).toLongLong();
            _current_server_num=item->data(Qt::UserRole+3).toLongLong();
        }
        else
        {
            _current_local_num=-1;
            _current_server_num=-1;
        }

        ui->pbLoadPreviews->setEnabled( ui->rbSourceServ->isChecked() && (d>0 || d==-2));
        ui->pbLoadPreviews_2->setEnabled( ui->rbSourceServ_2->isChecked() && (d>0 || d==-2));

        DisplayInfo(_current_local_num, _current_server_num);
    }
    else
    {
        DisplayInfo(0,0);
    }
}
//----------------------------------------------------------
void MainForm::UpToParentFiles(const QStandardItemModel *_model, const QModelIndex &_index, const Qt::CheckState _state)
{
    const QModelIndex parent_index=_model->parent(_index);
    QStandardItem *parent_item=_model->itemFromIndex(parent_index);
    qlonglong p_d=-10;
    if(parent_item!=NULL)
    {
        p_d=parent_item->data(Qt::UserRole+1).toLongLong();
    }
    Qt::CheckState state=_state;

    QStandardItem *item=_model->itemFromIndex(_index);
    qlonglong d=item->data(Qt::UserRole+1).toLongLong();
    if(state==Qt::Unchecked &&  p_d==3)
    {
        return;
    }

    if(d==-2 || d==-3)
    {
        return;
    }
    if(!parent_item)
    {
        return;
    }
    else
    {
        if(parent_item->data(Qt::UserRole+1).toLongLong()==-1)
        {
            QString parent_txt=parent_item->text();
            int child_count=parent_item->rowCount();

            if(child_count>1)
            {
                for(int i=0; i<child_count;i++)
                {
                    QStandardItem *child_item=parent_item->child(i);
                    if (child_item->isCheckable())
                    {
                        QString child_txt=child_item->text();

                        switch (child_item->checkState())
                        {
                        case Qt::Checked:
                        {
                            if(state!=Qt::Checked)
                            {
                                state=Qt::PartiallyChecked;
                            }
                            break;
                        }
                        case Qt::Unchecked:
                        {
                            if(state!=Qt::Unchecked)
                            {
                                state=Qt::PartiallyChecked;
                            }
                            break;
                        }
                        case Qt::PartiallyChecked:
                        {
                            state=Qt::PartiallyChecked;
                            break;
                        }
                        }
                    }
                    if(state==Qt::PartiallyChecked)
                    {
                        break;
                    }
                }
            }
            else
            {
                qlonglong s_num=parent_item->data(Qt::UserRole+1).toLongLong();
                if(s_num>0)
                {
                    state=Qt::Checked;

                }
            }
            parent_item->setCheckState(state);
        }
        UpToParentFiles(_model, parent_index, state);

    }
}
//----------------------------------------------------------
void MainForm::DownToChildrensFiles(const QStandardItemModel *_model, const QModelIndex _index, const Qt::CheckState _state, const tTreeMod _direction)
{
    QStandardItem *item=_model->itemFromIndex(_index);
    QString txt=item->text();
    qlonglong s_num=item->data(Qt::UserRole+1).toLongLong();
    if(s_num==-3 || s_num==-2)
    {
        return;
    }
    if(s_num>0 && s_num!=0)
    {
        qlonglong loc_num_mod=item->data(Qt::UserRole+2).toLongLong();
        qlonglong serv_num_mod=item->data(Qt::UserRole+3).toLongLong();

        //обновить таблицу ModelRead
        bool state=false;
        switch (_direction)
        {
        case Read:
        {
            if(_state==Qt::Checked)
            {
                state=true;
                //тут ставим true в структуре для модели
                SetToModelsTreeData(loc_num_mod, serv_num_mod, true, true);
            }
            else
            {
                //тут ставим false в структуре для модели
                SetToModelsTreeData(loc_num_mod, serv_num_mod, true, false);
            }
            break;
        }
        case Write:
        {
            if(_state==Qt::Checked)
            {
                state=true;
                //тут ставим true в структуре для модели
                SetToModelsTreeData(loc_num_mod, serv_num_mod, false, true);
            }
            else
            {
                //тут ставим false в структуре для модели
                SetToModelsTreeData(loc_num_mod, serv_num_mod, false, false);
            }
            break;
        }
        }
    }

    int child_count=item->rowCount();
    if(child_count==0)
    {
        return;
    }
    else
    {
        for(int i=0; i<child_count; i++)
        {
            QStandardItem *children=item->child(i);
            qlonglong d=children->data(Qt::UserRole+1).toLongLong();
            QString ch_txt=children->text();
            if(d!=-3)
            {

                if(d!=-2)
                {
                    children->setCheckState(_state);

                }
                QModelIndex child_index=children->index();
                DownToChildrensFiles(_model, child_index, _state, _direction);
            }
        }
    }
}
//----------------------------------------------------------
void MainForm::SaveDescriptionModel(const QString &_text)
{
    QString file_name="";
    QString hash="";
    QDateTime last_mod;
    qint64 size=0;

    //Записать изменения в базу, определить имя файла информации о модели
    main->SaveDescription(current_local_model, _text, file_name);

    //Сверить, в самом ли деле отличается то что в файле с тем что в окне
    //Если отличается - записать, пересчитать хэш и узнать время модификации
    EditingModelFile(file_name, _text, hash, last_mod, size);

    //если файл менялся то хэш не будет пустым

    if(hash!="")
    {
        //и нужно переписать хэш самого файла в таблице локальных файлов
        //а также дату модификации
        //после этого пересчитать общий хэш модели, и исправить общий хэш в локальной таблице моделей
        main->UpdateInfoData(current_local_model, file_name, hash, last_mod);

        EndUpdateServerModel(false);
    }
}
//----------------------------------------------------------
void  MainForm::DisplayInfo(const qlonglong _loc_num, const qlonglong _serv_num)
{


    if(_loc_num>0 || _serv_num>0)
    {

        DisplayModelInfo(_loc_num, _serv_num);
    }
    else
    {
        DisplayModelInfo(0, 0);
    }

}
//----------------------------------------------------------
void MainForm::SetToModelsTreeData(const qlonglong _loc_num_model, const qlonglong _serv_num_model, const bool _to_read, const bool _choice)
{
    for(int i=0; i<tree_data.size(); i++)
    {
        if(tree_data[i].model_local==_loc_num_model && tree_data[i].model_server==_serv_num_model)
        {
            //модель найдена
            if(_to_read)
            {
                tree_data[i].read_choice=_choice;
            }
            else
            {
                tree_data[i].write_choice=_choice;
            }
        }
    }
}
//----------------------------------------------------------
void MainForm::EditingModelFile(const QString &_file_name, const QString& _text, QString& _hash, QDateTime &_dt, qint64 &_size)
{

    if(_file_name!="")
    {
        QSettings s(main->GetRoot()+_file_name, QSettings::IniFormat);
        QTextCodec *codec =QTextCodec::codecForName("UTF-8");
        s.setIniCodec(codec);

        QString cur_text=s.value("Description", "").toString();
        if(cur_text!=_text)
        {
            s.setValue("Description", _text);

            //Найти новые дату модификации и хэш файла
            QFileInfo fi(main->GetRoot()+_file_name);
            _dt=fi.lastModified();
            tCalcHash ch;
            ch.GetFileHash(main->GetRoot()+_file_name);
            _hash=ch.ResultHash();
            _size=fi.size();
        }
    }
}
//----------------------------------------------------------
void MainForm::EndUpdateServerModel(const bool _rebuild)
{
    //формирование дерева чтения по полученым и имеющимся данным после обновления данных с сервера

    main->OnListFilesLocal();

    BuildingTree(user_login);

    ConstructTree(Read, list_compare);
    ConstructTree(Write, list_compare);
    if(_rebuild)
    {

        RecoveryTreeIndex();
    }
}
//----------------------------------------------------------
void MainForm::BuildingTree(const QString& _user_login)
{
    //В таблицу CompareTablesToTree из локальной, ласт и серверной таблиц занести ориентируясь на Struct суммарные хеши моделей
    main->WriteToCompareTablesToTree(_user_login);

    //Провести анализ на направление изменений моделей в целом
    list_compare=main->AnalyzeCompareAll();

    //добавляем к стректуре моделей различающиеся файлы
    main->AddFilesToModelsStruct(list_compare);



}
//----------------------------------------------------------


void MainForm::on_pbRead_clicked()
{
    SaveDescriptionModel(ui->pteDesRead->toPlainText());

    DisplayModelInfo(0, 0);
    max_models=0;

    //начать чтение с сервера по новому



    //если запрашиваемый файл есть только локально а на сервере его нет то это удаление файла локально
    //остальные случаи это или создание или модификация что тут - одно и то же
    StartReadModeles(main->GetRoot(), 0, false);
}
//----------------------------------------------------------
void MainForm::StartReadModeles(const QString &_root, const qlonglong _server_num_model, const bool _is_preview)
{
    max_models=-1;
    QString destination=_root;
    main->SetTransactionFlag(true);
    main->ClearConveyor();
    int is_work=false;
    int custom_copy=0;
    if(_server_num_model==0)
    {

        for(int i=0; i<tree_data.size();i++)
        {
            if(tree_data[i].read_choice)
            {
                for(int j=0; j<tree_data[i].file.size(); j++)
                {
                    QString file_name=tree_data[i].file[j].file_name;
                    if(tree_data[i].file[j].IsFounded==1)
                    {
                        main->DeletingLocalFile(file_name);
                    }
                    else
                    {
                        main->ReceiveFile(file_name);
                    }
                    is_work=true;
                }
            }
        }
    }
    else
    {
        QStringList files_list;
        if(_is_preview)
        {
            main->GetServerListPreviews(_server_num_model, files_list);
            custom_copy=2;
            destination=_root;
        }
        else
        {
            main->GetServerListFiles(_server_num_model, files_list);
            custom_copy=1;
            destination=_root;
        }
        max_models=1;
        for(int i=0; i<files_list.size(); i++)
        {
            main->ReceiveFile(files_list[i]);
        }

        is_work=true;

    }
    if(is_work)
    {
        if(max_models<0)
        {
            max_models=main->GetCountRecDelModels();
        }
        int x=this->geometry().x();
        int y=this->geometry().y();
        int w=this->geometry().width();
        int h=this->geometry().height();

        emit ProgressStart(max_models, x, y, w, h);

        main->StartReceiveDeleteFiles(destination, custom_copy, max_models);
    }
    else
    {
        QMessageBox MB;
        MB.setText(QString::fromUtf8("Нет выделеных моделей для чтения"));
        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
        MB.exec();
    }
}


//----------------------------------------------------------

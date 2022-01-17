#include "cclouddisk.h"
#include "ui_cclouddisk.h"

CCloudDisk::CCloudDisk(int id, CSqlite* sql, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CCloudDisk),m_id(id),m_sql(sql)
{
    ui->setupUi(this);



    m_delmark = 0;
    m_downmark = 0;
    m_sharemark = 0;
    m_createfile = 0;
    m_creategroup = 0;
    m_addgroup = 0;
    m_sharedownmark = 0;
    m_filepath = QString::number(m_id);

    m_tool = new CTool;
    connect(m_tool,SIGNAL(SIG_OK(QString)) , this, SLOT(create_file(QString)));
    m_progressUp = ui->imbox_progressUp;
    m_progressDowm = ui->imbox_progressDown;
    m_progressFinish = ui->imbox_progressFinish;
    //              ui
    //文件路径
    ui->lb_filedirectory->setText(m_filepath);
    //文件的显示列
    ui->gridLayout->setColumnStretch(0,1);
    ui->gridLayout->setColumnStretch(1,1);
    ui->gridLayout->setColumnStretch(2,1);
    ui->gridLayout->setColumnStretch(3,1);
    ui->gridLayout->setColumnStretch(4,1);
    ui->gridLayout->setColumnStretch(5,1);

    //文件的显示行
    ui->gridLayout->setRowStretch(0,1);
    ui->gridLayout->setRowStretch(1,1);
    ui->gridLayout->setRowStretch(2,1);
    ui->gridLayout->setRowStretch(3,1);

    //文件的显示列
    ui->gridLayout_share->setColumnStretch(0,1);
    ui->gridLayout_share->setColumnStretch(1,1);
    ui->gridLayout_share->setColumnStretch(2,1);
    ui->gridLayout_share->setColumnStretch(3,1);
    ui->gridLayout_share->setColumnStretch(4,1);
    ui->gridLayout_share->setColumnStretch(5,1);

    //文件的显示行
    ui->gridLayout_share->setRowStretch(0,1);
    ui->gridLayout_share->setRowStretch(1,1);
    ui->gridLayout_share->setRowStretch(2,1);
    ui->gridLayout_share->setRowStretch(3,1);
    setToolbarcontent();

    //群设置
    m_widgetList_create =  new IMToolList("创建的共享群");
    m_widgetList_join = new IMToolList("加入的共享群");
    ui->wdg_box->addItem(m_widgetList_create);
    ui->wdg_box->addItem(m_widgetList_join);
}

CCloudDisk::~CCloudDisk()
{

    delete ui;
}

void CCloudDisk::on_pb_file_clicked()
{
    ui->stack_page->setCurrentIndex(0);
}

void CCloudDisk::on_pb_trans_clicked()
{
    ui->stack_page->setCurrentIndex(1);
}
#include<QLineEdit>
void CCloudDisk::on_pb_newfile_clicked()
{
    m_createfile = 1;
    m_tool->show();
}

void CCloudDisk::create_file(QString filename )
{

    if(m_createfile)
    {
        m_createfile = 0;
        slot_Deal_File(filename);
        Q_EMIT SIG_CREATEFILE(filename);
    }
    else if(m_creategroup)
    {
        m_creategroup = 0;
        Q_EMIT SIG_CREATEGROUP(filename);
    }
    else if(m_addgroup)
    {
        m_addgroup = 0;
        Q_EMIT SIG_JOINGROUP(filename);

    }

}

void CCloudDisk::on_pb_deletefile_clicked()
{
    if(m_delmark)
    {
        m_delmark = 0;
        ui->wdg_filemanger->setStyleSheet(QString(""));
        return;
    }

    m_delmark = 1;
    ui->wdg_filemanger->setStyleSheet(QString("border: 1px solid #9f9f9f; border-radius: 5px;"));

}

QString CCloudDisk::getfilepath()
{
    return m_filepath;
}
//删除文件
void CCloudDisk::delfile(QString filename)
{

    CFileItem *item = m_mapFilenameToFileitem[filename];
    ui->gridLayout->removeWidget(item);
    item->setParent(0);
    m_mapFilenameToFileitem.remove(filename);
    delete item;
    item = NULL;
    return;
}

void CCloudDisk::addProgressBar(CProgressBar *bar)
{
    switch(bar->m_type)
    {
        case progress_up:
            ui->imbox_progressUp->addItem(bar);
            break;
        case progress_down:
            ui->imbox_progressDown->addItem(bar);
            break;
            //case progress_finish:
        //case progress_up_finish || progress_down_finish:
        default:
            ui->imbox_progressFinish->addItem(bar);
            break;
    }
}

void CCloudDisk::moveProgressBar(CProgressBar *bar)
{
    switch(bar->m_type)
    {
        case progress_up:
            ui->imbox_progressUp->removeItem(bar);
            break;
        case progress_down:
            ui->imbox_progressDown->removeItem(bar);
            break;
        case progress_up_finish:
            ui->imbox_progressUp->removeItem(bar);
            break;
        case progress_down_finish:
            ui->imbox_progressDown->removeItem(bar);
            break;
            //case progress_finish:

    }
}

void CCloudDisk::moveBarToFinishUi(CProgressBar *bar)
{
    ui->imbox_progressFinish->addItem(bar);
}
//单击文件事件
void CCloudDisk::slot_deal_click_event(QString filename,QString filepath)
{

    if(m_delmark)
    {

        Q_EMIT SIG_DELETE_FILE(filename);
        ui->wdg_filemanger->setStyleSheet(QString(""));
        m_delmark = 0;
    }
    else if(m_downmark)
    {
        Q_EMIT SIG_DOWN_FILE(filename,"");
        ui->wdg_filemanger->setStyleSheet(QString(""));
        m_downmark = 0;
    }
    else if(m_sharemark)
    {
        Q_EMIT SIG_SHARE_FILE(filename);
        ui->wdg_filemanger->setStyleSheet(QString(""));
        m_sharemark = 0;
    }
    else if(m_sharedownmark)
    {
        Q_EMIT SIG_DOWN_SHARE_FILE(filename,filepath);
        ui->wdg_filemanger_share->setStyleSheet(QString(""));
        m_sharedownmark = 0;
    }

    //共享文件

}
//双击文件事件
void CCloudDisk::slot_deal_double_click_event(QString filename)
{
    CFileItem *item = (CFileItem *)QObject::sender();
    if(item->m_filename.contains(".mp4"))
    {

        Q_EMIT SIG_PLAYSOURCE(item->m_filepath+':'+item->m_filename);
        return;
    }
    if(item->m_type == TYPE_OWN)
    {
        m_leftStack.push(m_filepath);
        m_filepath += ":";
        m_filepath += filename;
        ui->lb_filedirectory->setText(m_filepath);
        on_pb_flushUi_clicked(1,m_filepath);
    }
    else
    {
        m_leftshareStack.push(item->m_filepath);
        on_pb_flushUi_clicked(0,item->m_filepath+':'+item->m_filename);
    }
}

//刷新界面
void CCloudDisk::on_pb_flushUi_clicked(int type,QString filepath)
{

    QLayoutItem *item;
    if(type)
    {
        while((item = ui->gridLayout->takeAt(0)) != 0)
        {
            if(item->widget()){
                item->widget()->setParent(0);
                ui->gridLayout->removeWidget(item->widget());

                delete item->widget();
            }
            delete item;
        }
        get_file_list(m_filepath,1);


    }
    else
    {
        while((item = ui->gridLayout_share->takeAt(0)) != 0)
        {
            if(item->widget()){
                item->widget()->setParent(0);
                ui->gridLayout->removeWidget(item->widget());

                delete item->widget();
            }
            delete item;
        }
        //if(filepath!="")
            get_file_list(filepath,0);
    }

}

//获取文件列表
void CCloudDisk::get_file_list(QString filepath,int type)
{
    Q_EMIT SIG_GET_FILE_LIST(filepath,type);
}

void CCloudDisk::setToolbarcontent()
{
    ui->lb_uptoolbarconcent->setText(QString("正在上传 %1 个文件").arg(ui->imbox_progressUp->getcount()));
    ui->lb_downtoolbarconcent->setText(QString("正在下载 %1 个文件").arg(ui->imbox_progressDown->getcount()));
    ui->lb_finishtoolbarconcent->setText(QString("已完成传输 %1 个文件").arg(ui->imbox_progressFinish->getcount()));
}


//#include<vector>
void CCloudDisk::slot_Deal_File(QString filename,QString filepath)
{
    CFileItem *item = new CFileItem;
    if(filename.contains(".",Qt::CaseSensitive))
    {
        item->Info(filename,TYPE_FILE,TYPE_OWN,filepath);
    }
    else
    {
        item->Info(filename,TYPE_FOLDER,TYPE_OWN,filepath);
    }

    connect(item,SIGNAL(SIG_CLICK_EVENT( QString,QString)),this,SLOT(slot_deal_click_event(QString,QString)));
    connect(item,SIGNAL(SIG_DOUBLE_CLICK_EVENT(QString)),this,SLOT(slot_deal_double_click_event(QString)));

    m_mapFilenameToFileitem[filename] = item;

    ui->gridLayout->addWidget(item);
}

void CCloudDisk::on_pb_left_clicked()
{
    if(m_leftStack.empty())
        return;
    m_rightStack.push(m_filepath);
    QString filepath = m_leftStack.top();
    m_leftStack.pop();
    m_filepath = filepath;
    ui->lb_filedirectory->setText(m_filepath);
    on_pb_flushUi_clicked(1);
}

void CCloudDisk::on_pb_right_clicked()
{
    if(m_rightStack.empty())
         return;
    m_leftStack.push(m_filepath);
    QString filepath = m_rightStack.top();
    m_rightStack.pop();
    m_filepath = filepath;
    ui->lb_filedirectory->setText(m_filepath);
    on_pb_flushUi_clicked(1);
}
#include<QFileDialog>
void CCloudDisk::on_pb_upfile_clicked()
{
    QString s = QFileDialog::getOpenFileName(
                        this,"打开" );
    qDebug()<<s;
    if(s == "")
         return;
   Q_EMIT SIG_UP_FILE(s);
}

void CCloudDisk::on_pb_downfile_clicked()
{
    if(m_downmark)
    {
        m_downmark = 0;
        ui->wdg_filemanger->setStyleSheet(QString(""));
        return;
    }

    m_downmark = 1;
    ui->wdg_filemanger->setStyleSheet(QString("border: 1px solid #9f9f9f; border-radius: 5px;"));
}


void CCloudDisk::on_pb_up_file_clicked()
{
     ui->stack_smallpage->setCurrentIndex(0);
}

void CCloudDisk::on_pb_down_file_clicked()
{
    ui->stack_smallpage->setCurrentIndex(1);
}

void CCloudDisk::on_pb_finish_file_clicked()
{
    ui->stack_smallpage->setCurrentIndex(2);
}
#include<QMessageBox>
void CCloudDisk::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question(this,"提示","是否退出程序？") == QMessageBox::Yes)
    {
        Q_EMIT SIG_CLOSE_EVENT();
        event->accept();
    }
    else
    {
        event->ignore();
    }

}


void CCloudDisk::on_pb_recently_clicked()
{
    ui->stack_page->setCurrentIndex(2);
}

void CCloudDisk::on_pb_share_clicked()
{
    ui->stack_page->setCurrentIndex(3);
}
//创建共享群
void CCloudDisk::on_pb_creategroup_clicked()
{
    m_creategroup = 1;
    m_tool->show();
}

void CCloudDisk::creategroup(QString groupname)
{
    QCheckBox *box = new QCheckBox;
    box->setText(groupname);
    m_createBoxlst.push_back(box);
    m_widgetList_create->addItem(box);
}

void CCloudDisk::joinsharegroup(QString groupname)
{
    QCheckBox *box = new QCheckBox;
    box->setText(groupname);
    m_joinBoxlst.push_back(box);
    m_widgetList_join->addItem(box);
}

void CCloudDisk::get_group_info()
{
    Q_EMIT SIG_GET_GROUP_INFO();
}


void CCloudDisk::on_pb_sharefile_clicked()
{
    if(m_sharemark)
    {
        m_downmark = 0;
        ui->wdg_filemanger->setStyleSheet(QString(""));
        return;
    }
    ui->stack_page->setCurrentIndex(0);
    m_sharemark = 1;
    ui->wdg_filemanger->setStyleSheet(QString("border: 1px solid #9f9f9f; border-radius: 5px;"));
}


void CCloudDisk::on_pb_addsharefile_clicked()
{
    on_pb_sharefile_clicked();
}

void CCloudDisk::on_pb_look_clicked()
{
    on_pb_flushUi_clicked(0);
    Q_EMIT SIG_LOOK_SHARE_FILE();
}

void CCloudDisk::slot_Share_Deal_File(QString filename, QString filepath)
{
    CFileItem *item = new CFileItem;
    if(filename.contains(".",Qt::CaseSensitive))
    {
        item->Info(filename,TYPE_FILE,TYPE_SHARE,filepath);
    }
    else
    {
        item->Info(filename,TYPE_FOLDER , TYPE_SHARE ,filepath);
    }

    connect(item,SIGNAL(SIG_CLICK_EVENT( QString,QString)),this,SLOT(slot_deal_click_event(QString,QString)));
    connect(item,SIGNAL(SIG_DOUBLE_CLICK_EVENT(QString)),this,SLOT(slot_deal_double_click_event(QString)));

    m_mapFilenameToFileitem[filename] = item;

    ui->gridLayout_share->addWidget(item);
}

void CCloudDisk::on_pb_flushUi_clicked()
{
    on_pb_flushUi_clicked(1);
}

void CCloudDisk::on_pb_downfile_share_clicked()
{
    if(m_sharedownmark)
    {
        m_sharedownmark = 0;
        ui->wdg_filemanger_share->setStyleSheet(QString(""));
        return;
    }

    m_sharedownmark = 1;
    ui->wdg_filemanger_share->setStyleSheet(QString("border: 1px solid #9f9f9f; border-radius: 5px;"));
}

void CCloudDisk::on_pb_joingroup_clicked()
{
    m_addgroup = 1;
    m_tool->show();
}

void CCloudDisk::on_pb_left_share_clicked()
{
    if(m_leftshareStack.empty())
        return;
    QString filepath = m_leftshareStack.top();
    m_rightshareStack.push(filepath);

    m_leftshareStack.pop();
    on_pb_flushUi_clicked(0,filepath);
}

//void CCloudDisk::on_pb_right_share_clicked()
//{
//    if(m_rightshareStack.empty())
//         return;\
//    QString filepath = m_rightshareStack.top();
//    m_leftStack.push(filepath);
//    m_rightshareStack.pop();
//    on_pb_flushUi_clicked(1,filepath);
//}



void CCloudDisk::on_pb_flushUi_share_clicked()
{
    on_pb_flushUi_clicked(0);
}

void CCloudDisk::on_pb_right_share_clicked()
{
    if(m_rightshareStack.empty())
        return;
    QString filepath = m_rightshareStack.top();
    m_leftshareStack.push(filepath);

    m_leftshareStack.pop();
    on_pb_flushUi_clicked(0,filepath);
}

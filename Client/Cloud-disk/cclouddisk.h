#ifndef CCLOUDDISK_H
#define CCLOUDDISK_H

#include <QWidget>
#include<iostream>
#include<stack>
#include "ctool.h"
#include "cfileitem.h"
#include"cprogressbar.h"
#include "QMap"
#include<QCloseEvent>
#include<csqlite.h>
#include<IMToolBox.h>
#include"cgroupitem.h"
#include<QCheckBox>
#include"videoshow.h"
namespace Ui {
class CCloudDisk;
}

class CCloudDisk : public QWidget
{
    Q_OBJECT

public:
    explicit CCloudDisk(int id,CSqlite* sql,QWidget *parent = 0);
    ~CCloudDisk();

signals:
    void SIG_CREATEFILE(QString );
    void SIG_DELETE_FILE(QString  );
    void SIG_DOWN_FILE(QString ,QString);
    void SIG_DOWN_SHARE_FILE(QString,QString );
    void SIG_CREATEGROUP(QString);
    void SIG_JOINGROUP(QString);
    void SIG_SHARE_FILE(QString );
    void SIG_GET_FILE_LIST(QString,int );
    void SIG_UP_FILE(QString );
    void SIG_CLOSE_EVENT();
    void SIG_LOOK_SHARE_FILE();
    void SIG_GET_GROUP_INFO();
    void SIG_PLAYSOURCE(QString);

public slots:
    void slot_deal_click_event(QString, QString filepath);            //单击事件
    void slot_deal_double_click_event(QString filename);    //双击事件
    void get_file_list(QString filepath, int type);                     //获取文件列表
    void setToolbarcontent();               //设置进度条提示
    //void keepdata();
private slots:
    void on_pb_file_clicked();

    void on_pb_trans_clicked();                 //切换界面

    void on_pb_newfile_clicked();               //创建文件按钮

    void create_file(QString filename);       //创建文件

    void on_pb_deletefile_clicked();          //点击删除文件按钮

    void slot_Deal_File(QString filename, QString filepath="");


    void on_pb_left_clicked();          //切换上层文件目录

    void on_pb_right_clicked();         //切换下层文件目录

    void on_pb_upfile_clicked();            //上传文件点击

    void on_pb_downfile_clicked();          //下载文件点击

    void on_pb_flushUi_clicked(int type, QString filepath="");             //刷新显示文件界面

    void on_pb_up_file_clicked();           //上传文件界面

    void on_pb_down_file_clicked();         //下载文件界面

    void on_pb_finish_file_clicked();       //完成任务界面

    void closeEvent(QCloseEvent *event);

    void on_pb_recently_clicked();
    //文件分享
    void on_pb_share_clicked();         //共享群页面

    void on_pb_sharefile_clicked();     //共享文件

    void on_pb_creategroup_clicked();       //创建群


    void on_pb_addsharefile_clicked();      //添加共享文件

    void on_pb_look_clicked();
    void slot_Share_Deal_File(QString filename, QString filepath);



    void on_pb_flushUi_clicked();

    void on_pb_downfile_share_clicked();

    void on_pb_joingroup_clicked();

    void on_pb_left_share_clicked();

    //void on_pb_right_share_clicked();

    void on_pb_flushUi_share_clicked();

    void on_pb_right_share_clicked();


public:
    QString getfilepath();
    void delfile(QString);
    void addProgressBar(CProgressBar* bar);
    void moveProgressBar(CProgressBar* bar);
    void moveBarToFinishUi(CProgressBar* bar);
    void creategroup(QString groupname);
    void joinsharegroup(QString groupname);
    void get_group_info();
private:
    Ui::CCloudDisk *ui;
    CTool *m_tool;

    int m_delmark;
    int m_downmark;
    int m_sharedownmark;
    int m_sharemark;
    int m_createfile;
    int m_creategroup;
    int m_addgroup;
    int m_id;
    QString m_filepath;
    std::stack<QString> m_leftStack;
    std::stack<QString> m_rightStack;
    std::stack<QString> m_leftshareStack;
    std::stack<QString> m_rightshareStack;
public:
    QMap<QString,CFileItem*> m_mapFilenameToFileitem;
    QList<CProgressBar*> m_lstBar;
    CSqlite *m_sql;
    IMToolBox * m_progressUp;
    IMToolBox * m_progressDowm;
    IMToolBox * m_progressFinish;
    IMToolList * m_widgetList_create;
    IMToolList * m_widgetList_join;
    QList<QCheckBox *> m_createBoxlst;
    QList<QCheckBox *> m_joinBoxlst;
};

#endif // CCLOUDDISK_H

#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>

#include"./netapi/qmytcpclient.h"
#include"Thread_pool.h"
#include"clogin.h"
#include"cclouddisk.h"
#include<QFileInfo>
#include<QThread>
#include<qDebug>
#include<QMessageBox>
#include<QFileDialog>
#include<csqlite.h>
#include<QTime>
#include"tcpfilenet.h"
class CKernel;
class CMySendFile;
typedef void (CKernel::*PFUN)(char* buf,int nlen);

typedef struct FileBlockInfo
{

    FileBlockInfo()
    {

        m_total_size = 0;
        m_pos = 0;
        m_number = 0;
        m_finish_size = 0;
    }
    int m_total_size;
    int m_pos;
    int m_number;
    int m_finish_size;

} FileBlockInfo;
typedef struct FileInfo
{
    FileInfo()
    {
        //fpr = NULL;
        bar = NULL;
        m_file = NULL;//= new FILE;
        m_finish_len = 0;
        m_total_len = 0;
        //m_pos = 0;
        m_last_secondDataSize = 0;
    }
    FILE *m_file;
    int m_finish_len;
    int m_total_len;
    //QString m_path;
    CProgressBar *bar;
    //int m_pos;
    int m_last_secondDataSize;
    //uchar* fpr; //内存映射起始地址
    //QFile file;
    QList<FileBlockInfo*> m_lstBlockInfo;
} FileInfo;


class CKernel : public QObject
{
    Q_OBJECT

signals:
    void SIG_GET_FILE_LIST_RS(QString,QString);
    void SIG_GET_SHARE_LIST_RS(QString,QString);
    void SIG_SEND_FILE(int,int);
    void SIG_RECV_FILE(char*);
    void SIG_PRORESS_SPEED(double,CProgressBar*);

public:
    explicit CKernel(QObject *parent = 0);


public slots:
    void slot_ReadyData(char *buf, int nlen);
    //注册
    void slot_RegisterRs(char *buf, int neln);
    //登录
    void slot_LoginRq(QString ,QString);
    void slot_LoginRs(char *buf, int neln);
    void slots_Register_Login_commit(QString name, QString password);
    //强制离线
    void slot_ForceOfline(char *buf, int neln);
    //创建、删除文件
    void slot_Create_FileRq(QString);
    void slot_Delete_FileRq(QString filename);
    //获取目录
    void slot_Get_File_ListRq(QString filepath, int type);
     void slot_Get_File_ListRs(char *buf, int neln);
    //上传文件
    void slot_Send_FileRq(QString filepath);
    void slot_SendFileRs(char *buf, int neln);
    void dealSendFileRs(int number);
    void dealMiaochuan(int number);
    static void* slot_datablock_send( /*int pos,int number*/void*);
    //static void* slot_bigdatablock_send( /*int pos,int number*/void*);
    //下载文件
    void slot_Down_FileRq(QString filename, QString filepath="");
    void slot_Down_FileRs(char *buf, int neln);
    void slot_datablock_recv(char *buf, int neln);
    static void *recvfile(void *buf);
    //共享文件
    void slot_Create_GroupRq(QString groupname);

    //分享文件
    void slot_Share_File_Rq(QString filename);
    void slot_create_groupRs(char *buf, int neln);
    void slot_look_sharefileRq();
    void slot_look_sharefileRs(char *buf, int neln);
    void slot_Join_Share_GroupRq(QString filename);
    void slot_Join_Share_GroupRs(char *buf, int neln);
    void slot_Get_Group_InfoRq();

    //播放音视频
    void slot_Play_SourceRq(QString path);
    void slot_Play_SourceRs(char *buf, int neln);
    //功能模块
    void setProgressBar(double, CProgressBar *);            //设置进度
    void slot_closeUpDown();                                //关闭上传下载
    void slot_state_Change(int);                            //暂停播放
    void slot_ToFinishUi();                                 //完成上传下载
    static void* slot_show_UpDownLoad_Speed(void*);         //测速功能
    void setDownLoadInfo(QString path, QString filename, int number, int finishsize, int totalsize);    //加载下载信息
    void setUpLoadInfo(QString path, QString filename, int number, int finishsize, int totalsize);      //加载上传信息
    CProgressBar* create_ProgressBar(QString &filename, int beginpos, int filesize, int number, int type);      //创建进度条ui
    void keepdata();        //退出保存进度条
private:
    void setNetPackMap();               //设置映射关系
    void Info();
    void close();
    void loadSqlAndSet();           //加载进度条

    //void dealDownFileRs(QString filename,int number);


public:
    static CKernel* m_pThis;

private:
    //网络
    QMyTcpClient *m_tcp;
    QMyFileTcpClient *m_fileTcp;
    PFUN m_NetPackFunMap[DEF_PACK_COUNT];
    thread_pool *m_Threadpool;

    //界面类
    CLogin *m_login;
    CCloudDisk *m_CloudDlg;

    int m_id;

    //文件
    CMySendFile *m_FileSender;
    //QThread *m_WorkThread;
    int m_UpDownFileNumber;
    int m_NetworkSpeed_Switch;

    VideoShow *m_videoShow;
public:
    //QMap<int,FILE*> m_mapNumberToPfile;
    QMap<int , FileInfo*> m_mapNumToFileInfo;
    pool_t * m_pool;
    CSqlite *m_sql;


};

#endif // CKERNEL_H

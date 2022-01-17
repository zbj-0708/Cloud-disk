#include "ckernel.h"
#include<qDebug>
#include<QMessageBox>
#include<QCryptographicHash>


#define MD5_KEY 1234
static QByteArray GetMD5(QString password)
{

    QCryptographicHash hash(QCryptographicHash::Md5);
    QString key = QString("%1_%2").arg(password).arg(MD5_KEY);      //加盐
    hash.addData(key.toLocal8Bit());
    QByteArray res = hash.result();
    return res.toHex();   //转成16进制
}

static QByteArray GetMD5(QByteArray& content)
{

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(content);
    QByteArray res = hash.result();
    return res.toHex();   //转成16进制
}

CKernel *CKernel::m_pThis = NULL;
CKernel::CKernel(QObject *parent) : QObject(parent)
{
    ///////////////////////////////////
    m_pThis = this;
    //网络
    setNetPackMap();
    m_tcp = new QMyTcpClient;
    m_fileTcp = new QMyFileTcpClient;
    connect(m_tcp,SIGNAL(SIG_ReadyData(char*,int)),this,SLOT(slot_ReadyData(char*,int)));

    ///////////////////////////////////
    //登录
    m_login = new CLogin;
    connect(m_login , SIGNAL(SIG_Register_Login_commit(QString,QString)) , this , SLOT(slots_Register_Login_commit(QString ,QString))  );
    connect(m_login , SIGNAL(SIG_Login_commit(QString,QString)) , this , SLOT(slot_LoginRq(QString ,QString))  );
    m_login->show();
    //m_login->is_autoLogin();


}

void CKernel::Info()
{
    qDebug()<<QThread::currentThreadId();
    m_NetworkSpeed_Switch = 0;
    m_login->keep_Info();
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    m_UpDownFileNumber = qrand()%10000;
    //线程池创建
    m_Threadpool = new thread_pool;
    m_pool = m_Threadpool->Pool_create(20,10,15);
    //不是很好，最好想办法改一下
    connect(this, SIGNAL(SIG_PRORESS_SPEED(double,CProgressBar*)) , this , SLOT( setProgressBar(double,CProgressBar*) ),Qt::QueuedConnection);
    //登录成功后写入ini文件
    //数据库
    m_sql = new CSqlite;
    QString DBPath = QDir::currentPath() + "/sql/";
    qDebug()<<DBPath;
    QString FileName = "cloud_disk.db";
    m_sql->ConnectSql(DBPath + FileName );

    //播放器
    m_videoShow = new VideoShow;
    //主界面
    m_CloudDlg = new CCloudDisk(m_id,m_sql);
    connect(m_CloudDlg, SIGNAL(SIG_CREATEFILE(QString)) , this  , SLOT(slot_Create_FileRq(QString)));
    connect(m_CloudDlg, SIGNAL(SIG_DELETE_FILE(QString)) , this  , SLOT(slot_Delete_FileRq(QString))); 
    connect(m_CloudDlg, SIGNAL(SIG_GET_FILE_LIST(QString,int)) , this  , SLOT(slot_Get_File_ListRq(QString,int)));
    connect(this, SIGNAL(SIG_GET_FILE_LIST_RS(QString,QString)) , m_CloudDlg  , SLOT(slot_Deal_File(QString,QString)));
    connect(m_CloudDlg, SIGNAL(   SIG_SHARE_FILE(QString)) , this  , SLOT(slot_Share_File_Rq(QString)));
    connect(m_CloudDlg, SIGNAL(   SIG_UP_FILE(QString)  ) , this , SLOT(   slot_Send_FileRq(QString)   )  );
    connect(m_CloudDlg, SIGNAL(   SIG_DOWN_FILE(QString,QString)) , this  , SLOT(  slot_Down_FileRq(QString,QString)));
    connect(m_CloudDlg, SIGNAL(   SIG_DOWN_SHARE_FILE(QString,QString)) , this  , SLOT(  slot_Down_FileRq(QString,QString)));
    connect(m_CloudDlg, SIGNAL(   SIG_CREATEGROUP(QString)) , this  , SLOT(  slot_Create_GroupRq(QString)));
    connect(m_CloudDlg, SIGNAL(   SIG_CLOSE_EVENT()  ) , this , SLOT(   keepdata()  ) , Qt::DirectConnection );
    connect(m_CloudDlg, SIGNAL(   SIG_LOOK_SHARE_FILE()  ) , this , SLOT(  slot_look_sharefileRq()  ));
    connect(this, SIGNAL(   SIG_GET_SHARE_LIST_RS(QString,QString)  ) , m_CloudDlg , SLOT(  slot_Share_Deal_File(QString,QString)  ));
    connect(m_CloudDlg, SIGNAL(   SIG_JOINGROUP(QString) ), this  , SLOT(  slot_Join_Share_GroupRq(QString)));
    connect(m_CloudDlg, SIGNAL(   SIG_GET_GROUP_INFO() ), this  , SLOT(  slot_Get_Group_InfoRq()));
    connect(m_CloudDlg, SIGNAL(   SIG_PLAYSOURCE(QString) ), this  , SLOT(  slot_Play_SourceRq(QString)));

    m_CloudDlg->get_file_list(QString::number(m_id),1);
    m_CloudDlg->get_group_info();
    m_CloudDlg->show();

    loadSqlAndSet();



}

void CKernel::close()
{
    delete m_CloudDlg;
    m_CloudDlg = NULL;
}
#include<QDir>
#include<QFile>
void CKernel::loadSqlAndSet()
{
    QString DBPath = QDir::currentPath() + "/sql/";
    qDebug()<<DBPath;
    QString FileName = "cloud_disk.db";
   // m_sql->ConnectSql(DBPath + FileName );
    //先看路径存不存在
    QDir tempDir;
    if(!tempDir.exists(DBPath))
    {
        qDebug()<<"不存在sql路径";
        tempDir.mkdir(DBPath);  //创建此路径
    }
    QFile file(DBPath + FileName);

    if(file.exists())
    {
        //存在，加载数据
        //========================加载下载列表==============================
        QString sqlStr;
        sqlStr = "select fileName , filefinishSize , fileSize , filePath  , fileNumber from t_doingDownload";
        QStringList resList;
        bool res = m_sql->SelectSql(sqlStr , 5 , resList);
        if(!res)return;
        sqlStr = "delete from  t_doingDownload";
        res = m_sql->UpdateSql(sqlStr);
        if(!res)return;
        for(int i=0;i<resList.size();i+=5)
        {
            setDownLoadInfo(resList[i+3],resList[i],((QString)resList[i+4]).toInt(),((QString)resList[i+1]).toInt(),((QString)resList[i+2]).toInt());
            //发送下载文件请求
            STRU_DOWN_FILERQ rq;
            rq.m_number = ((QString)resList[i+4]).toInt();
            rq.m_pos = ((QString)resList[i+1]).toInt();
            rq.m_UserID = m_id;
            rq.m_result = file_downrq;
            memcpy(rq.szFileName,((QString)resList[i]).toStdString().c_str(),((QString)resList[i]).toStdString().size());
            memcpy(rq.szFilePath,m_CloudDlg->getfilepath().toStdString().c_str(),m_CloudDlg->getfilepath().toStdString().size());
            m_tcp->SendData((char*)&rq,sizeof(rq));

            //暂停
            m_mapNumToFileInfo[rq.m_number]->bar->on_pb_static_clicked();

        }
        resList.clear();
        //========================加载上传列表==============================
        sqlStr = "select fileName , filefinishSize , fileSize , filePath  , fileNumber , threadCount , beginpos , totallen ,cloudpath ,cloudname from t_doingUpload";
        res = m_sql->SelectSql(sqlStr , 10 , resList);
        if(!res)return;
        sqlStr = "delete from  t_doingUpload";
        res = m_sql->UpdateSql(sqlStr);
        if(!res)return;
        for(int i=0;i<resList.size();i+=10)
        {
            int number = ((QString)resList[i+4]).toInt();
            setUpLoadInfo(resList[i+3],resList[i],((QString)resList[i+4]).toInt(),((QString)resList[i+1]).toInt(),((QString)resList[i+2]).toInt());
            FileInfo * info = m_pThis->m_mapNumToFileInfo[number];
            info->bar->m_cloudname = (QString)resList[i+9];
            info->bar->m_filepath = (QString)resList[i+8];
            FileBlockInfo *blockinfo = NULL;

            //信息初始化
            int threadcount = ((QString)resList[i+5]).toInt();
            QString str_beginpos = (QString)resList[i+6];
            QString str_totallen = (QString)resList[i+7];
            int beginpos;
            int blocklen;
            int totallen = 0;
            int n;
            while(threadcount--)
            {
                blockinfo = new FileBlockInfo;
                blockinfo->m_number = number;
                //blockinfo->m_finish_size = 0;
                //开始位置
                n = str_beginpos.indexOf('#');
                beginpos = str_beginpos.mid(0,n).toInt();
                str_beginpos = str_beginpos.mid(n+1,-1);
                blockinfo->m_pos = beginpos;
                //完成位置
                n = str_totallen.indexOf('#');
                blocklen = str_totallen.mid(0,n).toInt();
                str_totallen = str_totallen.mid(n+1,-1);
                blockinfo->m_total_size = blocklen;
                totallen += blocklen;
                //加入列表
                info->m_lstBlockInfo.push_back(blockinfo);
                //加入任务队列
                m_Threadpool->Producer_add(m_pool,slot_datablock_send,(void*)blockinfo);
                //开启信号量
                //sem_post(&info->bar->m_sem);
                ++info->bar->m_semCount;
            }
            STRU_SEND_FILERQ rq;
            rq.m_number = number;
            rq.m_result = progress_up;
            rq.m_total_len = totallen;
            rq.m_UserID = m_id;
            memcpy(rq.szFileName,info->bar->m_cloudname.toStdString().c_str(),info->bar->m_cloudname.toStdString().size());
            memcpy(rq.szFilePath,((QString)resList[i+8]).toStdString().c_str(),((QString)resList[i+8]).toStdString().size());
            m_tcp->SendData((char*)&rq,sizeof(rq));
        }
        resList.clear();

        //========================加载完成列表==============================
        sqlStr = "select fileName , fileSize , filePath , fileType  , fileData from t_finishTrans";
        res = m_sql->SelectSql(sqlStr , 5 , resList);
        if(!res)return;
        sqlStr = "delete from t_finishTrans";
        res = m_sql->UpdateSql(sqlStr);
        if(!res)return;
        for(int i=0;i<resList.size();i+=5)
        {
            CProgressBar *bar = create_ProgressBar(resList[i],((QString)resList[i+1]).toInt(),((QString)resList[i+1]).toInt(),-1,((QString)resList[i+3]).toInt());
            bar->setFinshiState(resList[i+4]);
            bar->m_path = resList[i+2];
        }
        resList.clear();

        //更新主界面文字
        m_CloudDlg->setToolbarcontent();

    }
    else
    {
        if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            qDebug()<<"数据库创建失败，存储歌曲会出问题";
            QMessageBox::about(0,"提示","数据库创建失败，存储歌曲会出问题!");
        }
        else
        {
            file.close();

            //建表   正在下载t_doingDownload    正在上传t_doingUpload    已完成传输t_finishTrans
            QString sqlStr;
            sqlStr = "create table t_doingDownload(fileName varchar(260) ,filefinishSize int, fileSize int  , filePath varchar(260)  ,fileNumber int)";
            m_sql->UpdateSql(sqlStr);
            sqlStr = "create table t_doingUpload(fileName varchar(260) , filefinishSize int, fileSize int , filePath varchar(260) , fileNumber int ,threadCount int , beginpos varchar(260) , totallen varchar(260) , cloudpath varchar(260), cloudname varchar(260))";
            m_sql->UpdateSql(sqlStr);
            sqlStr = "create table t_finishTrans(fileName varchar(260) , fileSize int , filePath varchar(260) , fileType int , fileData varchar(260) )";
            m_sql->UpdateSql(sqlStr);
        }
    }
}
void CKernel::keepdata()
{   //   正在下载t_doingDownload    正在上传t_doingUpload    已完成传输t_finishTrans
    //fileName varchar(260) ,filefinishSize int, fileSize int  , filePath varchar(260)  ,fileNumber int
    //"create table t_finishTrans(fileName varchar(260) , fileSize int , filePath varchar(260) , fileType varchar(50) , fileData varchar(260) )"
    qDebug()<<"keepdata";
    QList<QWidget *> lst ;
    QString sqlStr;
    //下载信息保存
    lst = m_CloudDlg->m_progressDowm->m_itemList;
    for(auto ite = lst.begin();ite!=lst.end();ite++)
    {
//        int n = ((CProgressBar*)(*ite))->m_path.lastIndexOf('/');
//        ((CProgressBar*)(*ite))->m_path = ((CProgressBar*)(*ite))->m_path.mid(0,n);
        sqlStr = QString("insert into t_doingDownload(fileName,filefinishSize,fileSize,filePath, fileNumber) values('%1',%2,%3,'%4',%5)").
                arg(((CProgressBar*)(*ite))->m_filename).arg((int)((CProgressBar*)(*ite))->m_current_size).arg((int)((CProgressBar*)(*ite))->m_total_size).arg(((CProgressBar*)(*ite))->m_path).arg(((CProgressBar*)(*ite))->m_number);
        qDebug()<<sqlStr;
        bool is = m_sql->UpdateSql(sqlStr);
        qDebug()<<is;
    }
    //上传信息保存
    lst = m_CloudDlg->m_progressUp->m_itemList;
    for(auto ite = lst.begin();ite!=lst.end();ite++)
    {
        QList<FileBlockInfo*> fileinfolst = m_mapNumToFileInfo[((CProgressBar*)(*ite))->m_number]->m_lstBlockInfo;
        int threadCount = fileinfolst.size();
        QString beginpos;
        QString totallen;
        for(auto ite = fileinfolst.begin();ite!=fileinfolst.end();ite++)
        {
            qDebug()<<(*ite)->m_pos;
            beginpos += QString::number((*ite)->m_pos);
            beginpos += '#';
            totallen += QString::number((*ite)->m_total_size- (*ite)->m_finish_size);
            totallen += '#';
        }
        qDebug()<<beginpos;
        qDebug()<<totallen;
        sqlStr = QString("insert into t_doingUpload(fileName,filefinishSize,fileSize,filePath, fileNumber , threadCount , beginpos ,totallen,cloudpath,cloudname) values('%1',%2,%3,'%4',%5,%6,'%7','%8','%9','%10')").
                arg(((CProgressBar*)(*ite))->m_filename).arg((int)((CProgressBar*)(*ite))->m_current_size).arg((int)((CProgressBar*)(*ite))->m_total_size).
                arg(((CProgressBar*)(*ite))->m_path).arg(((CProgressBar*)(*ite))->m_number).arg(threadCount).arg(beginpos).arg(totallen).arg(((CProgressBar*)(*ite))->m_filepath).arg(((CProgressBar*)(*ite))->m_cloudname);
        qDebug()<<sqlStr;
        bool is = m_sql->UpdateSql(sqlStr);
        qDebug()<<is;
    }
    //完成信息保存
    lst = m_CloudDlg->m_progressFinish->m_itemList;

    for(auto ite = lst.begin();ite!=lst.end();ite++)
    {
//        int n = ((CProgressBar*)(*ite))->m_path.lastIndexOf('/');
//        ((CProgressBar*)(*ite))->m_path = ((CProgressBar*)(*ite))->m_path.mid(0,n);
        sqlStr = QString("insert into t_finishTrans(fileName,fileSize,filePath, fileType , fileData) values('%1',%2,'%3',%4,'%5')").
                arg(((CProgressBar*)(*ite))->m_filename).arg((int)((CProgressBar*)(*ite))->m_total_size).arg(((CProgressBar*)(*ite))->m_path).arg(((CProgressBar*)(*ite))->m_type).arg(((CProgressBar*)(*ite))->m_finishData);
        qDebug()<<sqlStr;
        bool is = m_sql->UpdateSql(sqlStr);
        qDebug()<<is;
    }

}
void CKernel::dealSendFileRs(int number)
{

    //内存映射
    FileInfo * info = m_pThis->m_mapNumToFileInfo[number];
    QFile file(info->bar->m_path+'/'+info->bar->m_filename);
    file.open(QIODevice::ReadOnly);
    int total_len = file.size();




    //分配任务
    FileBlockInfo *blockinfo = NULL;
    int len = 0;
    while(len + BLOCK_SIZE <=total_len)
    {

        //信息初始化
        blockinfo = new FileBlockInfo;
        blockinfo->m_number = number;
        blockinfo->m_pos = len;
        blockinfo->m_total_size = BLOCK_SIZE;
        info->m_lstBlockInfo.push_back(blockinfo);
        //加入任务队列
        m_Threadpool->Producer_add(m_pool,slot_datablock_send,(void*)blockinfo);
        len += BLOCK_SIZE;
        //开启信号量
        sem_post(&info->bar->m_sem);
        ++info->bar->m_semCount;

    }

    blockinfo = new FileBlockInfo;
    blockinfo->m_number = number;
    blockinfo->m_pos = len;
    blockinfo->m_total_size = total_len-len;
    info->m_lstBlockInfo.push_back(blockinfo);
    m_Threadpool->Producer_add(m_pool,slot_datablock_send,(void*)blockinfo);
    //默认信号量初始化是1，所以最后一个线程不用在加信号量
    //    sem_post(&info->bar->m_sem);
    //    ++info->bar->m_semCount;
    //Sleep(10000000);
}

void CKernel::dealMiaochuan(int number)
{
    CProgressBar * bar = m_mapNumToFileInfo[number]->bar;
    int size = m_pThis->m_mapNumToFileInfo[number]->m_total_len;
    Q_EMIT SIG_PRORESS_SPEED(size,bar);
    delete m_mapNumToFileInfo[number];
    m_mapNumToFileInfo.remove(number);
}



#define NetPackFunMap(a) m_NetPackFunMap[ a - DEF_PACK_BASE]


void CKernel::setNetPackMap()
{
    memset( m_NetPackFunMap , 0 , sizeof(PFUN)*DEF_PACK_COUNT );
    NetPackFunMap(DEF_PACK_REGISTER_RS) =  &CKernel::slot_RegisterRs;
    NetPackFunMap(DEF_PACK_LOGIN_RS) =  &CKernel::slot_LoginRs;
    NetPackFunMap(DEF_PACK_FORCE_OFFLINE) =  &CKernel::slot_ForceOfline;
    NetPackFunMap(DEF_PACK_GET_FILE_LISTRS) =  &CKernel::slot_Get_File_ListRs;
    NetPackFunMap(DEF_PACK_SEND_FILERS) =  &CKernel::slot_SendFileRs;
    NetPackFunMap(DEF_PACK_DOWN_FILERS) =  &CKernel::slot_Down_FileRs;
    NetPackFunMap(DEF_PACK_FILE_DATABLOCK) =  &CKernel::slot_datablock_recv;
    NetPackFunMap(DEF_PACK_CREATE_GROUPRS) =  &CKernel::slot_create_groupRs;
    NetPackFunMap(DEF_PACK_LOOK_SHARE_FILERS) =  &CKernel::slot_look_sharefileRs;
    NetPackFunMap(DEF_PACK_JOIN_SHARE_GROUPRS) =  &CKernel::slot_Join_Share_GroupRs;
    NetPackFunMap(DFE_PACK_PLAY_SOURCERS) =  &CKernel::slot_Play_SourceRs;
    //    NetPackFunMap(DEF_PACK_ADD_FRIEND_RQ) =  &CKernel::slot_dealaddFriendRq;
//    NetPackFunMap(DEF_PACK_ADD_FRIEND_RS) =  &CKernel::slot_dealaddFriendRs;
//    NetPackFunMap(DEF_PACK_FRIEND_INFO) =  &CKernel::slot_friendinfo;
//    NetPackFunMap(DEF_PACK_CHAT_RQ) =  &CKernel::slot_dealChatRq;
//    NetPackFunMap(DEF_PACK_CHAT_RS) =  &CKernel::slot_dealChatRs;
//    NetPackFunMap(DEF_PACK_JOINROOM_RS) =  &CKernel::slot_joinRoomRs;
//    NetPackFunMap(DEF_PACK_LEAVEROOM_RS) =  &CKernel::slot_leaveRoomRs;
//    NetPackFunMap(DEF_PACK_ROOM_MEMBER) =  &CKernel::slot_roomMemberRq;
//    NetPackFunMap(DEF_PACK_ADD_ROOMRQ) =  &CKernel::slot_add_RoomRq;
//    NetPackFunMap(DEF_PACK_ADD_ROOMRS) =  &CKernel::slot_add_RoomRs;
//    NetPackFunMap(DEF_PACK_VIDEO_FRAME) =  &CKernel::slot_recvVideoframe;
//    NetPackFunMap(DEF_PACK_AUDIO_FRAME) =  &CKernel::slot_audio_frame;
//    NetPackFunMap(DEF_PACK_CREATEROOM_RS) =  &CKernel::slot_createRoomRs;

}



void CKernel::slot_ReadyData(char*buf,int nlen)
{
    int type = *(int*) buf;
    if( type >= DEF_PACK_BASE && type < DEF_PACK_BASE + DEF_PACK_COUNT ) {
        PFUN p = m_NetPackFunMap[ type - DEF_PACK_BASE ];
        if( p != NULL )
        {
            (this->*p)( buf , nlen );
        }
    }
    //如果是多线程，需要考虑回收问题
    if(type != DEF_PACK_FILE_DATABLOCK )
       delete[] buf;
}


//注册提交
void CKernel::slots_Register_Login_commit(QString name,QString password)
{
    qDebug()<<"slots_Register_Login_commit";
    STRU_REGISTER_RQ rq;
    std::string str = name.toStdString();
    const char *str1 = str.data();
    strcpy_s(rq.m_szUser,MAX_SIZE,str1);

    QByteArray  bt= GetMD5(password);
    memcpy(rq.m_szPassword,bt.data(),bt.size());

    m_tcp->SendData((char*)&rq,sizeof(rq));
}

//注册回复
void CKernel::slot_RegisterRs(char *buf, int neln)
{
    qDebug()<<"slot_RegisterRs";
    STRU_REGISTER_RS *rs = (STRU_REGISTER_RS*)buf;
    switch(rs->m_lResult)
    {
       case userid_is_exist:
              QMessageBox::about(m_login,"提示","用户名已被注册");
              break;
       case register_sucess:
              QMessageBox::about(m_login,"提示","注册成功");
//              //注册成功后写入ini文件
//              LPTSTR  lpPath = new wchar_t[INI_MAX_PATH_SIZE];

//              MultiByteToWideChar(CP_ACP,0,INI_PATH,-1,lpPath,INI_MAX_PATH_SIZE);
//              WritePrivateProfileString(L"1", L"number", QString(rs->m_szUser).toStdWString().c_str(), lpPath);
//              delete lpPath;
              break;
       default:
              break;
    }
}
//登录提交
void CKernel::slot_LoginRq(QString number,QString password)
{
    qDebug()<<"slot_LoginRq";
    STRU_LOGIN_RQ rq;
    std::string str = number.toStdString();
    const char *str1 = str.data();
    strcpy_s(rq.m_szUser,MAX_SIZE,str1);

    QByteArray  bt= GetMD5(password);
    memcpy(rq.m_szPassword,bt.data(),bt.size());
    qDebug()<<bt;

    m_tcp->SendData((char*)&rq,sizeof(rq));
}

//登录回复
void CKernel::slot_LoginRs(char *buf, int neln)
{
    qDebug()<<"slot_LoginRs";
    STRU_LOGIN_RS *rs = (STRU_LOGIN_RS*)buf;
    switch(rs->m_lResult)
    {

        case userid_no_exist:
            QMessageBox::about(m_login,"提示","用户名不存在");
             break;
        case password_error :
            QMessageBox::about(m_login,"提示","密码错误");
            break;
        case login_sucess :
            //登录界面写一个正在登陆
            m_id = rs->m_UserID;
            Info();
            m_login->hide();
             break;
        default :
             break;
    }
}
//强制离线
void CKernel::slot_ForceOfline(char *buf, int neln)
{
    QMessageBox::about(m_login,"提示","异地登录，强制下线");
    close();
    m_login->show();
}
//创建文件夹
void CKernel::slot_Create_FileRq(QString filename)
{
    STRU_CREATE_FILE_RQ rq;
    rq.m_UserID = m_id;
    strcpy(rq.szFileName,filename.toStdString().c_str());
    strcpy(rq.szFilePath,m_CloudDlg->getfilepath().toStdString().c_str());

    m_tcp->SendData((char*)&rq,sizeof(rq));
}
//删除文件
void CKernel::slot_Delete_FileRq(QString filename)
{
    STRU_DELETE_FILE_RQ rq;
    rq.m_UserID = m_id;
    strcpy(rq.szFileName,filename.toStdString().c_str());
    strcpy(rq.szFilePath,m_CloudDlg->getfilepath().toStdString().c_str());

    m_tcp->SendData((char*)&rq,sizeof(rq));
    m_CloudDlg->delfile(filename);

}

void CKernel::slot_Get_File_ListRq(QString filepath,int type)
{
    STRU_GET_FILE_LIST_RQ rq;
    rq.m_UserID = m_id;
    rq.type = type;
    strcpy(rq.szFilePath,filepath.toStdString().c_str());
    m_tcp->SendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_Get_File_ListRs(char *buf, int neln)
{
    STRU_GET_FILE_LIST_RS *rs = (STRU_GET_FILE_LIST_RS*)buf;
    if(rs->type)
        Q_EMIT SIG_GET_FILE_LIST_RS(rs->szFileName,rs->szFilePath);
    else
        Q_EMIT SIG_GET_SHARE_LIST_RS(rs->szFileName,rs->szFilePath);
}
/*
 上传回复结果
#define file_uprq             0
#define file_uprs             1
#define file_cancel_uprq      2
#define file_cancel_uprs      3
#define file_up _failedrs      4
#define file_up _successrs     5
#define file_suspend_uprq     6
#define file_suspend_uprs     7
#define file_continue_uprq    8
#define file_continue_uprs    9
 */
////ui
//#define progress_up 0
//#define progress_down 1
//#define progress_finish 2
void CKernel::slot_Send_FileRq(QString filepath)
{
    qDebug()<<QThread::currentThreadId();
    STRU_SEND_FILERQ rq;
    QFileInfo info(filepath);
    rq.m_UserID = m_id;
    rq.m_result = file_uprq;
    rq.m_number = m_UpDownFileNumber;
    rq.m_total_len = info.size();

        QFile file(filepath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug()<<"Can't open the file!"<<endl;
        }
        QByteArray content = file.readAll();

        QByteArray  str = GetMD5(content);
        //str.toStdString().c_str();
        strcpy(rq.m_md5,str.toStdString().c_str());
        qDebug()<<rq.m_md5;
    //注意：QString与string类型中文字的存储大小不相等

    memcpy(rq.szFileName,info.fileName().toStdString().c_str(),info.fileName().toStdString().size());
    memcpy(rq.szFilePath,m_CloudDlg->getfilepath().toStdString().c_str(),m_CloudDlg->getfilepath().toStdString().size());
    if(m_tcp->SendData((char*)&rq,sizeof(rq)) != -1)
    {
        int n = filepath.lastIndexOf('/');
        QString filename = filepath.mid(n+1,-1);
        filepath = filepath.mid(0,n);
        CProgressBar* bar = create_ProgressBar(filename,0,rq.m_total_len,rq.m_number,progress_up);
        FileInfo *fileInfo = new FileInfo;
        fileInfo->bar = bar;
        fileInfo->bar->m_path = filepath;
        fileInfo->bar->m_filepath = m_CloudDlg->getfilepath();
        fileInfo->m_total_len = info.size();
        m_mapNumToFileInfo[m_UpDownFileNumber] = fileInfo;
        if(!m_NetworkSpeed_Switch)
        {
            m_NetworkSpeed_Switch = 1;
            m_Threadpool->Producer_add(m_pool,slot_show_UpDownLoad_Speed,0);
        }
    }
    m_UpDownFileNumber++;
}


void CKernel::slot_SendFileRs(char *buf, int neln)
{
    STRU_SEND_FILERS *rs = (STRU_SEND_FILERS*)buf;
    switch(rs->m_result)
    {
        case file_uprs:
            //QMessageBox::about(0,"提示",QString("开始上传%1").arg(rs->szFileName));
            m_mapNumToFileInfo[rs->m_number]->bar->m_cloudname = rs->m_cloudname;
            dealSendFileRs(rs->m_number);
           // Q_EMIT SIG_SEND_FILE(rs->m_pos,rs->m_number);
            break;
        case file_cancel_uprs:
            QMessageBox::about(0,"提示",QString("取消上传%1").arg(rs->szFileName));
            break;
        case file_continue_uprs:
            QMessageBox::about(0,"提示",QString("继续上传%1").arg(rs->szFileName));
            break;
        case file_suspend_uprs:
            QMessageBox::about(0,"提示",QString("暂停上传%1").arg(rs->szFileName));
            break;
        case file_miaochuan:
            qDebug()<<"秒传成功";
            m_mapNumToFileInfo[rs->m_number]->bar->m_cloudname = rs->m_cloudname;
            dealMiaochuan(rs->m_number);
            break;
        case file_up_failedrs:
            //QMessageBox::about(0,"提示",QString("%1上传失败").arg(rs->szFileName));
            break;
//        default:
//            QMessageBox::about(0,"提示"," 上传错误");
//            break;
    }

}

CProgressBar* CKernel::create_ProgressBar(QString &filename, int beginpos, int filesize,int number,int type)
{
    qDebug()<<"create_ProgressBar"<<QThread::currentThreadId();
    CProgressBar *bar ;
    bar = new CProgressBar;
    bar->InfoBar(filename,beginpos,filesize,number,type);
    connect(bar , SIGNAL(SIG_CLOSE() ), this , SLOT(slot_closeUpDown()) );
    connect(bar , SIGNAL(SIG_CHANGE_STATE(int) ), this , SLOT(slot_state_Change(int)) );
    connect(bar, SIGNAL(SIG_PRORESS_FINISH()) , this , SLOT( slot_ToFinishUi() ));
    m_CloudDlg->addProgressBar(bar);
    m_CloudDlg->setToolbarcontent();
    return bar;

}
void CKernel::setProgressBar(double pos,CProgressBar* bar)
{
    bar->setProgressBar(pos);
}

void CKernel::slot_closeUpDown()
{
     CProgressBar* bar = qobject_cast<CProgressBar*>(sender());
     if(bar->m_type == progress_up)
     {
         STRU_SEND_FILERQ rq;
         rq.m_UserID = m_id;
         rq.m_number = bar->m_number;
         rq.m_result = file_cancel_uprq;
         m_tcp->SendData((char*)&rq,sizeof(rq));
         m_CloudDlg->moveProgressBar(bar);
     }
     else if(bar->m_type == progress_down)
     {
         STRU_DOWN_FILERQ rq;
         rq.m_UserID = m_id;
         rq.m_number = bar->m_number;
         rq.m_result = file_cancel_downrq;
         m_tcp->SendData((char*)&rq,sizeof(rq));
         m_CloudDlg->moveProgressBar(bar);
     }
     else
     {
           m_CloudDlg->m_progressFinish->removeItem(bar);
     }

     delete bar;
     bar = NULL;
     m_CloudDlg->setToolbarcontent();
}

void CKernel::slot_state_Change(int state)
{
    CProgressBar* bar = qobject_cast<CProgressBar*>(sender());
    STRU_DOWN_FILERQ rq;
    rq.m_number = bar->m_number;
    rq.m_UserID = m_id;
    if(state)
        rq.m_result = file_continue_downrq;
    else
        rq.m_result = file_suspend_downrq;
    m_tcp->SendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_ToFinishUi()
{
    CProgressBar* bar = qobject_cast<CProgressBar*>(sender());
    QDateTime curDateTime=QDateTime::currentDateTime();
    QString finishtime = curDateTime.toString("yyyy-MM-dd hh:mm:ss");
    bar->setFinshiState(finishtime);
    m_CloudDlg->moveProgressBar(bar);
    m_CloudDlg->moveBarToFinishUi(bar);

    m_CloudDlg->setToolbarcontent();
}

void *CKernel::slot_show_UpDownLoad_Speed(void *)
{
    qDebug()<<"速度检测开启";
    int speed = 0;
    int quit = 0;
    while(1)
    {

        quit = 1;
        for(auto ite = CKernel::m_pThis->m_mapNumToFileInfo.begin();ite != CKernel::m_pThis->m_mapNumToFileInfo.end();ite++)
        {
            if( (*ite)!= NULL &&!(*ite)->bar->m_is_finish)
            {
                quit = 0;
                speed = ((*ite)->bar->m_current_size-(*ite)->m_last_secondDataSize)/1024;
                (*ite)->m_last_secondDataSize = (*ite)->bar->m_current_size;
                (*ite)->bar->setNetworkSpeed(speed);
            }
        }
        if(quit)
            break;
        Sleep(1000);
    }
    qDebug()<<"速度检测关闭";
    CKernel::m_pThis->m_NetworkSpeed_Switch = 0;
}

#include<QMessageBox>
void* CKernel::slot_datablock_send(/*int pos, int number*/void* arg)
{
    qDebug() << "SendFile: " <<QThread::currentThreadId();
    //准备工作
    FileBlockInfo *blockinfo = (FileBlockInfo*)arg;
    FileInfo * info = m_pThis->m_mapNumToFileInfo[blockinfo->m_number];
    CProgressBar *bar = info->bar;
    //文件映射
    QFile file(info->bar->m_path+'/'+info->bar->m_filename);
    file.open(QIODevice::ReadOnly);
    uchar* pmmap = file.map(blockinfo->m_pos,blockinfo->m_total_size);
    //发送的数据包
    STRU_BIG_FILE_DATABLOCK rq;
    qDebug()<<sizeof(rq)<<"  "<<rq.block_size;
    rq.m_UserID = m_pThis->m_id;
    rq.m_number = blockinfo->m_number;


    while(blockinfo->m_finish_size + MAX_FILE_CONTENT_LEN < blockinfo->m_total_size && !info->bar->m_close)
    {
        if(!info->bar->m_state)
        {
            sem_wait(&info->bar->m_sem);
            sem_post(&info->bar->m_sem);
        }
        //读到rq.m_block中
        memcpy(rq.m_block,pmmap+blockinfo->m_finish_size,MAX_FILE_CONTENT_LEN);
        //out.readRawData(arr,MAX_FILE_CONTENT_LEN);
        rq.m_len = MAX_FILE_CONTENT_LEN;
        blockinfo->m_finish_size += MAX_FILE_CONTENT_LEN;
        rq.m_pos = blockinfo->m_pos;
        blockinfo->m_pos += MAX_FILE_CONTENT_LEN;
        qDebug()<<"i:"<<rq.m_pos<<QThread::currentThreadId();
        m_pThis->m_fileTcp->SendData((char*)&rq,sizeof(rq));
        qDebug()<<"大块"<<sizeof(rq);
        Q_EMIT m_pThis->SIG_PRORESS_SPEED(MAX_FILE_CONTENT_LEN,bar);
    }
    if(info->bar->m_close)
         qDebug()<<"close send threaad: "<<QThread::currentThreadId();
    else
    {
        rq.m_pos = blockinfo->m_pos;
        qDebug()<<"i:"<<rq.m_pos<<QThread::currentThreadId();;
        //out.readRawData(rq.m_block,blockinfo->m_total_size-blockinfo->m_finish_size);
        memcpy(rq.m_block,pmmap+blockinfo->m_finish_size,blockinfo->m_total_size-blockinfo->m_finish_size);
        rq.m_len = blockinfo->m_total_size-blockinfo->m_finish_size;
        m_pThis->m_fileTcp->SendData((char*)&rq,sizeof(rq));
        qDebug()<<"小块"<<sizeof(rq);
        info->m_finish_len += blockinfo->m_total_size;
    }
    //信号量-1
    sem_wait(&info->bar->m_sem);
    info->bar->m_semCount--;
    Q_EMIT m_pThis->SIG_PRORESS_SPEED(blockinfo->m_total_size-blockinfo->m_finish_size,bar);
    info->m_lstBlockInfo.removeOne(blockinfo);
    if(info->m_finish_len == info->m_total_len)
    {
        //m_CloudDlg->m_progressUp->removeItem(bar);
        delete CKernel::m_pThis->m_mapNumToFileInfo[blockinfo->m_number];
        CKernel::m_pThis->m_mapNumToFileInfo.remove(blockinfo->m_number);
    }

    file.unmap(pmmap);
    file.close();

    delete arg;



    //CKernel::m_pThis->m_CloudDlg->create_file(info->bar->m_filename);
}


void CKernel::slot_Down_FileRq(QString filename,QString filepath)
{
//    QString path = QFileDialog::getExistingDirectory (
//                        0,"保存到" );
    QString downlocalPath = "E:/云盘下载列表";
    qDebug()<<downlocalPath;
    setDownLoadInfo(downlocalPath,filename,m_UpDownFileNumber,0,9999999);

    STRU_DOWN_FILERQ rq;
    rq.m_number = m_UpDownFileNumber;
    rq.m_UserID = m_id;
    rq.m_result = file_downrq;
    rq.m_pos = 0;
    memcpy(rq.szFileName,filename.toStdString().c_str(),filename.toStdString().size());
    if(filepath =="")
    {
        memcpy(rq.szFilePath,m_CloudDlg->getfilepath().toStdString().c_str(),m_CloudDlg->getfilepath().toStdString().size());
    }
    else
    {
        memcpy(rq.szFilePath,filepath.toStdString().c_str(),filepath.toStdString().size());
    }
    m_tcp->SendData((char*)&rq,sizeof(rq));

    m_UpDownFileNumber++;
}

void CKernel::slot_Down_FileRs(char *buf, int neln)
{
    STRU_DOWN_FILERS *rs = (STRU_DOWN_FILERS*)buf;
    CProgressBar *bar;
    switch(rs->m_result)
    {
        case file_downrs:
            m_mapNumToFileInfo[rs->m_number]->bar->setTotalSize(rs->m_total_len);
            //QMessageBox::about(0,"提示",QString("开始下载%1").arg(rs->szFileName));

            break;
        case file_cancel_downrs:
            bar = m_mapNumToFileInfo[rs->m_number]->bar;
            fclose(m_mapNumToFileInfo[bar->m_number]->m_file);
            m_mapNumToFileInfo.remove(bar->m_number);
            m_mapNumToFileInfo[bar->m_number] = NULL;
            QMessageBox::about(0,"提示",QString("取消下载%1").arg(rs->szFileName));
            break;
        case file_continue_downrs:
            QMessageBox::about(0,"提示",QString("继续下载%1").arg(rs->szFileName));
            break;
        case file_suspend_downrs:
            QMessageBox::about(0,"提示",QString("暂停下载%1").arg(rs->szFileName));
            break;
        case file_down_successrs:
            //QMessageBox::about(0,"提示",QString("%1下载成功").arg(rs->szFileName));
            break;
        case file_down_failedrs:
            //QMessageBox::about(0,"提示",QString("%1下载失败").arg(rs->szFileName));
            break;
        default:
            QMessageBox::about(0,"提示","下载错误");
            break;
    }
}
//#include<FILE>

void CKernel::setDownLoadInfo(QString path, QString filename,int number,int finishsize,int totalsize)
{
    QDir tempDir;
    if(!tempDir.exists(path))
    {
        qDebug()<<"不存在sql路径";
        tempDir.mkdir(path);  //创建此路径
    }
    QString openpath = path + '/' + filename;
    const wchar_t * str = reinterpret_cast<const wchar_t  *>(openpath.utf16());
    //查看文件是否存在
    FILE *pfile;
    QFile file( openpath );
    if(file.exists())
    {
        pfile = _wfopen(str, L"rb+");
    }
    else
        pfile = _wfopen(str, L"wb+");
    FileInfo *info = new FileInfo;
    info->m_file = pfile;

    m_mapNumToFileInfo[number] = info;
    m_mapNumToFileInfo[number]->bar = create_ProgressBar(filename,finishsize,totalsize,number,progress_down);
    //m_mapNumToFileInfo[number]->m_pos = finishsize;
    m_mapNumToFileInfo[number]->bar->m_path = path;


    if(!m_NetworkSpeed_Switch)
    {
        m_NetworkSpeed_Switch = 1;
        m_Threadpool->Producer_add(m_pool,slot_show_UpDownLoad_Speed,0);
    }


}

void CKernel::setUpLoadInfo(QString path, QString filename, int number, int finishsize, int totalsize)
{
    FileInfo *info = new FileInfo;
    info->bar = create_ProgressBar(filename,finishsize,totalsize,number,progress_up);

    info->bar->on_pb_static_clicked();//暂停上传
    info->bar->m_semCount--;
    info->bar->m_path = path;
    path+='/';
    path+= filename;
    const wchar_t * str = reinterpret_cast<const wchar_t  *>(path.utf16());

    FILE *file = _wfopen(str, L"rb+");

    info->m_file = file;

    info->m_finish_len = finishsize;
    info->m_total_len = totalsize;
    m_mapNumToFileInfo[number] = info;

    if(!m_NetworkSpeed_Switch)
    {
        m_NetworkSpeed_Switch = 1;
        m_Threadpool->Producer_add(m_pool,slot_show_UpDownLoad_Speed,0);
    }
}
void CKernel::slot_datablock_recv(char *buf, int neln)
{
    //qDebug()<<"slot_datablock_recv"<<QThread::currentThreadId();
    m_Threadpool->Producer_add(m_pool,recvfile,buf);
}
void* CKernel::recvfile(void *buf)
{
    STRU_FILE_DATABLOCK *rq = (STRU_FILE_DATABLOCK*)buf;
    FILE *fp = CKernel::m_pThis->m_mapNumToFileInfo[rq->m_number]->m_file;
    CProgressBar *bar = CKernel::m_pThis->m_mapNumToFileInfo[rq->m_number]->bar;

    if(fp == NULL)
    {
        qDebug()<<"文件写入失败";
        return 0;
    }
    sem_wait(&bar->m_sem);
    fseek(fp,rq->m_pos,SEEK_SET);
    qDebug()<<rq->m_pos;
    fwrite(rq->m_block,rq->m_len,sizeof(char),fp);      //会自动向后偏移
    sem_post(&bar->m_sem);
    if(rq->m_is_finish)
    {
        fclose(fp);
        fp = NULL;
        delete CKernel::m_pThis->m_mapNumToFileInfo[rq->m_number];
        CKernel::m_pThis->m_mapNumToFileInfo.remove(rq->m_number);
        CKernel::m_pThis->m_mapNumToFileInfo[rq->m_number] = NULL;
    }

    Q_EMIT m_pThis->SIG_PRORESS_SPEED(rq->m_len,bar);
    delete[] rq;
}
void CKernel::slot_Create_GroupRq(QString groupname)
{
    STRU_CREATE_GROUPRQ rq;
    rq.m_UserID = m_id;
    memcpy(rq.szGroupName,groupname.toStdString().c_str(),groupname.toStdString().size());
    m_tcp->SendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_Share_File_Rq(QString filename)
{
    STRU_CREATE_SHARE_FILE_RQ rq;
    rq.m_UserID = m_id;
    strcpy(rq.szFileName,filename.toStdString().c_str());
    strcpy(rq.szFilePath,m_CloudDlg->getfilepath().toStdString().c_str());
    for(auto ite = m_CloudDlg->m_createBoxlst.begin();ite!=m_CloudDlg->m_createBoxlst.end();ite++)
    {
        if((*ite)->checkState())
        {
            strcpy(rq.szGroupName,(*ite)->text().toStdString().c_str());
            m_tcp->SendData((char*)&rq,sizeof(rq));
        }
    }
}

void CKernel::slot_create_groupRs(char *buf, int neln)
{
    STRU_CREATE_GROUPRS *rs = (STRU_CREATE_GROUPRS*)buf;
    m_CloudDlg->creategroup(rs->szGroupName);
}

void CKernel::slot_look_sharefileRq()
{
    STRU_LOOK_SHARE_FILE_RQ rq;
    rq.m_UserID = m_id;
    for(auto ite = m_CloudDlg->m_createBoxlst.begin();ite!=m_CloudDlg->m_createBoxlst.end();ite++)
    {
        if((*ite)->checkState())
        {
            strcpy(rq.szGroupName,(*ite)->text().toStdString().c_str());
            m_tcp->SendData((char*)&rq,sizeof(rq));
        }
    }
    for(auto ite = m_CloudDlg->m_joinBoxlst.begin();ite!=m_CloudDlg->m_joinBoxlst.end();ite++)
    {
        if((*ite)->checkState())
        {
            strcpy(rq.szGroupName,(*ite)->text().toStdString().c_str());
            m_tcp->SendData((char*)&rq,sizeof(rq));
        }
    }
}
void CKernel::slot_look_sharefileRs(char *buf, int neln)
{
    STRU_LOOK_SHARE_FILE_RS *rs = (STRU_LOOK_SHARE_FILE_RS *)buf;

    Q_EMIT SIG_GET_SHARE_LIST_RS(rs->szFileName,rs->szFilePath);

}

void CKernel::slot_Join_Share_GroupRq(QString filename)
{
    STRU_JOIN_SHARE_GROUP_RQ rq;
    rq.m_UserID = m_id;
    memcpy(rq.szGroupName,filename.toStdString().c_str(),filename.toStdString().size());
    m_tcp->SendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_Join_Share_GroupRs(char *buf, int neln)
{
    STRU_JOIN_SHARE_GROUP_RS *rs = (STRU_JOIN_SHARE_GROUP_RS*)buf;
    if(rs->m_result)
    {
        m_CloudDlg->joinsharegroup(rs->szGroupName);
    }
}

void CKernel::slot_Get_Group_InfoRq()
{
    STRU_GET_GROUP_INFORQ rq;
    rq.m_UserID = m_id;
    m_tcp->SendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_Play_SourceRs(char *buf, int neln)
{
    STRU_PLAY_SOURCERS *rs = (STRU_PLAY_SOURCERS*)buf;
    QString rtmp = VIDEO_STREAM;
    rtmp += rs->szPath;
    m_videoShow->slot_rtmp(rtmp);
    m_videoShow->show();
}

void CKernel::slot_Play_SourceRq(QString path)
{
    STRU_PLAY_SOURCERQ rq;
    memcpy(rq.szPath,path.toStdString().c_str(),MAX_SIZE);
    m_tcp->SendData((char*)&rq,sizeof(rq));
}

////////////////////////子线程处理函数/////////////////////////////////////////////
//CMySendFile::CMySendFile(QObject *parent) : QObject(parent)
//{
//}

//void CMySendFile::sendfile(int pos, int number)
//{
//    qDebug()<<QThread::currentThreadId();
//    //CKernel::m_pThis->slot_datablock_send(pos,number);
//}

//void CMySendFile::recvfile(char *buf)
//{
//    qDebug()<<QThread::currentThreadId();
//    STRU_FILE_DATABLOCK *rq = (STRU_FILE_DATABLOCK*)buf;
//    FILE *fp = CKernel::m_pThis->m_mapNumToFileInfo[rq->m_number]->m_file;
//    if(fp == NULL)
//        qDebug()<<"文件写入失败";
//    fwrite(rq->m_block,sizeof(char),rq->m_len,fp);      //会自动向后偏移
//    CProgressBar *bar = CKernel::m_pThis->m_mapNumToFileInfo[rq->m_number]->bar;
//    Q_EMIT SIG_PRORESS_SPEED(rq->m_len,bar);
//    if(rq->m_is_finish)
//    {
//        fclose(fp);
//        fp = NULL;
//        delete CKernel::m_pThis->m_mapNumToFileInfo[rq->m_number];
//        CKernel::m_pThis->m_mapNumToFileInfo.remove(rq->m_number);
//    }
//    delete[] buf;
//}

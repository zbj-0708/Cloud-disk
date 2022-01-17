#include<TCPKernel.h>
#include "packdef.h"
#include<stdio.h>

using namespace std;


////注册
//#define  DEF_PACK_REGISTER_RQ    (DEF_PACK_BASE + 0)
//#define  DEF_PACK_REGISTER_RS    (DEF_PACK_BASE + 1)
////登录
//#define  DEF_PACK_LOGIN_RQ    (DEF_PACK_BASE + 2)
//#define  DEF_PACK_LOGIN_RS    (DEF_PACK_BASE + 3)

static const ProtocolMap m_ProtocolMapEntries[] =
{
    {DEF_PACK_REGISTER_RQ , &TcpKernel::RegisterRq},
    {DEF_PACK_LOGIN_RQ , &TcpKernel::LoginRq},
    {DEF_PACK_ABNORMAL_EXIT , &TcpKernel::Abnormal_exit},
    {DEF_PACK_CREATE_FILERQ , &TcpKernel::CreateFileRq},
    {DEF_PACK_DELETE_FILERQ , &TcpKernel::DeleteFileRQ},
    {DEF_PACK_GET_FILE_LISTRQ , &TcpKernel::GetLileListRq},
    {DEF_PACK_SEND_FILERQ , &TcpKernel::Send_FileRq},
    //{DEF_PACK_FILE_DATABLOCK , &TcpKernel::DealFIleDataBlock},
    {DEF_PACK_BIG_FILE_DATABLOCK , &TcpKernel::DealMmapFIleDataBlock},
    {DEF_PACK_DOWN_FILERQ , &TcpKernel::FileDownRq},
    {DEF_PACK_CREATE_GROUPRQ , &TcpKernel::CreateGroupRq},
    {DEF_PACK_CREATE_SHARE_FILERQ , &TcpKernel::ShareFileRq},
    {DEF_PACK_LOOK_SHARE_FILERQ , &TcpKernel::LookShareFileRq},
    {DEF_PACK_JOIN_SHARE_GROUPRQ , &TcpKernel::JoinShareGroupRq},
    {DEF_PACK_GET_GROUP_INFORQ , &TcpKernel::GetGroupInfoRq},
    {DFE_PACK_PLAY_SOURCERQ , &TcpKernel::PlaySourceRq},
    {0,0}
};
#define RootPath   "/home/colin/Video/"

int TcpKernel::Open()
{
    //m_pos = -1;
    m_sql = new CMysql;
    m_redis = new Redis;
    m_tcp = new TcpNet(this);
    m_tcp->SetpThis(m_tcp);
    pthread_mutex_init(&m_tcp->alock,NULL);
    pthread_mutex_init(&m_tcp->rlock,NULL);
    if(  !m_sql->ConnectMysql("localhost","root","colin123","Cloud_Disk")  )
    {
        printf("Conncet Mysql Failed...\n");
        return FALSE;
    }
    else
    {
        printf("MySql Connect Success...\n");
    }
    if(   !m_redis->connect("127.0.0.1",_DEF_REDIS_PORT))
    {
        printf("Conncet Redis Failed...\n");
        return FALSE;
    }
    else
    {
        printf("Redis Connect Success...\n");
    }

    if( !m_tcp->InitNetWork()  )
    {
        printf("InitNetWork Failed...\n");
        return FALSE;
    }
    else
    {
        printf("Init Net Success...\n");
    }
    //文件索引前缀初始化
    int i;
    for( i=0;i<4;i++)
    {
        m_file_index[i] = 'a';
    }
    m_file_index[i] = '\0';
    pthread_mutex_init(&lock, NULL); /* 动态初始化，	成功返回0，失败返回非0 */
    //随机数初始化
    InitRand();
    return TRUE;
}

void TcpKernel::Close()
{
    m_sql->DisConnect();
    m_tcp->UnInitNetWork();
}


void TcpKernel::DealData(int clientfd,char *szbuf,int nlen)
{
    PackType *pType = (PackType*)szbuf;
    int i = 0;
    while(1)
    {
        //可以用下表直接读
        if(*pType == m_ProtocolMapEntries[i].m_type)
        {
            PFUN fun= m_ProtocolMapEntries[i].m_pfun;
            (this->*fun)(clientfd,szbuf,nlen);
        }
        else if(m_ProtocolMapEntries[i].m_type == 0 &&
                m_ProtocolMapEntries[i].m_pfun == 0)
            return;
        ++i;
    }
    return;
}


//注册
void TcpKernel::RegisterRq(int clientfd,char* szbuf,int nlen)
{
    printf("clientfd:%d RegisterRq\n", clientfd);

    STRU_REGISTER_RQ * rq = (STRU_REGISTER_RQ *)szbuf;
    STRU_REGISTER_RS rs;
    char strsql[1024] = "";  //pinjie
    sprintf(strsql,"select name from t_user where name = '%s';",rq->m_szUser);
    list<string> reslst;

    if(!m_sql->SelectMysql(strsql,1,reslst))
    {
        printf("SelectMysql error!!!\n");
    }
    if(reslst.size() == 0)
    {
        rs.m_lResult = register_sucess;
        sprintf(strsql,"insert into t_user(name,password) values('%s','%s');",
                rq->m_szUser,rq->m_szPassword);

        if(!m_sql->UpdataMysql(strsql))
        {
            printf("UpdataMysql error!!!\n");
        }
        list<string> resId;
        sprintf(strsql,"select id from t_user where name = '%s'",rq->m_szUser);
        m_sql->SelectMysql(strsql,1,resId);
        int id = 0;
        if(resId.size()>0)
        {
            id = atoi(resId.front().c_str());
        }
        sprintf(strsql,"insert into t_userInfo(id,name,icon) values(%d,'%s',0)",id,rq->m_szUser);
        m_sql->UpdataMysql(strsql);
    }
    else
    {
        rs.m_lResult = userid_is_exist;
        strcpy(rs.m_szUser,rq->m_szUser);
    }
    m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
}
//登录
void TcpKernel::LoginRq(int clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d LoginRq\n", clientfd);

    STRU_LOGIN_RQ * rq = (STRU_LOGIN_RQ *)szbuf;
    STRU_LOGIN_RS rs;

    char sql[1024] = "";
    sprintf(sql,"select password ,id from t_user where name = '%s'",rq->m_szUser);
    list<string> reslst;
    if(!m_sql->SelectMysql(sql,2,reslst))
    {
        printf("SelectMysql error!!\n");
        return;
    }


    if(reslst.size() > 0)   //用户存在
    {
        if(strcmp(reslst.front().c_str(), rq->m_szPassword) == 0)       //密码正确
        {

            rs.m_lResult = login_sucess;
            reslst.pop_front();
            int id = atoi(reslst.front().c_str());
            rs.m_UserID = id;
            if(is_online(id,clientfd))
            {
                m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
                return;
            }
            //初始化信息
            GetUserInfoFromSql(id,clientfd);

        }
        else        //密码错误
        {
            rs.m_lResult = password_error;
        }
    }
    else        //用户不存在
    {
         rs.m_lResult = userid_no_exist;
    }


    m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
}

//检测异地登录
bool TcpKernel::is_online(int userid,int fd)
{

    if(m_mapIdToUserInfo.find(userid) != m_mapIdToUserInfo.end())
    {
        //用户在线

        STRU_FORCE_OFFLINE  rq;
        m_tcp->SendData(m_mapIdToUserInfo[userid]->m_fd,(char*)&rq,sizeof(rq));
        m_mapIdToUserInfo[userid]->m_fd = fd ;
        return true;
    }

    return false;
}

//异常退出
void TcpKernel::Abnormal_exit(int clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d Abnormal_exit\n", clientfd);

    //从在线列表移除
    int userid = m_mapFdToUseId[clientfd];
    delete m_mapIdToUserInfo[userid];

    m_mapIdToUserInfo.erase(userid);
    m_mapFdToUseId.erase(clientfd);

    //查看当前是否在传输文件
    //下载列表
    DownFileCrotrol *ctrl ;
    list<DownFileCrotrol *> lst_down = m_mapUserid_To_FilelstDown[userid];
    for(auto ite = lst_down.begin();ite !=lst_down.end();ite++)
    {
            ctrl = *ite;
            ctrl->m_is_close = 1;
            if(!ctrl->m_is_play)
            {
                ctrl->m_is_play = 1;
                pthread_mutex_unlock(&ctrl->m_mutex);
            }
    }

    //上传列表
    MmapFileInfo *fileinfo ;
    list<MmapFileInfo *> lst_up = m_mapUserid_To_MmapFilelstUp[userid];
    for(auto ite = lst_up.begin();ite !=lst_up.end();ite++)
    {
        fileinfo = *ite;
        //释放资源
        pthread_mutex_destroy(&fileinfo->m_mutex);
        if(-1 == munmap(fileinfo->m_first_pos,fileinfo->m_total_len))
        {
            err_str("munmap close error:",0);
        }
        m_mapUserid_To_MmapFilelstUp[userid].remove(fileinfo);
        delete fileinfo;
        if(m_mapUserid_To_MmapFilelstUp[userid].empty())
            m_mapUserid_To_MmapFilelstUp.erase(userid);
    }



    //如果有好友之类的，
//    STRU_ABNORMAL_EXIT *rq = (STRU_ABNORMAL_EXIT*)szbuf;
//    rq->m_UserID = userid;
//    转发
}


void TcpKernel::GetUserInfoFromSql(int id ,int fd)
{


    UserInfo *info = new UserInfo;

    char strsql[1024]="";
    sprintf(strsql,"select name , icon from t_userInfo where id = %d;",id);
    list<string> reslst;
    m_sql->SelectMysql(strsql,2,reslst);
    strcpy(info->m_userName,reslst.front().c_str());
    reslst.pop_front();
    info->m_iconID = atoi(reslst.front().c_str());
    info->m_id = id;
    info->m_state = 1;
    info->m_fd = fd;
    m_mapIdToUserInfo[id] = info;
    m_mapFdToUseId[fd] = id;

}

//创建文件夹请求
void TcpKernel::CreateFileRq(int clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d CreateFileRq\n", clientfd);
    STRU_CREATE_FILE_RQ *rq = (STRU_CREATE_FILE_RQ*)szbuf;
    char key[MAX_KEY_SQL];
    strcpy(key,rq->szFilePath);
    m_redis->set("sadd",key,rq->szFileName);
}

//删除文件请求
void TcpKernel::DeleteFileRQ(int clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d DeleteFileRQ\n", clientfd);
    STRU_DELETE_FILE_RQ *rq = (STRU_DELETE_FILE_RQ*)szbuf;
    DeleteFile(rq->szFilePath,rq->szFileName);
    char key[1024];
    char value[1024];
    sprintf(key,"%d:creategroup",rq->m_UserID);
    sprintf(value,"%s:%s",rq->szFilePath,rq->szFileName);
    if(m_redis->checkexist("sismember",key,value))
    {
        m_redis->del("srem",key,value);

    }

}
//获取文件列表
void TcpKernel::GetLileListRq(int clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d GetLileListRq\n", clientfd);
    STRU_GET_FILE_LIST_RQ *rq = (STRU_GET_FILE_LIST_RQ*)szbuf;
    vector<std::string> v;
    v = m_redis->getvector("smembers" , rq->szFilePath);
    STRU_GET_FILE_LIST_RS rs;
    rs.type = rq->type;
    rs.m_UserID = rq->m_UserID;
    strcpy(rs.szFilePath,rq->szFilePath);
    while(!v.empty())
    {
        strcpy(rs.szFileName,const_cast <char *> (v[v.size()-1].c_str()));
        v.pop_back();
        m_tcp->SendData( clientfd , (char*)&rs , sizeof(rs) );
    }


}

//发送文件请求
void TcpKernel::Send_FileRq(int clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d Send_FileRq\n", clientfd);
    STRU_SEND_FILERQ *rq = (STRU_SEND_FILERQ*)szbuf;

    STRU_SEND_FILERS rs;
    rs.m_number = rq->m_number;
    memcpy(rs.szFileName,rq->szFileName,MAX_SIZE);

    //检查秒传文件
    if(m_redis->checkexist("sismember","MD5",rq->m_md5))
    {
        printf("秒传成功...\n");
        //加到客户端对应目录
        m_redis->set("sadd",rq->szFilePath,rq->szFileName);
        //找到文件索引
        string file_index = m_redis->getone("get",rq->m_md5);
        //设置文件对应映射
        string key;
        key = rq->szFilePath;
        key+=':';
        key += rq->szFileName;
        m_redis->set("set",key,file_index);
        m_redis->set("incr",rq->m_md5+file_index);
        //返回
        rs.m_result = file_miaochuan;
        m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
        return;
    }
    //取消上传
    if(rq->m_result == file_cancel_uprq)
    {
        for(auto ite = m_mapUserid_To_MmapFilelstUp[rq->m_UserID].begin();ite != m_mapUserid_To_MmapFilelstUp[rq->m_UserID].end();ite++)
        {
            if((*ite)->m_number == rq->m_number)
            {
                MmapFileInfo *fileinfo = *ite;

                m_mapUserid_To_MmapFilelstUp[rq->m_UserID].erase(ite);
                if(-1 == munmap(fileinfo->m_first_pos,fileinfo->m_total_len))
                {
                    err_str("munmap close error:",0);
                    return;
                }
                //从本地删除此文件
                string filename = fileinfo->m_file_index;
                filename += fileinfo->m_filename;
                if(-1 == remove(filename.c_str()))
                {
                       printf("删除%s失败\n",filename.c_str());
                }
                delete fileinfo;
                return;
                rs.m_result = file_cancel_uprs;
                 m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
            }
        }
    }


    //初始化结构体
    MmapFileInfo *mmapfile = new MmapFileInfo;
    //mmapfile->m_filefd = fd;
    mmapfile->m_current_len = 0;
    mmapfile->m_number = rq->m_number;
    mmapfile->m_total_len = rq->m_total_len;

    memcpy(mmapfile->m_path,rq->szFilePath,MAX_SIZE);
    memcpy(mmapfile->m_filename,rq->szFileName,MAX_SIZE);
    memcpy(mmapfile->m_md5,rq->m_md5,33);
    pthread_mutex_init(&mmapfile->m_mutex, NULL);


    //文件是否存在
    int fd;
    string filename = rq->szFileName;
    if((access(filename.c_str(),F_OK))!=-1)
    {
        printf("文件 %s 存在.\n ",filename.c_str());
        fd = open(filename.c_str(),O_RDWR);
        struct stat buf;
        fstat(fd, &buf);
        //映射内存空间
        mmapfile->m_first_pos = (char*)mmap(NULL,buf.st_size,PROT_WRITE | PROT_READ,MAP_SHARED,fd,0);
    }
    else
    {
        printf("文件不存在!,正在创建 ");
        filename = GetFIleIndex();
        filename += rq->szFileName;
        fd = open(filename.c_str(),O_RDWR |O_CREAT);
        fchmod(fd,S_IRUSR|S_IWUSR | S_IRGRP | S_IROTH |S_IXUSR);
        // lseek将文件指针往后移动 len - 1 位
        lseek(fd, rq->m_total_len - 1, SEEK_END);
        // 预先写入一个空字符；mmap不能扩展文件长度，这里相当于预先给文件长度，准备一个空架子
        write(fd, " ", 1);
        //映射内存空间
        mmapfile->m_first_pos = (char*)mmap(NULL,mmapfile->m_total_len,PROT_WRITE | PROT_READ,MAP_SHARED,fd,0);
        memcpy(mmapfile->m_file_index,filename.c_str(),4);
        //回复包
        rs.m_pos = 0;
        rs.m_result = file_uprs;
        memcpy(rs.m_cloudname,filename.c_str(),MAX_PATH);
        m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
    }

    if(MAP_FAILED == mmapfile->m_first_pos)
    {
        err_str("mmap error",0);
        exit(1);
    }
    close(fd);
    m_mapUserid_To_MmapFilelstUp[rq->m_UserID].push_back(mmapfile);



}

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h> /*必须引用这个文件 */
//void TcpKernel::DealFIleDataBlock(int clientfd ,char* szbuf,int nlen)
//{

//    STRU_FILE_DATABLOCK *rq = (STRU_FILE_DATABLOCK*)szbuf;
//    //printf("use:%d  thread:%ld\n",rq->m_UserID,pthread_self());
////    list<FileInfo*> lst = m_mapUserid_To_FilelstUp[rq->m_UserID];
////    FileInfo *fileinfo = NULL;
//    for(auto ite = lst.begin(); ite != lst.end();ite++)
//    {
//        if((*ite)->m_number == rq->m_number)
//        {
//            fileinfo = *ite;
//            break;
//        }
//    }
//    if(fileinfo == NULL)
//        return;
//    FILE *fp = fileinfo->m_file;
//    fileinfo->m_pos += rq->m_len;

//   // m_pos =rq->m_pos;
//    fwrite(rq->m_block,sizeof(char),rq->m_len,fp);      //会自动向后偏移
//    printf("%d\n",rq->m_pos);
//    if(rq->m_is_finish)
//    {
// //       m_pos = 0;
//        m_mapUserid_To_FilelstUp[rq->m_UserID].remove(fileinfo);
//        if(m_mapUserid_To_FilelstUp[rq->m_UserID].empty())
//            m_mapUserid_To_FilelstUp.erase(rq->m_UserID);
//        //关闭
//        fclose(fp);
//        //写到树据库
//        m_redis->set("sadd",fileinfo->m_path,fileinfo->m_filename);
//        string key;
//        key = fileinfo->m_path;
//        key+=':';
//        key += fileinfo->m_filename;
//        string value = fileinfo->m_file_index;
//        value += fileinfo->m_filename;
//        m_redis->set("set",key,value);
//        STRU_SEND_FILERS rs;
//        rs.m_number = rq->m_number;
//        memcpy(rs.szFileName,fileinfo->m_filename,MAX_SIZE);
//        rs.m_pos = 0;
//        rs.m_result = file_up_successrs;
//        m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
//    }
//}

void TcpKernel::DealMmapFIleDataBlock(int clientfd ,char* szbuf,int nlen)
{
    //printf("use:%d  thread:%ld\n",rq->m_UserID,pthread_self());
    STRU_BIG_FILE_DATABLOCK* rq = (STRU_BIG_FILE_DATABLOCK*)szbuf;
    list<MmapFileInfo*> lst = m_mapUserid_To_MmapFilelstUp[rq->m_UserID];
    MmapFileInfo *fileinfo = NULL;
    for(auto ite = lst.begin();ite!=lst.end();ite++)
    {
        if((*ite)->m_number == rq->m_number)
        {
            fileinfo = *ite;
            break;
        }
    }
    if(fileinfo == NULL)return;
    memcpy(fileinfo->m_first_pos+rq->m_pos,rq->m_block,rq->m_len);
    pthread_mutex_lock(&fileinfo->m_mutex); /* 阻塞的锁定互斥锁 */
    fileinfo->m_current_len += rq->m_len;
    pthread_mutex_unlock(&fileinfo->m_mutex); /* 阻塞的锁定互斥锁 */
    printf("%d--%d\n",fileinfo->m_current_len,fileinfo->m_total_len);
    //是否上传完成
    if(fileinfo->m_current_len == fileinfo->m_total_len)
    {
        printf("use:%d  %s文件上传完成\n",rq->m_UserID,fileinfo->m_filename);
        //释放资源
        pthread_mutex_destroy(&fileinfo->m_mutex);
        if(-1 == munmap(fileinfo->m_first_pos,fileinfo->m_total_len))
        {
            err_str("munmap close error:",0);
        }
        m_mapUserid_To_MmapFilelstUp[rq->m_UserID].remove(fileinfo);
        if(m_mapUserid_To_MmapFilelstUp[rq->m_UserID].empty())
            m_mapUserid_To_MmapFilelstUp.erase(rq->m_UserID);
        //写入数据库
        m_redis->set("sadd",fileinfo->m_path,fileinfo->m_filename);
        string key;
        key = fileinfo->m_path;
        key+=':';
        key += fileinfo->m_filename;
        string value = fileinfo->m_file_index;
        value += fileinfo->m_filename;
        m_redis->set("set",key,value);
        //记录MD5
        m_redis->set("sadd","MD5",fileinfo->m_md5);
        m_redis->set("set",fileinfo->m_md5,value);
        m_redis->set("set",value,fileinfo->m_md5);
        m_redis->set("set",fileinfo->m_md5+value,"1");
        STRU_SEND_FILERS rs;
        rs.m_number = rq->m_number;
        memcpy(rs.szFileName,fileinfo->m_filename,MAX_SIZE);
        rs.m_pos = 0;
        rs.m_result = file_up_successrs;
        m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));

        delete fileinfo;

    }
}
//文件下载请求
void TcpKernel::FileDownRq(int clientfd ,char* szbuf,int nlen)
{
    printf("clientfd:%d FileDownRq\n", clientfd);
    STRU_DOWN_FILERQ *rq = (STRU_DOWN_FILERQ*)szbuf;
    list<DownFileCrotrol*> lst = m_mapUserid_To_FilelstDown[rq->m_UserID];
    DownFileCrotrol *ctrl = NULL;

    STRU_DOWN_FILERS rs;
    rs.m_pos = rq->m_pos;
    rs.m_UserID = rq->m_UserID;
    rs.m_number = rq->m_number;
    memcpy(rs.szFileName,rq->szFileName,sizeof(rq->szFileName));
    for(auto ite = lst.begin();ite !=lst.end();ite++)
    {
        if((*ite)->m_number == rq->m_number)
        {
            ctrl = *ite;
            break;
        }
    }
    if(ctrl == NULL)
    {
        ctrl = new DownFileCrotrol;
        pthread_mutex_init(&ctrl->m_mutex, NULL); /* 动态初始化，	成功返回0，失败返回非0 */
        ctrl->m_number = rq->m_number;
        ctrl->m_is_close = 0;
        ctrl->m_is_play = 1;
        m_mapUserid_To_FilelstDown[rq->m_UserID].push_back(ctrl);
    }


    switch(rq->m_result)
    {
        case file_downrq:
            SendDataBlock(clientfd,szbuf);
            break;
        case file_cancel_downrq :
            if(!ctrl->m_is_play)
                pthread_mutex_unlock(&ctrl->m_mutex);
            ctrl->m_is_close = 1;
            rs.m_result = file_cancel_downrs;
            m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
            break;
        case file_suspend_downrq :
            pthread_mutex_lock(&ctrl->m_mutex);
            ctrl->m_is_play = 0;
            break;
        case file_continue_downrq :
           ctrl->m_is_play = 1;
            pthread_mutex_unlock(&ctrl->m_mutex);
            break;
        default:
            break;
    }

}

//发送数据块
void TcpKernel::SendDataBlock(int clientfd , char *szbuf)
{

        m_tcp->Addfd(clientfd,TRUE);

        STRU_DOWN_FILERQ *filerq = (STRU_DOWN_FILERQ*)szbuf;
        printf("SendDataBlock:%s\n",filerq->szFileName);
        //0.写回复包
        STRU_DOWN_FILERS rs;
        rs.m_UserID = filerq->m_UserID;
        rs.m_result = file_downrs;
        rs.m_pos = filerq->m_pos;
        rs.m_number = filerq->m_number;

        memcpy(rs.szFileName,filerq->szFileName,MAX_SIZE);

        //1.找到对应文件名字
        string key = filerq->szFilePath;
        key += ':';
        key += filerq->szFileName;
        string filename = m_redis->getone("get" , key);
        //2.只读打开文件
        FILE *fp = fopen(filename.c_str(),"r+");
        fseek(fp, 0, SEEK_END);
        int block_size = ftell(fp);
        rs.m_total_len = block_size;
        m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
        sleep(1);

        if(fp == NULL)
            return;
        fclose(fp);
        //3.建立内存映射
        int fd = open(filename.c_str(),O_RDWR);
        char *pfirst = (char*)mmap(NULL,block_size,PROT_WRITE | PROT_READ,MAP_SHARED,fd,0);
        if(MAP_FAILED == pfirst)
        {
            err_str("mmap error",0);
            exit(1);
        }
        close(fd);
        //4.初始化
        STRU_FILE_DATABLOCK rq;
        rq.m_UserID = filerq->m_UserID;
        rq.m_number = filerq->m_number;

        int pos =filerq->m_pos;


        //加入控制量

        DownFileCrotrol *ctrl ;
        list<DownFileCrotrol *> lst = m_mapUserid_To_FilelstDown[rq.m_UserID];
        for(auto ite = lst.begin();ite !=lst.end();ite++)
        {
            if((*ite)->m_number == rq.m_number)
            {
                ctrl = *ite;
                break;
            }
        }
        //4.循环发送文件
        while(pos + MAX_FILE_CONTENT_LEN < block_size && !ctrl->m_is_close)
        {
            rq.m_pos = pos;
            //printf("use:%d number:%d i:%d thread:%ld\n",rq.m_UserID,rq.m_number,rq.i,pthread_self());
            if(!ctrl->m_is_play)
            {
                pthread_mutex_lock(&ctrl->m_mutex);
                if(ctrl->m_is_close)
                    break;
                pthread_mutex_unlock(&ctrl->m_mutex);
            }

            //读到rq.m_block中
            memcpy(rq.m_block,pfirst+pos,MAX_FILE_CONTENT_LEN);
            //fread(rq.m_block,sizeof(char),MAX_FILE_CONTENT_LEN,fp);
            rq.m_len = MAX_FILE_CONTENT_LEN;
            m_tcp->SendData(clientfd,(char*)&rq,sizeof(rq));
            printf("SendDataBlock:%s  %d \n",filerq->szFileName,rq.m_pos);
            pos += MAX_FILE_CONTENT_LEN;

        }

        if(ctrl->m_is_close)
        {
            rq.m_is_finish = 0;
            //6.下载失败
            rs.m_result = file_down_failedrs;
            rq.m_len = 0;
            printf("SendDataBlock:%s取消下载 \n",filerq->szFileName);
        }
        else
        {
            rq.m_is_finish = 1;
            //6.下载完成
            printf("下载完成  use:%d number:%d thread:%ld\n",rq.m_UserID,rq.m_number,pthread_self());
            rs.m_result = file_down_successrs;
            rq.m_len = block_size-pos;
            rq.m_pos = pos;
            //5.最后不足填满缓冲区的一个包，代表完成任务
            //fread(rq.m_block,sizeof(char),block_size-pos,fp);
            memcpy(rq.m_block,pfirst+pos,block_size-pos);
            m_tcp->SendData(clientfd,(char*)&rq,sizeof(rq));

        }

        memcpy(rs.szFileName,filerq->szFileName,MAX_SIZE);
        m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
        //printf("use:%d number:%d i:%d thread:%ld\n",rq.m_UserID,rq.m_number,rq.i,pthread_self());


        //关闭
        //fclose(fp);
        //fp = NULL;
        if(-1 == munmap(pfirst,block_size))
        {
            err_str("munmap close error:",0);
            return;
        }
        pfirst = NULL;

        m_mapUserid_To_FilelstDown[rq.m_UserID].remove(ctrl);
        pthread_mutex_destroy(&ctrl->m_mutex);
        delete ctrl;
        ctrl = NULL;
        if(m_mapUserid_To_FilelstDown[rq.m_UserID].empty())
        {
            m_mapUserid_To_FilelstDown.erase(rq.m_UserID);
        }
        m_tcp->Deletefd(clientfd);

}
void TcpKernel::InitRand()
{
    struct timeval time;
    gettimeofday(&time,NULL);
    srand(time.tv_sec + time.tv_usec);
}

void TcpKernel::ShareFileRq(int clientfd, char *szbuf,int nlen)
{
    STRU_CREATE_SHARE_FILE_RQ *rq = (STRU_CREATE_SHARE_FILE_RQ*)szbuf;
    char value[1024];
    sprintf(value,"%s:%s",rq->szFilePath,rq->szFileName);
    m_redis->set("sadd",rq->szGroupName,value);

}

void TcpKernel::LookShareFileRq(int clientfd, char *szbuf,int nlen)
{
    STRU_LOOK_SHARE_FILE_RQ *rq = (STRU_LOOK_SHARE_FILE_RQ*)szbuf;
    vector<string> v;
    v = m_redis->getvector("smembers",rq->szGroupName);
    STRU_LOOK_SHARE_FILE_RS rs;
    rs.m_UserID = rq->m_UserID;
    string s;
    int n;
    for(auto ite = v.begin();ite!=v.end();ite++)
    {
        n = (*ite).find_last_of(':');
        s = (*ite).substr(n+1);
        memcpy(rs.szFileName,s.c_str(),s.size());
        s = ((*ite).substr(0,n));
        memcpy(rs.szFilePath,s.c_str(),s.size());
        m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
    }
}

void TcpKernel::JoinShareGroupRq(int clientfd, char *szbuf,int nlen)
{
    STRU_JOIN_SHARE_GROUP_RQ *rq = (STRU_JOIN_SHARE_GROUP_RQ*)szbuf;
    STRU_JOIN_SHARE_GROUP_RS rs;
    char key[1024];
    if(m_redis->checkexist("sismember","group",rq->szGroupName))
        {
        sprintf(key,"%d:joingroup",rq->m_UserID);
        m_redis->set("sadd",key,rq->szGroupName);
        rs.m_UserID = rq->m_UserID;
        rs.m_result = 1;
        memcpy(rs.szGroupName,rq->szGroupName,MAX_SIZE);
    }
    else
    {
        rs.m_result = 0;
    }
    m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
}

void TcpKernel::GetGroupInfoRq(int clientfd, char *szbuf,int nlen)
{
    STRU_GET_GROUP_INFORQ *rq = (STRU_GET_GROUP_INFORQ*)szbuf;
    char key[1024] ;
    vector<std::string> v;
    STRU_CREATE_GROUPRS creaters;
    STRU_JOIN_SHARE_GROUP_RS joinrs;
    sprintf(key,"%d:creategroup",rq->m_UserID);
    v = m_redis->getvector("smembers",key);
    creaters.m_UserID = rq->m_UserID;
    for(auto ite = v.begin();ite != v.end();ite++)
    {
        string str = *ite;
        memcpy(creaters.szGroupName,str.c_str(),(*ite).size());
        m_tcp->SendData(clientfd,(char*)&creaters,sizeof(creaters));
    }
    v.clear();
    sprintf(key,"%d:joingroup",rq->m_UserID);
    v = m_redis->getvector("smembers",key);
    joinrs.m_UserID = rq->m_UserID;
    joinrs.m_result = 1;
    for(auto ite = v.begin();ite != v.end();ite++)
    {
        string str = *ite;
        memcpy(joinrs.szGroupName,str.c_str(),(*ite).size());
        m_tcp->SendData(clientfd,(char*)&joinrs,sizeof(joinrs));
    }
}

void TcpKernel::PlaySourceRq(int clientfd, char *szbuf,int nlen)
{
    STRU_PLAY_SOURCERQ *rq = (STRU_PLAY_SOURCERQ*)szbuf;
    string str = m_redis->getone("get",rq->szPath);
    STRU_PLAY_SOURCERS rs;
    memcpy(rs.szPath,str.c_str(),MAX_SIZE);
    m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
}
void TcpKernel::CreateGroupRq(int clientfd, char *szbuf,int nlen)
{
    /*
     * key:  id:creategroup
     * value: (rand):(groupname)
     */
    STRU_CREATE_GROUPRQ *rq = (STRU_CREATE_GROUPRQ*)szbuf;
    char key[1024] ;
    char value[1024];
    sprintf(key,"%d:creategroup",rq->m_UserID);
    int groupid = rand()%10000000;
    sprintf(value,"%d:%s",groupid,rq->szGroupName);
    m_redis->set("sadd",key,value);
    m_redis->set("sadd","group",value);
    STRU_CREATE_GROUPRS rs;
    rs.m_UserID = rq->m_UserID;
    memcpy(rs.szGroupName,value,MAX_SIZE);
    m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));

}

#include<stack>
void TcpKernel::DeleteFile(char *filepath,char *filename)
{
    stack<string> s;
    string newpath ;
    newpath = filepath;
    newpath += ':';
    newpath += filename;
    s.push(newpath);
    vector<string> v;
    while(!s.empty())
    {
        v = m_redis->getvector("smembers",s.top());
        if(v.empty())
        {
            int n = s.top().find_last_of(':');
            string filename = s.top().substr(n+1,s.top().size());
            string filepath = s.top().substr(0,n);
            s.pop();
            if(s.empty())break;
            m_redis->del("srem",filepath,filename);     //页的某一个元素
            if(filename.find('.')!=string::npos)        //如果是文件
            {
                filepath = filepath+':'+filename;
                filename = m_redis->getone("get",filepath); //获取索引文件名

                m_redis->delkey("del",filepath);        //删除(路径--索引)

                string md5 = m_redis->getone("get",filename); //获取索引MD5

                if(m_redis->decr(md5+filename) == "0")   //减一
                {
                    m_redis->del("srem","MD5",md5);
                    m_redis->delkey("del",md5);        //删除(md5--索引)
                    m_redis->delkey("del",filename);        //删除(索引--md5)
                    m_redis->delkey("del",md5+filename);        //删除(路径--索引)
                    if(-1 == remove(filename.c_str()))
                    {
                        printf("删除%s失败\n",filename.c_str());
                    }
                }
            }

        }
        else
        {
            while(!v.empty())
            {
                newpath = s.top();
                newpath += ':';
                newpath += v.back();
                s.push(newpath);
                v.pop_back();
            }
        }
    }
    m_redis->del("srem",filepath,filename);
    string fname = filename;
    string fpath = filepath;
    if(fname.find('.')!=string::npos)        //如果是文件
    {
        fpath = fpath+':'+fname;
        fname = m_redis->getone("get",fpath); //获取索引文件名

        m_redis->delkey("del",fpath);        //删除(路径--索引)

        string md5 = m_redis->getone("get",fname); //获取索引MD5

        if(m_redis->decr(md5+fname) == "0")   //减一
        {
            m_redis->del("srem","MD5",md5);
            m_redis->delkey("del",md5);        //删除(md5--索引)
            m_redis->delkey("del",fname);        //删除(索引--md5)
            m_redis->delkey("del",md5+fname);        //删除(路径--索引)
            if(-1 == remove(fname.c_str()))
            {
                printf("删除%s失败\n",fname.c_str());
            }
        }
    }
//    newpath = filepath;
//    newpath += ':';
//    newpath += filename;

//    if(newpath.find('.',0) != string::npos)
//    {
//        string newfile;
//        newfile = newpath;
//        newpath = m_redis->getone("get",newpath);
//        m_redis->delkey("del",newfile);
//        if(-1 == remove(newpath.c_str()))
//        {
//               printf("删除%s失败\n",filename);
//        }
//    }


}

char* TcpKernel::GetFIleIndex()
{
    pthread_mutex_lock(&lock); /* 阻塞的锁定互斥锁 */
    if(m_file_index[3] <'z')
    {
        m_file_index[3]++;
    }
    else if(m_file_index[2] < 'z')
    {
        m_file_index[2]++;
    }
    else if(m_file_index[1] < 'z')
    {
        m_file_index[1]++;
    }
    else
    {
        m_file_index[0]++;
    }
    pthread_mutex_unlock(&lock); /* 解锁互斥锁 */
    return m_file_index;
}


















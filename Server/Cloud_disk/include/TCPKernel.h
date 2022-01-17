#ifndef _TCPKERNEL_H
#define _TCPKERNEL_H



#include "TCPNet.h"
#include "Mysql.h"
#include "redis.h"
#include <fcntl.h>
#include<sys/mman.h>
typedef struct UserInfo
{
    UserInfo()
    {
         m_fd = 0;
         m_id = 0;
         m_iconID= 0;
         m_state= 0;

         memset(m_userName, 0 , MAX_SIZE);
    }
    int  m_fd;
    int  m_id;
    int  m_iconID;
    int  m_state;
    char m_userName[MAX_SIZE];

}UserInfo;

//typedef struct FileInfo
//{

//    FileInfo()
//    {
//        m_file = new FILE;
//        m_pos = 0;
//        m_number =0;
//        memset(m_path,0,MAX_PATH);
//        //memset(m_redis_key,0,MAX_SIZE);
//        memset(m_filename,0,MAX_SIZE);
//        memset(m_file_index,0,5);

//    }
//    FILE *m_file;
//    //FILE m_file/*[MAX_CONTENT_LEN]*/;
//    int m_pos;
//    int m_number;
//    char m_path[MAX_PATH];
//    //char m_redis_key[MAX_SIZE];
//    char m_filename[MAX_SIZE];
//    char m_file_index[5];
//} FileInfo;
typedef struct MmapFileInfo
{

    MmapFileInfo()
    {

        m_current_len = 0;
        m_number =0;
        m_total_len = 0;
        memset(m_path,0,MAX_PATH);
        memset(m_filename,0,MAX_SIZE);
        memset(m_file_index,0,5);
        memset(m_md5,0,32);
    }
    //int m_filefd;
    int m_current_len;
    int m_number;
    int m_total_len;
    char *m_first_pos;
    char m_path[MAX_PATH];
    char m_filename[MAX_SIZE];
    char m_file_index[5];
    char m_md5[33];
    pthread_mutex_t m_mutex;
} MmapFileInfo;

typedef struct DownFileCrotrol
{

    DownFileCrotrol()
    {
        m_is_close = 0;
        m_number =0;
        m_is_play = 1;
    }
    int m_is_close;
    int m_number;
    int m_is_play;
    pthread_mutex_t m_mutex;
} DownFileCrotrol;

class TcpKernel;
typedef void (TcpKernel::*PFUN)(int,char*,int nlen);

typedef struct
{
    PackType m_type;
    PFUN m_pfun;
} ProtocolMap;



class TcpKernel:public IKernel
{
public:
    int Open();
    void Close();
    void DealData(int,char*,int);

    //注册
    void RegisterRq(int,char*,int);
    //登录
    void LoginRq(int,char*,int);
    bool is_online(int , int fd);
    void Abnormal_exit(int,char*,int);
    void GetUserInfoFromSql(int id,int fd);

    //创建---删除文件
    void CreateFileRq(int,char*,int);
    void DeleteFileRQ(int,char*,int);
    void GetLileListRq(int,char*,int);
    //上传文件
    void Send_FileRq(int,char*,int);
    //void DealFIleDataBlock(int,char*,int);
    void DealMmapFIleDataBlock(int,char*,int);
    //下载文件
    void FileDownRq(int,char*,int);
    void SendDataBlock(int clientfd,char* szbuf);
    //共享文件
    void InitRand();
    void CreateGroupRq(int,char*,int);
    void ShareFileRq(int,char*,int);
    void LookShareFileRq(int,char*,int);
    void JoinShareGroupRq(int,char*,int);
    void GetGroupInfoRq(int,char*,int);
    //音视频
    void PlaySourceRq(int,char*,int);
    //上传文件
    //一些算法
    void DeleteFile(char *filepath, char* filename);
    char *GetFIleIndex();
private:
    CMysql * m_sql;
    Redis *m_redis;
    TcpNet * m_tcp;
    map<int ,UserInfo*> m_mapIdToUserInfo;
    map<int ,int> m_mapFdToUseId;
    //  map<int,list<FileInfo *>> m_mapUserid_To_FilelstUp;
    map<int,list<MmapFileInfo *>> m_mapUserid_To_MmapFilelstUp;
    map<int,list<DownFileCrotrol *>> m_mapUserid_To_FilelstDown;
    char m_file_index[5];
    pthread_mutex_t lock; /* 互斥锁定义 文件唯一索引*/
    //int m_pos;
};






////读取上传的视频流信息
//void UpLoadVideoInfoRq(int,char*);
//void UpLoadVideoStreamRq(int,char*);
//void GetVideoRq(int,char*);
//char* GetVideoPath(char*);
//void QuitRq(int,char*);
//void PraiseVideoRq(int,char*);
//void GetAuthorInfoRq(int,char*);
#endif

#ifndef _PACKDEF_H
#define _PACKDEF_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "err_str.h"
#include <malloc.h>

#include<iostream>
#include<map>
#include<list>


//边界值
#define _DEF_SIZE 45
#define _DEF_BUFFERSIZE 1000
#define _DEF_PORT 8000
#define _DEF_SERVERIP "192.168.113.170"
#define _DEF_REDIS_PORT 6381


#define TRUE true
#define FALSE false


#define _DEF_LISTEN 128
#define _DEF_EPOLLSIZE 4096
#define _DEF_IPSIZE 16
#define _DEF_COUNT 10
#define _DEF_TIMEOUT 5
#define _DEF_SQLIEN 400



#define BOOL bool
#define DEF_PACK_BASE  (10000)

//#define FILE_TCP_REGULAR_BLOCK_LEN  (8212)      //文件传输固定包大小

////注册
//#define  DEF_PACK_REGISTER_RQ    (DEF_PACK_BASE + 0)
//#define  DEF_PACK_REGISTER_RS    (DEF_PACK_BASE + 1)
////登录
//#define  DEF_PACK_LOGIN_RQ    (DEF_PACK_BASE + 2)
//#define  DEF_PACK_LOGIN_RS    (DEF_PACK_BASE + 3)
////创建房间
//#define DEF_PACK_CREATEROOM_RQ  (DEF_PACK_BASE + 4)
//#define DEF_PACK_CREATEROOM_RS  (DEF_PACK_BASE + 5)
////加入房间
//#define DEF_PACK_JOINROOM_RQ  (DEF_PACK_BASE + 6)
//#define DEF_PACK_JOINROOM_RS  (DEF_PACK_BASE + 7)
////房间列表请求
//#define DEF_PACK_ROOM_MEMBER    (DEF_PACK_BASE + 8)
////音频数据
//#define DEF_PACK_AUDIO_FRAME    (DEF_PACK_BASE + 9)
////退出房间请求
//#define DEF_PACK_LEAVEROOM_RQ   (DEF_PACK_BASE + 10)
////退出房间回复
//#define DEF_PACK_LEAVEROOM_RS   (DEF_PACK_BASE + 11)

typedef enum Net_PACK
{
    DEF_PACK_REGISTER_RQ = 10000,
    DEF_PACK_REGISTER_RS,

    DEF_PACK_LOGIN_RQ,
    DEF_PACK_LOGIN_RS,

    DEF_PACK_FORCE_OFFLINE,
    DEF_PACK_ABNORMAL_EXIT,

    DEF_PACK_CREATEROOM_RQ,
    DEF_PACK_CREATEROOM_RS,




    DEF_PACK_JOINROOM_RQ ,
    DEF_PACK_JOINROOM_RS,

    DEF_PACK_ROOM_MEMBER ,

    DEF_PACK_AUDIO_FRAME,

    DEF_PACK_LEAVEROOM_RQ ,
    DEF_PACK_LEAVEROOM_RS ,

    DEF_PACK_ADD_FRIEND_RQ,
    DEF_PACK_ADD_FRIEND_RS,

    DEF_PACK_FRIEND_INFO,

    DEF_PACK_CHAT_RQ,
    DEF_PACK_CHAT_RS,

    DEF_PACK_OFFLINRE_RQ,
    DEF_PACK_OFFLINRE_RS,

    DEF_PACK_VIDEO_FRAME,

    DEF_PACK_ADD_ROOMRQ,
    DEF_PACK_ADD_ROOMRS,

    DEF_PACK_CREATE_FILERQ,

    DEF_PACK_DELETE_FILERQ,

    DEF_PACK_GET_FILE_LISTRQ,
    DEF_PACK_GET_FILE_LISTRS,

    DEF_PACK_SEND_FILERQ,
    DEF_PACK_SEND_FILERS,

    DEF_PACK_FILE_DATABLOCK,
    DEF_PACK_BIG_FILE_DATABLOCK,


    DEF_PACK_DOWN_FILERQ,
    DEF_PACK_DOWN_FILERS,

    DEF_PACK_CREATE_GROUPRQ,
    DEF_PACK_CREATE_GROUPRS,
    DEF_PACK_CREATE_SHARE_FILERQ,

    DEF_PACK_LOOK_SHARE_FILERQ,
    DEF_PACK_LOOK_SHARE_FILERS,

    DEF_PACK_JOIN_SHARE_GROUPRQ,
    DEF_PACK_JOIN_SHARE_GROUPRS,

    DEF_PACK_GET_GROUP_INFORQ,
    DEF_PACK_GET_GROUP_INFORS,


    DFE_PACK_PLAY_SOURCERQ,
    DFE_PACK_PLAY_SOURCERS,



}Net_PACK;
//注册请求结果
#define userid_is_exist      0
#define register_sucess      1

//登录请求结果
#define userid_no_exist      0
#define password_error       1
#define login_sucess         2
#define user_online          3

//创建房间结果
#define room_is_exist        0
#define create_success       1

//加入房间结果
#define room_no_exist        0
#define join_success         1

//上传请求结果
#define file_is_exist        0
#define file_uploaded        1
#define file_uploadrq_sucess 2
#define file_upload_refuse   3

//上传回复结果
#define file_uprq             0
#define file_uprs             1
#define file_cancel_uprq      2
#define file_cancel_uprs      3
#define file_up_failedrs      4
#define file_up_successrs     5
#define file_suspend_uprq     6
#define file_suspend_uprs     7
#define file_continue_uprq    8
#define file_continue_uprs    9
#define file_miaochuan        10

//下载请求结果
#define file_downrq             0
#define file_downrs             1
#define file_cancel_downrq      2
#define file_cancel_downrs      3
#define file_down_failedrs      4
#define file_down_successrs     5
#define file_suspend_downrq     6
#define file_suspend_downrs     7
#define file_continue_downrq    8
#define file_continue_downrs    9

#define _downloadfileblock_fail  0
#define _downloadfileblock_success	1

#define DEF_PACK_COUNT (100)

#define MAX_PATH            (256 )
#define MAX_SIZE            (128  )
#define DEF_HOBBY_COUNT     (8  )
#define MAX_CONTENT_LEN     (4096 )
#define MAX_FILE_CONTENT_LEN     (8192 )
#define MAX_KEY_SQL         (80)


/////////////////////网络//////////////////////////////////////


#define DEF_MAX_BUF	  1024
#define DEF_BUFF_SIZE	  4096


typedef int PackType;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//登录请求
typedef struct STRU_LOGIN_RQ
{
    STRU_LOGIN_RQ()
    {
        m_nType = DEF_PACK_LOGIN_RQ;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_szUser[MAX_SIZE] ; //用户ID
    char     m_szPassword[MAX_SIZE];  //密码
} STRU_LOGIN_RQ;


//登录回复
typedef struct STRU_LOGIN_RS
{
    STRU_LOGIN_RS()
    {
        m_nType= DEF_PACK_LOGIN_RS;
        m_UserID = 0;
        m_lResult = 0;
    }
    PackType m_nType;   //包类型
    int  m_UserID;
    int  m_lResult ; //注册结果

} STRU_LOGIN_RS;
//强制离线
typedef struct STRU_FORCE_OFFLINE
{
    STRU_FORCE_OFFLINE()
    {
        m_nType= DEF_PACK_FORCE_OFFLINE;
     //   m_UserID = 0;
    }
    PackType m_nType;   //包类型
   // int  m_UserID;

} STRU_FORCE_OFFLINE;

//异常退出DEF_PACK_ABNORMAL_EXIT
typedef struct STRU_ABNORMAL_EXIT
{
    STRU_ABNORMAL_EXIT()
    {
        m_nType= DEF_PACK_ABNORMAL_EXIT;
        m_UserID = 0;
    }
    PackType m_nType;   //包类型
    int  m_UserID;

} STRU_ABNORMAL_EXIT;



//注册请求
typedef struct STRU_REGISTER_RQ
{
    STRU_REGISTER_RQ()
    {
        m_nType = DEF_PACK_REGISTER_RQ;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_szPassword,0,MAX_SIZE);
    }

    PackType m_nType;   //包类型
    char     m_szUser[MAX_SIZE] ; //用户名
    char     m_szPassword[MAX_SIZE];  //密码

} STRU_REGISTER_RQ;

//注册回复
typedef struct STRU_REGISTER_RS
{
    STRU_REGISTER_RS()
    {
        m_nType= DEF_PACK_REGISTER_RS;
        m_lResult = 0;
         memset(m_szUser,0,MAX_SIZE);
    }
    PackType m_nType;   //包类型
    int  m_lResult ; //注册结果
    char     m_szUser[MAX_SIZE] ; //用户ID

} STRU_REGISTER_RS;

//创建房间请求
typedef struct STRU_CREATEROOM_RQ
{
    STRU_CREATEROOM_RQ()
    {
        m_nType = DEF_PACK_CREATEROOM_RQ;
        m_UserID = 0;
        m_RoomID = 0;
    }

    PackType m_nType;   //包类型
    int m_UserID;
    int m_RoomID;

} STRU_CREATEROOM_RQ;

//创建房间回复
typedef struct STRU_CREATEROOM_RS
{
    STRU_CREATEROOM_RS()
    {
        m_nType= DEF_PACK_CREATEROOM_RS;
        m_lResult = 0;
        m_RoomId = 0;
        m_AudioPort = 0;
        memset( m_AudioIP, 0, 20);
    }
    PackType m_nType;   //包类型
    int  m_lResult ;    //注册结果
    int  m_RoomId;
    int  m_AudioPort;
    char m_AudioIP[20];


} STRU_CREATEROOM_RS;

//加入房间请求
typedef struct STRU_JOINROOM_RQ
{
    STRU_JOINROOM_RQ()
    {
        m_nType = DEF_PACK_JOINROOM_RQ;
        m_UserID = 0;
        m_RoomID = 0;
    }

    PackType m_nType;   //包类型
    int m_UserID;
    int m_RoomID;

} STRU_JOINROOM_RQ;

//加入房间回复
typedef struct STRU_JOINROOM_RS
{
    STRU_JOINROOM_RS()
    {
        m_nType= DEF_PACK_JOINROOM_RS;
        m_lResult = 0;
        m_RoomId = 0;
        m_AudioPort = 0;
        memset(m_AudioIP,0,20);
    }
    PackType m_nType;   //包类型
    int  m_lResult ;    //注册结果
    int  m_RoomId;
    int m_AudioPort;
    char m_AudioIP[20];

} STRU_JOINROOM_RS;

//房间成员请求
typedef struct STRU_ROOM_MEMBER_RQ
{
    STRU_ROOM_MEMBER_RQ()
    {
        m_nType= DEF_PACK_ROOM_MEMBER;
        m_AudioPort = 0;
        m_UserID = 0;
        memset(m_szUser,0,MAX_SIZE);
        memset(m_RtmpUrl,0,MAX_SIZE);
        memset(m_AudioIP,0,MAX_SIZE);
    }
    PackType m_nType;   //包类型
    int m_UserID;
    int m_AudioPort;
    char m_szUser[MAX_SIZE];
    char m_RtmpUrl[MAX_SIZE];
    char m_AudioIP[20];

} STRU_ROOM_MEMBER_RQ;

//离开房间请求
typedef struct STRU_LEAVEROOM_RQ
{
    STRU_LEAVEROOM_RQ()
    {
        m_nType = DEF_PACK_LEAVEROOM_RQ;
        m_nUserId = 0;
        m_RoomId = 0;
    }
    PackType   m_nType;   //包类型
    int    m_nUserId; //用户ID
    int    m_RoomId;
} STRU_LEAVEROOM_RQ;

//离开房间回复
typedef struct STRU_LEAVEROOM_RS
{
    STRU_LEAVEROOM_RS()
    {
        m_nType = DEF_PACK_LEAVEROOM_RS;
        m_UserID = 0;
        memset(szUserName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szUserName[MAX_SIZE];
} STRU_LEAVEROOM_RS;


//创建文件夹请求
typedef struct STRU_CREATE_FILE_RQ
{
    STRU_CREATE_FILE_RQ()
    {
        m_nType = DEF_PACK_CREATE_FILERQ;
        m_UserID = 0;
        memset(szFilePath,0,MAX_SIZE);
        memset(szFileName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szFilePath[MAX_SIZE];
    char szFileName[MAX_SIZE];
} STRU_CREATE_FILE_RQ;

//删除文件请求
typedef struct STRU_DELETE_FILE_RQ
{
    STRU_DELETE_FILE_RQ()
    {
        m_nType = DEF_PACK_DELETE_FILERQ;
        m_UserID = 0;
        memset(szFilePath,0,MAX_SIZE);
        memset(szFileName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szFilePath[MAX_SIZE];
    char szFileName[MAX_SIZE];
} STRU_DELETE_FILE_RQ;

//获取文件列表请求
typedef struct STRU_GET_FILE_LIST_RQ
{
    STRU_GET_FILE_LIST_RQ()
    {
        m_nType = DEF_PACK_GET_FILE_LISTRQ;
        m_UserID = 0;
        type =0;
        memset(szFilePath,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    int type;
    char szFilePath[MAX_SIZE];
} STRU_GET_FILE_LIST_RQ;

//获取文件列表回复
typedef struct STRU_GET_FILE_LIST_RS
{
    STRU_GET_FILE_LIST_RS()
    {
        m_nType = DEF_PACK_GET_FILE_LISTRS;
        m_UserID = 0;
        type= 0;
        memset(szFileName,0,MAX_SIZE);
    memset(szFilePath,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    int type;
    char szFileName[MAX_SIZE];
    char szFilePath[MAX_SIZE];
} STRU_GET_FILE_LIST_RS;

//上传文件请求
typedef struct STRU_SEND_FILERQ
{
    STRU_SEND_FILERQ()
    {
        m_nType = DEF_PACK_SEND_FILERQ;
        m_UserID = 0;
        m_number = 0;
        m_total_len = 0;
        m_result = 0;
        memset(szFileName,0,MAX_SIZE);
        memset(szFilePath,0,MAX_PATH);
        memset(m_md5,0,33);
    }
    PackType   m_nType;   //包类型
    char szFileName[MAX_SIZE];
    char szFilePath[MAX_PATH];
    int m_UserID;
    int m_number;
    int m_result;
    int m_total_len;
    char m_md5[33];


}STRU_SEND_FILERQ;

//上传文回复
typedef struct STRU_SEND_FILERS
{
    STRU_SEND_FILERS()
    {
        m_nType = DEF_PACK_SEND_FILERS;
        m_pos = 0;
        m_number = 0;
        m_result = 0;
        memset(m_cloudname,0,MAX_SIZE);
        memset(szFileName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_pos;
    int m_number;
    int m_result;
    char m_cloudname[MAX_SIZE];
    char szFileName[MAX_SIZE];

}STRU_SEND_FILERS;


//数据包
typedef struct STRU_FILE_DATABLOCK
{
    STRU_FILE_DATABLOCK()
    {
        m_UserID = 0;
        m_len = 0;
        m_is_finish = 0;
        m_pos=0;
        m_number = 0;
        m_nType = DEF_PACK_FILE_DATABLOCK;
        memset(m_block,0,MAX_FILE_CONTENT_LEN);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    int m_len;
    int m_number;
    int m_is_finish;
    char m_block[MAX_FILE_CONTENT_LEN];
    int m_pos;
}STRU_FILE_DATABLOCK;

//大文件数据包
typedef struct STRU_BIG_FILE_DATABLOCK
{
    STRU_BIG_FILE_DATABLOCK()
    {
        m_nType = DEF_PACK_BIG_FILE_DATABLOCK;
        m_UserID = 0;
        m_len = 0;
        m_pos=0;
        m_number = 0;

        memset(m_block,0,MAX_FILE_CONTENT_LEN);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    int m_len;
    int m_number;
    char m_block[MAX_FILE_CONTENT_LEN];
    int m_pos;
}STRU_BIG_FILE_DATABLOCK;

//下载文件请求
typedef struct STRU_DOWN_FILERQ
{
    STRU_DOWN_FILERQ()
    {
        m_nType = DEF_PACK_DOWN_FILERQ;
        m_UserID = 0;
        m_result=  0;
        m_pos = 0;
        m_number = 0;
        memset(szFileName,0,MAX_SIZE);
        memset(szFilePath,0,MAX_PATH);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szFileName[MAX_SIZE];
    char szFilePath[MAX_PATH];
    int m_result;
    int m_pos;
    int m_number;
}STRU_DOWN_FILERQ;

//下载文件回复
typedef struct STRU_DOWN_FILERS
{
    STRU_DOWN_FILERS()
    {
        m_nType = DEF_PACK_DOWN_FILERS;
        m_UserID = 0;
        m_pos = 0;
        m_number = 0;
        m_result=  0;
        m_total_len = 0;
        memset(szFileName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    int m_result;
    int m_pos;
    int m_number;
    int m_total_len;
    char szFileName[MAX_SIZE];
}STRU_DOWN_FILERS;

////////////////共享文件///////////////////
//创建共享文件
typedef struct STRU_CREATE_GROUPRQ
{
    STRU_CREATE_GROUPRQ()
    {
        m_nType = DEF_PACK_CREATE_GROUPRQ;
        m_UserID = 0;
        memset(szGroupName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szGroupName[MAX_SIZE];
}STRU_CREATE_GROUPRQ;
typedef struct STRU_CREATE_GROUPRS
{
    STRU_CREATE_GROUPRS()
    {
        m_nType = DEF_PACK_CREATE_GROUPRS;
        m_UserID = 0;
        memset(szGroupName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szGroupName[MAX_SIZE];
}STRU_CREATE_GROUPRS;
//创建共享文件夹请求
typedef struct STRU_CREATE_SHARE_FILE_RQ
{
    STRU_CREATE_SHARE_FILE_RQ()
    {
        m_nType = DEF_PACK_CREATE_SHARE_FILERQ;
        m_UserID = 0;
        memset(szFilePath,0,MAX_SIZE);
        memset(szFileName,0,MAX_SIZE);
        memset(szGroupName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szFilePath[MAX_SIZE];
    char szFileName[MAX_SIZE];
    char szGroupName[MAX_SIZE];
} STRU_CREATE_SHARE_FILE_RQ;


//查看共享文件请求
typedef struct STRU_LOOK_SHARE_FILE_RQ
{
    STRU_LOOK_SHARE_FILE_RQ()
    {
        m_nType = DEF_PACK_LOOK_SHARE_FILERQ;
        m_UserID = 0;
        memset(szGroupName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szGroupName[MAX_SIZE];
} STRU_LOOK_SHARE_FILE_RQ;
typedef struct STRU_LOOK_SHARE_FILE_RS
{
    STRU_LOOK_SHARE_FILE_RS()
    {
        m_nType = DEF_PACK_LOOK_SHARE_FILERS;
        m_UserID = 0;
        memset(szFilePath,0,MAX_SIZE);
        memset(szFileName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szFilePath[MAX_SIZE];
    char szFileName[MAX_SIZE];
} STRU_LOOK_SHARE_FILE_RS;
//加入共享群
typedef struct STRU_JOIN_SHARE_GROUP_RQ
{
    STRU_JOIN_SHARE_GROUP_RQ()
    {
        m_nType = DEF_PACK_JOIN_SHARE_GROUPRQ;
        m_UserID = 0;
        memset(szGroupName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szGroupName[MAX_SIZE];
} STRU_JOIN_SHARE_GROUP_RQ;
typedef struct STRU_JOIN_SHARE_GROUP_RS
{
    STRU_JOIN_SHARE_GROUP_RS()
    {
        m_nType = DEF_PACK_JOIN_SHARE_GROUPRS;
        m_UserID = 0;
        m_result = 0;
        memset(szGroupName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    int m_result;
    char szGroupName[MAX_SIZE];
} STRU_JOIN_SHARE_GROUP_RS;
//获取组信息
typedef struct STRU_GET_GROUP_INFORQ
{
    STRU_GET_GROUP_INFORQ()
    {
        m_nType = DEF_PACK_GET_GROUP_INFORQ;
        m_UserID = 0;
    }
    PackType   m_nType;   //包类型
    int m_UserID;
} STRU_GET_GROUP_INFORQ;
typedef struct STRU_GET_GROUP_INFORS
{
    STRU_GET_GROUP_INFORS()
    {
        m_nType = DEF_PACK_GET_GROUP_INFORS;
        m_UserID = 0;
        memset(szGroupName,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    int m_UserID;
    char szGroupName[MAX_SIZE];
} STRU_GET_GROUP_INFORS;

typedef struct STRU_PLAY_SOURCERQ
{
    STRU_PLAY_SOURCERQ()
    {
        m_nType = DFE_PACK_PLAY_SOURCERQ;
        memset(szPath,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    char szPath[MAX_SIZE];

} STRU_PLAY_SOURCERQ;
typedef struct STRU_PLAY_SOURCERS
{
    STRU_PLAY_SOURCERS()
    {
        m_nType = DFE_PACK_PLAY_SOURCERS;
        memset(szPath,0,MAX_SIZE);
    }
    PackType   m_nType;   //包类型
    char szPath[MAX_SIZE];

} STRU_PLAY_SOURCERS;


#endif




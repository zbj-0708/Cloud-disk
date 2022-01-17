#ifndef QTCPCLIENT_H
#define QTCPCLIENT_H

#include <QObject>
#include"TCPNet.h"
#include"tcpfilenet.h"
#define DEF_SERVER_IP   "192.168.113.170"/*"192.168.43.188"*//*192.168.5.147"/*"192.168.0.113"*//*"10.168.1.145"*/

//此类用于调用TCPNet，
#define DEF_TCP_PORT   8000
class QMyTcpClient : public QObject,public ITcpMediator
{
    Q_OBJECT
public:
    explicit QMyTcpClient(QObject *parent = 0);
    ~QMyTcpClient();
signals:
    void SIG_ReadyData( char* szbuf , int nLen );
public slots:
    bool InitNetWork(char* szBufIP = DEF_SERVER_IP, unsigned short port = DEF_TCP_PORT);    //对内调用
    void setIpAndPort(char* szBufIP = DEF_SERVER_IP, unsigned short port = DEF_TCP_PORT);   //对外调用 目前没用上
    void UnInitNetWork();                                                                   //对内调用
    int SendData(char* szbuf,int nLen);                         //发送数据                      对外调用
    int ConnectToServer( char* szBufIP= DEF_SERVER_IP);         //目前没用上                  对外调用
    void DealData(char* szbuf , int nLen);      //重写了ITcpMediator中的数据处理                 对外调用，通过SIG_ReadyData与外部传递数据
    int IsConnected();                                                                      //对内调用
    virtual void InfoTcpNet();
public:
    TCPNet *m_pTcp;         //new此对象是传入一个this指针，此对象中有一个成员ITcpMediator用于数据处理，
                            //this正好是他的子类，所以调用此对象中的数据处理其实就是调用自己的数据处理

    char  m_szBufIP[50];
    unsigned short m_port ;
};

class QMyFileTcpClient : public QMyTcpClient
{
public:
    QMyFileTcpClient(){
    }
    virtual void InfoTcpNet();
};

#endif // QTCPCLIENT_H

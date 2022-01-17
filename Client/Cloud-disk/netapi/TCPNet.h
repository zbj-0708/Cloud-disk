#ifndef TCPNET_H
#define TCPNET_H

#include <process.h>
#include <winsock2.h>
#include<pthread.h>
class ITcpMediator
{
public:
    virtual void DealData(char* szbuf , int nLen) = 0;
    virtual ~ITcpMediator(){}
};

class TCPNet
{
public:
    TCPNet(ITcpMediator *pMediator);
	~TCPNet();
public:
     bool InitNetWork(char* szBufIP , unsigned short port);         //初始化网络之后开辟一个接受数据的线程，为了让调用它的类有一个数据处理函数，
                                                                    //引入了中介者m_pMediator，线程处理完数据调用m_pMediator中的dealdata，然后发送信号让之日那个槽函数接收
	 void UnInitNetWork();
     virtual int SendData(char* szbuf,int nLen);
     int ConnectToServer( char* szBufIP);
     int IsConnected();
public:
	static  unsigned  __stdcall ThreadProc( void * );
public:
	SOCKET m_sockClient;
	HANDLE m_hThread;
	bool   m_bFlagQuit;
    unsigned short m_port;
    ITcpMediator *m_pMediator;      //数据处理用他发送，他的初始化是指向QMyTcpClient中的m_pMediator，
    bool  m_isConnected;
private:
    //pthread_mutex_t m_mutex;        //多线程用此网络发送消息时，用到锁

};



#endif//TCPNET_H

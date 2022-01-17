#include "tcpfilenet.h"
#include"QDebug"
TCPFileNet::TCPFileNet(ITcpMediator *pMediator):TCPNet(pMediator)
{

}

int TCPFileNet::SendData(char *szbuf, int nLen)
{
    if(!szbuf || nLen <=0)
        return -1;
//    pthread_mutex_lock(&m_mutex);
//    int n = send(m_sockClient,(const char*)&nLen,sizeof(int),0);
//    if(n <=0)
//    {
//        pthread_mutex_unlock(&m_mutex);
//        return -1;
//    }
    //qDebug()<<nLen<<"  "<<QThread::currentThreadId();
    int n = send(m_sockClient,szbuf,nLen,0);
    qDebug()<<"发送文件大小"<<n;
    //pthread_mutex_unlock(&m_mutex);
    return n;
}

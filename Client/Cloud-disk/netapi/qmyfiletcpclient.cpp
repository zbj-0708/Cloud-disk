#include "qmyfiletcpclient.h"

QMyFileTcpClient::QMyFileTcpClient(QObject *parent) : QObject(parent)
{

}

int QMyFileTcpClient::SendData(char *szbuf, int nLen)
{
    //判断是否处于连接
    if( IsConnected() )
        return m_pTcp->SendData(szbuf,nLen);
    else
    {
        m_pTcp->UnInitNetWork( );
        delete m_pTcp;
        m_pTcp = new TCPNet(this);
        if( this->InitNetWork(m_szBufIP , m_port) )
        {
            return m_pTcp->SendData(szbuf,nLen);
        }
        else
        {
            return -1;
        }
    }
}

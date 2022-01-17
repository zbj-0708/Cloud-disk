#ifndef TCPFILENET_H
#define TCPFILENET_H

#include"TCPNet.h"

class TCPFileNet : public TCPNet
{
public:
    TCPFileNet(ITcpMediator *pMediator);
    virtual int SendData(char* szbuf,int nLen);
};

#endif // TCPFILENET_H

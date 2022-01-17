#ifndef QMYFILETCPCLIENT_H
#define QMYFILETCPCLIENT_H

#include <QObject>
#include"qmytcpclient.h"
class QMyFileTcpClient : public QObject ,public QMyTcpClient
{
    Q_OBJECT
public:
    explicit QMyFileTcpClient(QObject *parent = 0);

signals:
public slots:
    virtual int SendData(char* szbuf,int nLen);                 //发送数据  对外调用
};

#endif // QMYFILETCPCLIENT_H

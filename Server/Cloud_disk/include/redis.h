#ifndef _REDIS_H_
#define _REDIS_H_

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <hiredis/hiredis.h>

class Redis
{
public:

    Redis(){}
    ~Redis()
    {
        this->_connect = NULL;
        this->_reply = NULL;
    }
public:
    bool connect(std::string host, int port);
    std::string decr(std::string type);
    std::string getone(std::string type,std::string key);
    std::vector<std::string> getvector(std::string type,std::string key);
    void set(std::string type,std::string key);
    void set(std::string type,std::string key, std::string value);
    void del(std::string type,std::string key, std::string value);
    void delkey(std::string type,std::string key);
    bool checkexist(std::string type,std::string key, std::string value);
    //订阅
    void subscribe(std::string id ,std::string key);
    //发送
    void pushlist(std::string key,std::string value);
private:

    redisContext* _connect;
    redisReply* _reply;

};

#endif  //_REDIS_H_

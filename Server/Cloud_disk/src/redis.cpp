#include"redis.h"

bool Redis::connect(std::string host, int port)
{
    this->_connect = redisConnect(host.c_str(), port);
    if(this->_connect != NULL && this->_connect->err)
    {
        printf("connect error: %s\n", this->_connect->errstr);
        return 0;
    }

    return 1;
}

std::string Redis::decr( std::string key)
{
    redisCommand(this->_connect, "decr %s ", key.c_str());

    this->_reply = (redisReply*)redisCommand(this->_connect, "get %s" ,key.c_str());
    std::string str = this->_reply->str;
    freeReplyObject(this->_reply);
    return str;
}

std::string Redis::getone(std::string type,std::string key)
{
    this->_reply = (redisReply*)redisCommand(this->_connect, "%s %s", type.c_str() ,key.c_str());
    std::string str = this->_reply->str;
    freeReplyObject(this->_reply);
    return str;
}


std::vector<std::string> Redis::getvector(std::string type,std::string key)
{
    this->_reply = (redisReply*)redisCommand(this->_connect, "scard %s", key.c_str());
    int valueSize = this->_reply->integer;

    this->_reply = (redisReply*)redisCommand(this->_connect, "%s %s", type.c_str() ,key.c_str());
    redisReply** replyVector = this->_reply->element;//获取数组指针
    std::vector<std::string> result;
    for(int i=0;i<valueSize;i++)
    {
        std::string temp =(*replyVector)->str;//遍历redisReply*数组,存入vector向量

        result.push_back(temp);
        replyVector++;
    }
    return result;
}

void Redis::set(std::string type, std::string key)
{
    redisCommand(this->_connect, "%s %s ", type.c_str() , key.c_str());
}



void Redis::set(std::string type,std::string key, std::string value)
{
    redisCommand(this->_connect, "%s %s %s", type.c_str() , key.c_str(), value.c_str());
}
void Redis::delkey(std::string type,std::string key)
{
    redisCommand(this->_connect, "%s %s", type.c_str() , key.c_str());
}

bool Redis::checkexist(std::string type, std::string key, std::string value)
{
    this->_reply = (redisReply*)redisCommand(this->_connect, "%s %s  %s", type.c_str(),key.c_str(),value.c_str());
    //redisReply** replyVector = this->_reply->element;//获取数组指针
    return  this->_reply->integer;//遍历redisReply*数组,存入vector向量
}
void Redis::del(std::string type,std::string key, std::string value)
{
    redisCommand(this->_connect, "%s %s %s", type.c_str() , key.c_str(), value.c_str());
}




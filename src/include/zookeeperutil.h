#pragma once

#include<string>
#include<zookeeper/zookeeper.h>
#include<semaphore.h>


//zk客户端类
class ZkClient{

public:
    ZkClient();
    ~ZkClient();
    //zkclient启动链接zkserver
    void Start();
    //在zkserver上根据指定的path创建znode节点
    void Create(const char* path,const char* data,int datalen,int state=0);
    //根据参数指定的znode节点路径，或者znode节点的值
    std::string GetData(const char *path);

private:
    //zk的客户端句柄
    zhandle_t *m_zhandle;
};
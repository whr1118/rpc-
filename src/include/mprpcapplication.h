#pragma once

#include"mprpcconfig.h"
#include"mprpcchannel.h"
#include"mprpccontroller.h"

//mprpc框架的基础类,负责框架的一些初始化操作
class MprpcApplication{
public:
    static void Init(int argc,char **argv);

    //获取实例的方法
    static MprpcApplication& GetInstance();

    //返回配置
    static MprpcConfig& GetConfig();

private:
    static MprpcConfig m_config;

    MprpcApplication(){}
    MprpcApplication(const MprpcApplication&)=delete;
    MprpcApplication(MprpcApplication&&)=delete;
};

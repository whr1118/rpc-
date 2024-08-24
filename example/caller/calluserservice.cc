#include<iostream>
#include"mprpcapplication.h"
#include"user.pb.h"
#include"mprpcchannel.h"

int main(int argc,char **argv){

    //整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数
    MprpcApplication::Init(argc,argv);

    //演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    //rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("刘子阳");
    request.set_pwd("123456");

    //rpc方法的响应
    fixbug::LoginResponse response;


    //RpcChannel->RpcChannel::callMethod集中来做rpc方法调用的参数序列化和网络发送
    //rpc方法的调用 同步的rpc方法的调用：MprpcChannel->callMethod()
    stub.Login(nullptr,&request,&response,nullptr);


    //rpc调用完成，读调用的结果
    if(response.result().errcode()==0){
        std::cout<<"rpc login response success: "<<response.success()<<std::endl;
    }else{
        std::cout<<"rpc login response error: "<<response.result().errmsg()<<std::endl;
    }

    //演示远程发布的注册功能Register
    //演示start
    fixbug::RegisterReuqest req;
    req.set_id(250);
    req.set_name("小刘子阳");
    req.set_pwd("987654");
    fixbug::RegisterResponse rsp;

    //已同步的方式发起rpc调用请求，等待返回结果
    stub.Register(nullptr,&req,&rsp,nullptr);

    if(rsp.result().errcode()==0){
        std::cout<<"rpc register response success: "<<rsp.success()<<std::endl;
    }else{
        std::cout<<"rpc register response error: "<<rsp.result().errmsg()<<std::endl;
    }
    //演示end


    return 0;
}
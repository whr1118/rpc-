#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"



/*
UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendLists
*/

//使用在RPC服务发布端（RPC服务提供者）
class UserService : public fixbug::UserServiceRpc{
public:
    bool Login(std::string name,std::string pwd){
        std::cout<<"doing local service: Login"<<std::endl;
        std::cout<<"name:"<<name<<"pwd:"<<pwd<<std::endl;
        return true;
    }
    
    bool Register(uint32_t id,std::string name,std::string pwd){
        std::cout<<"doing local service: Register"<<std::endl;
        std::cout<<"id: "<<id<<"name: "<<name<<"pwd: "<<pwd<<std::endl;
        return true;
    }

    //重写基类UserServiceRpc的虚函数
    //下面这些方法都是框架直接调用的，此时是处在callee的角色
    /*1.caller  ->  Login(LoginRequest) ->  muduo   ->callee
      2.callee  ->  Login(LoginRequest) ->  交到下面重写的这个Login方法上
    */
    void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterReuqest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done){
        uint32_t id=request->id();
        std::string name=request->name();
        std::string pwd=request->pwd();

        bool ret=Register(id,name,pwd);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(ret);
    }


    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done){
        //框架给业务上报了请求参数LoginRequest，应用获取响应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        //做本地业务
        bool login_result = Login(name,pwd);

        //把响应写入  包括错误码、错误消息、返回值
        fixbug::ResultCode *code=response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        //执行回调操作  执行相应对象数据的序列化和网络发送（由框架实现）
        done->Run();
        
    }
};

int main(int argc,char** argv){
    
    //调用框架的初始化操作
    MprpcApplication::Init(argc,argv);

    //把UserService对象发布到rpc节点上
    //provider是一个rpc网络服务对象
    RpcProvider provider;
    provider.NotifyService(new UserService());

    //启动一个rpc服务发布节点，Run之后线程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}
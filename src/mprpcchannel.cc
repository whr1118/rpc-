#include"mprpcchannel.h"
#include<string>
#include"rpcheader.pb.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include"mprpcapplication.h"
#include"mprpccontroller.h"
#include"zookeeperutil.h"

/*
header_size + service_name method_name args_size + args
*/
//所有通过stub代理对象调用的rpc方法，都走到这里了，统一做rpc方法调用的数据序列化和网络发送
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                        google::protobuf::RpcController* controller, 
                        const google::protobuf::Message* request,
                        google::protobuf::Message* response, 
                        google::protobuf::Closure* done){
    
    const google::protobuf::ServiceDescriptor* sd=method->service();
    std::string service_name=sd->name();
    std::string method_name=method->name();

    //获取参数的序列化字符串长度args_size
    uint32_t args_size=0;
    std::string args_str;
    if(request->SerializeToString(&args_str)){//序列化成功
        args_size=args_str.size();
    }else{
        // std::cout<<"serialize request error!"<<std::endl;
        controller->SetFailed("serialize request error!");
        return;
    }

    //定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    // 序列化
    uint32_t header_size=0;
    std::string rpc_header_str;
    if(rpcHeader.SerializeToString(&rpc_header_str)){//成功
        header_size=rpc_header_str.size();
    }else{//失败
        // std::cout<<"serialize rpc header error!"<<std::endl;
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    //组织待发送的rpc请求的字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0,std::string((char*)&header_size,4));//header_size
    send_rpc_str+=rpc_header_str;//rpcheader
    send_rpc_str+=args_str;//args

    //打印调试信息
    std::cout<<"======================================="<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header_str: "<<rpc_header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"method_name: "<<method_name<<std::endl;
    std::cout<<"args_str: "<<args_str<<std::endl;
    std::cout<<"======================================="<<std::endl;

    //使用Tcp变成，完成rpc方法的远程调用
    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    if(clientfd==-1){
        // std::cout<<"create socket error! errno: "<< errno << std::endl;
        char errtxt[512]={0};
        sprintf(errtxt,"create socket error! errno: %d",errno);
        controller->SetFailed(errtxt);
        return;
    }

    //读取配置文件rpcserver的信息
    // std::string ip=MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port=atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());    
    //rpc调用方法调用service_name的method_name服务，需要查询zk上服务所在的host信息
    ZkClient zkCli;
    zkCli.Start();
    std::string method_path="/"+service_name+"/"+method_name;
    std::string host_data=zkCli.GetData(method_path.c_str());
    if(host_data==""){
        controller->SetFailed(method_path+" is not exist!");
        return;
    }
    int idx=host_data.find(":");
    if(idx==-1){
        controller->SetFailed(method_path+"address is invalid!");
        return;
    }
    std::string ip=host_data.substr(0,idx);
    uint16_t port=atoi(host_data.substr(idx+1,host_data.size()-idx).c_str());


    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=inet_addr(ip.c_str());


    //发起连接
    if(connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
        // std::cout<<"connect error! errno: "<< errno <<std::endl;
        close(clientfd);
        char errtxt[512]={0};
        sprintf(errtxt,"connect error! errno: %d",errno);
        controller->SetFailed(errtxt);
        return;
    }

    //发送rpc请求
    if(send(clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0)==-1){
        // std::cout<<"send error! errno: "<< errno <<std::endl;
        close(clientfd);
        char errtxt[512]={0};
        sprintf(errtxt,"send error! errno: %d",errno);
        controller->SetFailed(errtxt);
        return;     
    }

    //接受rpc请求的响应值
    char recv_buf[1024]={0};
    int recv_size=0;
    if((recv_size=recv(clientfd,recv_buf,1024,0))==-1){
        // std::cout<<"recv error! errno: "<< errno <<std::endl;
        close(clientfd);
        char errtxt[512]={0};
        sprintf(errtxt,"recv error! errno: %d",errno);
        controller->SetFailed(errtxt);
        return;
    }

    //反序列化rpc调用的响应数据
    // std::string response_str(recv_buf,0,recv_size);//出现bug,recv_buf中遇到\0后面的数据就存不下来
    // if(!response->ParseFromString(response_str)){
    if(!response->ParseFromArray(recv_buf,recv_size)){
        // std::cout<<"parse error! response_str: "<<recv_buf<<std::endl;
        close(clientfd);
        char errtxt[1024]={0};
        // sprintf(errtxt,"parse error! errno: %s",recv_buf);编译警告，会溢出，recv_buf为1024，errtxt为512
        snprintf(errtxt, sizeof(errtxt), "parse error! errno: %.1003s", recv_buf);

        controller->SetFailed(errtxt);
        return;
    }

    close(clientfd);
}
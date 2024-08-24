#include"test.pb.h"
#include<iostream>
#include<string>

using namespace fixbug;

// int main(){
//     //封装了login请求对象的数据
//     LoginRequest req;
//     req.set_name("刘子阳");
//     req.set_pwd("123456");

//     //对象数据序列化为char*
//     std::string send_str;
//     if(req.SerializeToString(&send_str)){//如果序列化成功，就放到send_str中
//         std::cout << send_str << std::endl;
//     }

//     //反序列化
//     //从send_str反序列化为一个login请求对象
//     LoginRequest reqB;
//     if(reqB.ParseFromString(send_str)){
//         std::cout << reqB.name() << std::endl;
//         std::cout << reqB.pwd() << std::endl;
//     }

//     return 0;
// }

int main(){
    // LoginResponse rsp;
    // ResultCode *rc=rsp.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("登陆失败了");

    GetFriendListResponse rsp;
    ResultCode *rc=rsp.mutable_result();
    rc->set_errcode(0);
    
    User *user1= rsp.add_friend_list();
    user1->set_name("刘子阳");
    user1->set_age(18);
    user1->set_sex(User::man);

    User *user2= rsp.add_friend_list();
    user2->set_name("刘子阳");
    user2->set_age(18);
    user2->set_sex(User::man);

    std::cout<<rsp.friend_list_size() <<std::endl;


    return 0;
}
# aux_source_directory(. SRC_LIST)

set(SRC_LIST 
    mprpcapplication.cc 
    mprpcconfig.cc 
    rpcheader.pb.cc 
    rpcprovider.cc 
    mprpcchannel.cc
    mprpccontroller.cc
    logger.cc
    zookeeperutil.cc)
# add_library(mprpc SHARED ${SRC_LIST})
#因为muduo是.a文件是静态库，生成的.so文件是动态库，链接会发生问题
add_library(mprpc ${SRC_LIST})
#要注意base和net库的顺序
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)
#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "CServer.h"
#include "ConfigMgr.h"
#include "const.h"
#include "RedisMgr.h"



void TestRedisMgr() {

    assert(RedisMgr::GetInstance()->Set("blogwebsite", "llfc.club"));//通过单例模式返回
    std::string value = "";
    assert(RedisMgr::GetInstance()->Get("blogwebsite", value));
    assert(RedisMgr::GetInstance()->Get("nonekey", value) == false);
    assert(RedisMgr::GetInstance()->HSet("bloginfo", "blogwebsite", "llfc.club"));
    assert(RedisMgr::GetInstance()->HGet("bloginfo", "blogwebsite") != "");
    assert(RedisMgr::GetInstance()->ExistsKey("bloginfo"));
    assert(RedisMgr::GetInstance()->Del("bloginfo"));
    assert(RedisMgr::GetInstance()->Del("bloginfo"));
    assert(RedisMgr::GetInstance()->ExistsKey("bloginfo") == false);
    assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue1"));
    assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue2"));
    assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue3"));
    assert(RedisMgr::GetInstance()->RPop("lpushkey1", value));
    assert(RedisMgr::GetInstance()->RPop("lpushkey1", value));
    assert(RedisMgr::GetInstance()->LPop("lpushkey1", value));
    assert(RedisMgr::GetInstance()->LPop("lpushkey2", value) == false);//assert()如果里面是false，程序会立刻崩溃
    
}
int main()
{
    //TestRedis();
    //TestRedisMgr();
	auto & gCfgMgr =  ConfigMgr::Inst();
	std::string gate_port_str =  gCfgMgr["GateServer"]["Port"];//取出GateServer的端口号。
	unsigned short gate_port = atoi(gate_port_str.c_str());
	try {
		unsigned short port = static_cast<unsigned short>(8080);
		net::io_context ioc{ 1 };//创建并发数为1的io_context上下文
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);//用于注册和处理信号的类 SIGINT: 通常由用户按下 Ctrl+C 触发的中断信号。
	//SIGTERM: 终止信号，通常由系统或管理员发送，要求程序终止。
		signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
			if (error) {//信号接受失败，直接返回
				return;
			}
			ioc.stop();//外部信号导致停止
			});
		std::make_shared<CServer>(ioc, port)->Start();//用于接收链接
		std::cout << "Gate Server listen on port:" << port << std::endl;
		ioc.run();//运行ioc
		
	}
	catch (std::exception const& e) {
		std::cerr << "Error" << e.what() << std::endl;
		return	EXIT_FAILURE;
	}
}
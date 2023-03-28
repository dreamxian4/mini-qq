#pragma once
#include<WinSock2.h>
#include<iostream>
using namespace std;

class INetmediator;//声明有一个INetmediator的类，不需要包含头文件
class INet {
public:
	INet() {};
	virtual ~INet() {};//使用时，父类指针指向子类对象，需要使用虚析构
	//初始化网络
	virtual bool InitNet() = 0;
	//关闭网络
	virtual void UninitNet() = 0;
	//发送数据
	virtual bool SendData(long lSendIP, char* buf,int nlen) = 0;
protected:
	//接收数据
	virtual bool RecvData() = 0;
	INetmediator* m_pMediator;
};

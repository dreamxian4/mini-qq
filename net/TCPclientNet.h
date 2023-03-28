#pragma once
#include"INet.h"

class TcpClientNet : public INet {
public:
	TcpClientNet();
	TcpClientNet(INetmediator* p);
	~TcpClientNet();//使用时，父类指针指向子类对象，需要使用虚析构
   //初始化网络
	bool InitNet();
	//关闭网络
	void UninitNet();
	//发送数据
	bool SendData(long lSendIP, char* buf, int nlen);
protected:
	static unsigned int _stdcall recvthread(LPVOID p);
	//接收数据
	bool RecvData();
	SOCKET m_sock;
	HANDLE handle;
	bool m_isstop;
};

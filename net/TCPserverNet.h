#pragma once
#include"INet.h"
#include<list>
#include<map>

class TcpServerNet : public INet {
public:
	TcpServerNet();
	TcpServerNet(INetmediator* p);
	~TcpServerNet();//使用时，父类指针指向子类对象，需要使用虚析构
   //初始化网络
	bool InitNet();
	//关闭网络
	void UninitNet();
	//发送数据
	bool SendData(long lSendIP, char* buf, int nlen);
protected:
	static unsigned int _stdcall recvthread(LPVOID p);
	static unsigned int _stdcall acceptthread(LPVOID p);
	//接收数据
	bool RecvData();
	SOCKET m_sock;//用于连接的socket
	bool m_isstop;
	list<HANDLE>m_hThreadHandleList;//保存线程句柄
	map<unsigned int, SOCKET>m_mapThreadToSocket;//保存客户端和对应sock的映射关系
};

#include "UDPnet.h"
#include"PackDef.h"
#include"UDPmediator.h"
#include<process.h>


UDPnet::UDPnet() {}
UDPnet::~UDPnet() {}

UDPnet::UDPnet(INetmediator* p):m_sock(INVALID_SOCKET), handle(0),m_isstop(false){
	m_pMediator = p;
}

//初始化网络
bool UDPnet::InitNet()
{
	//1.选项目--加载库
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);//2.2版本号
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {//err等于0，说明加载成功
		printf("WSAStartup failed with error:%d\n", err);
		system("pause");
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {//判断版本号是否为2.2
		printf("Could not find a usable version of Winsock.dll\n");
		UninitNet();
		system("pause");
		return false;
	}

	//2.雇人--创建套接字
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);//三个参数分别代表：ipv4地址 使用udp协议 UDP协议
	if (m_sock == INVALID_SOCKET) {//创建后依然非法
		printf("socket function failed with error=%d\n", WSAGetLastError());//打印错误码（工具->错误查找->输入错误码）
		UninitNet();
		system("pause");
		return false;
	}

	//有限广播权限
	BOOL bval = TRUE;
	setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bval, sizeof(bval));

	//绑定IP
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;//绑定任意网卡
	//inet_addr 字符串转换成u_long类型
	service.sin_port = htons(_DEF_UDP_PORT);//端口号
	err = bind(m_sock, (SOCKADDR*)&service, sizeof(service));
	if (err == SOCKET_ERROR) {
		printf("bind failed with error %d\n", WSAGetLastError());
		UninitNet();
		system("pause");
		return false;
	}

	//TODO:接收数据 阻塞函数：创建线程
	//createThread 当函数中有c++运行时库 函数开辟空间，在退出线程时不会回收空间
	//createThread和ExitThread时同时使用的，ExitThread函数是系统自动调用的
	//_beginthreadex和_endthreadex一起使用，_endthreadex函数是系统自动调用的
	//_endthreadex里面是先回收c++运行时库函数开辟的空间，然后再调用ExitThread退出线程
	handle = (HANDLE)_beginthreadex(NULL, 0,&recvthread ,this , 0, NULL);
	return true;
}

unsigned int _stdcall UDPnet::recvthread(LPVOID p) {
	UDPnet* pthis = (UDPnet*)p;
	pthis->RecvData();
	return 0;
}

//关闭网络
void UDPnet::UninitNet()
{
	//退出线程
	m_isstop = true;
	if (handle) {
		if (WaitForSingleObject(handle, 100) == WAIT_TIMEOUT) {
			TerminateThread(handle,-1);
		}
		CloseHandle(handle);
		handle = NULL;
	}
	//关闭套接字
	if (m_sock && m_sock != INVALID_SOCKET) {
		closesocket(m_sock);
	}
	WSACleanup();//卸载库

}
//接收数据
bool UDPnet::RecvData()
{
	char recvbuf[_DEF_BUFFER_SIZE]="";
	SOCKADDR_IN addrClient;
	int nsize = sizeof(addrClient);
	int nrecvNum;
	m_isstop = false;
	while (!m_isstop) {
		nrecvNum = recvfrom(m_sock, recvbuf, _DEF_BUFFER_SIZE, 0, (sockaddr*)&addrClient, &nsize);
		if (nrecvNum > 0) {
			char* pack = new char[nrecvNum];
			if (pack) {
				memcpy(pack, recvbuf, nrecvNum);
				m_pMediator->DealData(m_sock,pack, nrecvNum);
			}
		}
		else {
			printf("recvfrom error:%d\n", WSAGetLastError());
		}
	}
	return true;
}
//发送数据
bool UDPnet::SendData(long lSendIP,char* buf, int nlen)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_UDP_PORT);
	addr.sin_addr.S_un.S_addr = lSendIP;
	if (sendto(m_sock, buf, nlen, 0, (sockaddr*)&addr, sizeof(addr))<=0) {
		printf("sendto error：%d\n", WSAGetLastError());
		return false;
	}
	return true;
}



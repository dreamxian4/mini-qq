#include "TCPclientNet.h"
#include"TCPclientMediator.h"
#include"PackDef.h"
#include<process.h>


TcpClientNet::TcpClientNet() {}
TcpClientNet::~TcpClientNet() {}

TcpClientNet::TcpClientNet(INetmediator* p):m_sock(INVALID_SOCKET),handle(0),m_isstop(false){
	m_pMediator = p;
}

//初始化网络
bool TcpClientNet::InitNet()
{
	//加载库
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("WSAStartup failed with error:%d\n", err);
		system("pause");
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("Could not find a usable version of Winsock.dll\n");
		UninitNet();
		system("pause");
		return false;
	}

	//创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET) {
		printf("socket function failed with error=%d\n", WSAGetLastError());
		UninitNet();
		system("pause");
		return false;
	}

	//建立连接
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(_DEF_SERVER_IP);
	service.sin_port = htons(_DEF_TCP_PORT);
	err = connect(m_sock, (sockaddr*)&service, sizeof(service));
	if (err == SOCKET_ERROR) {
		printf("bind failed with error %d\n", WSAGetLastError());
		UninitNet();
		system("pause");
		return false;
	}

	handle = (HANDLE)_beginthreadex(NULL, 0,&recvthread ,this , 0, NULL);
	return true;
}

unsigned int _stdcall TcpClientNet::recvthread(LPVOID p) {
	TcpClientNet* pthis = (TcpClientNet*)p;
	pthis->RecvData();
	return 0;
}

//关闭网络
void TcpClientNet::UninitNet()
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
bool TcpClientNet::RecvData()
{
	int npackSize = 0;
	int nRecvNum = 0;
	int offset = 0;//偏移量

	while (!m_isstop) {
		//先接收包大小，再接收包内容
		nRecvNum = recv(m_sock, (char*)&npackSize, sizeof(int), 0);
		if (nRecvNum <= 0)break;
		char* recvBuf = new char[npackSize];
		while (npackSize ){
			nRecvNum = recv(m_sock, recvBuf + offset, npackSize, 0);
			if (nRecvNum > 0) {
				npackSize -= nRecvNum;//剩余包大小
				offset += nRecvNum;//偏移量
			}
		}
        //处理包内容
		this->m_pMediator->DealData(m_sock,recvBuf,offset);
		//TODO:recvBuf要回收
		offset = 0;
	}

	return true;
}
//发送数据
bool TcpClientNet::SendData(long lSendIP,char* buf, int nlen)
{
	//1.判断传入参数是否合法
	if (!buf || nlen <= 0)return false;

	//2.防止粘包，先发送包大小，再发送包内容
	if (send(m_sock,(char*)&nlen,sizeof(int),0) <= 0)return false;
	if (send(m_sock,buf,nlen,0) <= 0)return false;

	return true;
}



#include "TCPservernet.h"
#include "TCPserverMediator.h"
#include "PackDef.h"
#include<process.h>


TcpServerNet::TcpServerNet() {}
TcpServerNet::~TcpServerNet() {}

TcpServerNet::TcpServerNet(INetmediator* p) :m_isstop(false) {
	m_pMediator = p;
}

//初始化网络
bool TcpServerNet::InitNet()
{
	//1.选项目--加载库
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

	//2.雇人--创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET) {
		printf("socket function failed with error=%d\n", WSAGetLastError());
		UninitNet();
		system("pause");
		return false;
	}

	//绑定IP
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(_DEF_TCP_PORT);
	err = bind(m_sock, (SOCKADDR*)&service, sizeof(service));
	if (err == SOCKET_ERROR) {
		printf("bind failed with error %d\n", WSAGetLastError());
		UninitNet();
		system("pause");
		return false;
	}

	//监听
	err = listen(m_sock, 10);
	if (err == SOCKET_ERROR) {
		printf("listen failed with error %d\n", WSAGetLastError());
		UninitNet();
		system("pause");
		return false;
	}

	//接受连接
	HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &acceptthread, this, 0, NULL);
	if (handle) {
		m_hThreadHandleList.push_back(handle);
	}
	return true;
}

unsigned int _stdcall TcpServerNet::acceptthread(LPVOID p) {
	TcpServerNet* pthis = (TcpServerNet*)p;
	sockaddr_in addrClient;
	int nSize = sizeof(addrClient);
	while (!pthis->m_isstop) {
		SOCKET sock = accept(pthis->m_sock, (sockaddr*)&addrClient, &nSize);
		cout << "ip:" << inet_ntoa(addrClient.sin_addr) << endl;
		//给每个客户端创建一个对应的接受数据的线程
		unsigned int nThreadid = 0;
		HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &recvthread, pthis, 0, &nThreadid);
		if (handle) {
			pthis->m_hThreadHandleList.push_back(handle);
			pthis->m_mapThreadToSocket[nThreadid] = sock;
			nThreadid++;
		}
	}
	return 0;
}

unsigned int _stdcall TcpServerNet::recvthread(LPVOID p) {
	TcpServerNet* pthis = (TcpServerNet*)p;
	pthis->RecvData();
	return 0;
}


//关闭网络
void TcpServerNet::UninitNet()
{
	//退出线程
	m_isstop = true;
	for (auto ite = m_hThreadHandleList.begin(); ite != m_hThreadHandleList.end();) {
		if (*ite) {
			if (WaitForSingleObject(*ite, 100) == WAIT_TIMEOUT) {
				TerminateThread(*ite, -1);
			}
			CloseHandle(*ite);
			*ite = NULL;
		}
		ite = m_hThreadHandleList.erase(ite);
	}

	//关闭套接字
	if (m_sock && m_sock != INVALID_SOCKET) {
		closesocket(m_sock);
	}
	for (auto ite = m_mapThreadToSocket.begin(); ite != m_mapThreadToSocket.end(); ite++) {
		if (ite->second && ite->second != INVALID_SOCKET) {
			closesocket(ite->second);
		}
		ite = m_mapThreadToSocket.erase(ite);
	}

	WSACleanup();//卸载库

}
//接收数据
bool TcpServerNet::RecvData()
{
	//获取与客户端通信的socket
	Sleep(100);//线程创建后立即执行，但是线程id和socket存入map也许要时间执行，所以先休眠一会
	SOCKET sock=m_mapThreadToSocket[GetCurrentThreadId()];//获取当前线程ID

	//判断socket的合法性
	if (!sock || sock == INVALID_SOCKET)return false;

	//接收数据
	int npackSize = 0;//包大小
	int nRecvNum = 0;//实际接收的大小
	int offset = 0;//偏移量

	while (!m_isstop) {
		//先接收包大小，再接收包内容
		nRecvNum = recv(sock, (char*)&npackSize, sizeof(int), 0);
		if (nRecvNum <= 0)break;
		char* recvBuf = new char[npackSize];
		while (npackSize) {
			nRecvNum = recv(sock, recvBuf + offset, npackSize, 0);
			if (nRecvNum > 0) {
				npackSize -= nRecvNum;//剩余包大小
				offset += nRecvNum;//偏移量
			}
		}
		this->m_pMediator->DealData(sock,recvBuf, offset);
		//TODO:recvBuf要回收
		offset = 0;
	}

	return true;
}
//发送数据
bool TcpServerNet::SendData(long lSendIP, char* buf, int nlen)
{
	//lSendIP里面装的是SOCKET
	//1.判断传入参数是否合法
	if (!buf || nlen <= 0)return false;

	//2.防止粘包，先发送包大小，再发送包内容
	if (send(lSendIP, (char*)&nlen, sizeof(int), 0) <= 0)return false;
	if (send(lSendIP, buf, nlen, 0) <= 0)return false;

	return true;
}



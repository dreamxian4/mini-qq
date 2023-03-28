#include"TCPserverMediator.h"
#include"TCPserverNet.h"

TcpServermediator::TcpServermediator() {
	m_pNet = new TcpServerNet(this);
}
TcpServermediator::~TcpServermediator() {
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}
//打开网络
bool TcpServermediator::OpenNet() {
	if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
}
//发送数据
bool TcpServermediator::SendData(long lSendIP, char* buf, int nlen) {
	if (!m_pNet->SendData(lSendIP, buf, nlen)) {
		return false;
	}
	return true;
}
//关闭网络
void TcpServermediator::CloseNet() {
	m_pNet->UninitNet();
}
//处理数据
void TcpServermediator::DealData(long lSendIP, char* buf, int nlen) {
	//TODO:把数据发给核心处理类
	cout << buf << endl;
	SendData(lSendIP, buf, nlen);
}
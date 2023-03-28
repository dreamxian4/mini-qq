#include"TCPclientMediator.h"
#include"TCPclientNet.h"

TcpClientmediator::TcpClientmediator() {
	m_pNet = new TcpClientNet(this);
}
TcpClientmediator::~TcpClientmediator() {
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}
//打开网络
bool TcpClientmediator::OpenNet() {
	if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
}
//发送数据
bool TcpClientmediator::SendData(long lSendIP, char* buf, int nlen) {
	if (!m_pNet->SendData(lSendIP, buf, nlen)) {
		return false;
	}
	return true;
}
//关闭网络
void TcpClientmediator::CloseNet() {
	m_pNet->UninitNet();
}
//处理数据
void TcpClientmediator::DealData(long lSendIP, char* buf, int nlen) {
	//TODO:把数据发给核心处理类
    Q_EMIT SIG_ReadyData(lSendIP,  buf,  nlen);
}

#include"UDPmediator.h"
#include"UDPnet.h"

UDPmediator::UDPmediator() {
	m_pNet = new UDPnet(this);
}
UDPmediator::~UDPmediator() {
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}
//打开网络
bool UDPmediator::OpenNet() {
	if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
}
//发送数据
bool UDPmediator::SendData(long lSendIP, char* buf, int nlen) {
	if (!m_pNet->SendData(lSendIP, buf, nlen)) {
		return false;
	}
	return true;
}
//关闭网络
void UDPmediator::CloseNet() {
	m_pNet->UninitNet();
}
//处理数据
void UDPmediator::DealData(long lSendIP, char* buf, int nlen) {
	//TODO:把数据发给核心处理类
	cout << buf << endl;
}
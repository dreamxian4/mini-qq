#pragma once
#include"INetMediator.h"

class INet;
class TcpServermediator :public INetmediator {
public:
	TcpServermediator();
	~TcpServermediator();
	virtual bool OpenNet();
	virtual bool SendData(long lSendIP, char* buf, int nlen);
	virtual void CloseNet();
	virtual void DealData(long lSendIP, char* buf, int nlen);

protected:
	INet* m_pNet;
};
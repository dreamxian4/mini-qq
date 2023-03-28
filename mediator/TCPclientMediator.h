#pragma once
#include"INetMediator.h"

class INet;
class TcpClientmediator :public INetmediator {
    Q_OBJECT
signals:
    void SIG_ReadyData(long lSendIP, char* buf, int nlen);

public:
	TcpClientmediator();
	~TcpClientmediator();
	virtual bool OpenNet();
	virtual bool SendData(long lSendIP, char* buf, int nlen);
	virtual void CloseNet();
	virtual void DealData(long lSendIP, char* buf, int nlen);

protected:
	INet* m_pNet;
};

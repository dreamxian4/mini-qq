#pragma once
#include<QObject>
#include"PackDef.h"

class INet;
class INetmediator :public QObject{
    Q_OBJECT
public:
    INetmediator();
    virtual ~INetmediator();
	//打开网络
	virtual bool OpenNet() = 0;
	//发送数据
	virtual bool SendData(long lSendIP, char* buf, int nlen) = 0;
	//关闭网络
	virtual void CloseNet() = 0;
	//处理数据
	virtual void DealData(long lSendIP,char* buf, int nlen) = 0;

protected:
	INet* m_pNet;
};

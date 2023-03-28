#ifndef CKERNEL_H
#define CKERNEL_H

#include"mychatdialog.h"
#include"INetMediator.h"
#include"logindialog.h"
#include <QObject>
#include<QMap>
#include"useritem.h"
#include"chatdialog.h"

class CKernel;

//函数指针
typedef void (CKernel::*FUNC)(char* buf, int nlen);

class CKernel : public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel();
signals:

public slots:
    //处理接收到的数据
    void slot_ReadyData(long lSendIP, char* buf, int nlen);
    //处理注册请求回复
    void slot_RegisterRs(char* buf,int nLen);
    //处理登录请求回复
    void slot_LoginRs(char* buf,int nLen);
    //处理好友信息请求
    void slot_FriendInfoRq(char* buf,int nLen);
    //处理聊天请求
    void slot_ChatRq(char* buf,int nLen);
    //处理聊天回复
    void slot_ChatRs(char* buf,int nLen);
    //处理好友添加请求
    void slot_AddFriendRq(char* buf,int nLen);
    //处理好友添加回复
    void slot_AddFriendRs(char* buf,int nLen);
    //处理下线请求
    void slot_OfflineRq(char* buf,int nLen);

    //处理登录信息提交
    void slot_LoginCommit(QString tel,QString password);
    //处理注册信息提交
    void slot_RegisterCommit(QString tel,QString password,QString name);
    //处理点击用户头像
    void slot_UserItemClicked(int id);
    //处理聊天内容发送请求
    void slot_SendChatMsg(int id,QString content);
    //处理添加好友请求
    void slot_AddFriend();
    //处理登录/注册窗口关闭事件
    void slot_DestroyInstance();
    //处理好友列表窗口关闭事件
    void slot_offLine();

private:
    //初始化协议映射表
    void setProtocalMap();
    //utf-8和GB2312编码的转换
    //QString->GB2312
    static void utf8ToGB2312(char* gbBuf,int nlen,QString &utf8);
    //GB2312->QString
    static QString GB2312ToUtf8(char* gbBuf);

private:
    myChatDialog* m_mainWnd;
    INetmediator* m_tcpClientMediator;
    LoginDialog* m_loginDialog;
    //协议映射表(函数指针数组)【协议头-base】->处理函数
    FUNC m_netProtocalMap[_DEF_PROTOCAL_COUNT];
    int m_id;
    QString m_name;
    //保存useritem和id的映射关系
    QMap<int,UserItem*> m_mapIdToUserItem;
    //保存聊天窗口和id的映射关系
    QMap<int,ChatDialog*> m_mapIdToChatDialog;
};


#endif // CKERNEL_H

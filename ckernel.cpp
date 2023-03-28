#include "ckernel.h"
#include"TCPclientMediator.h"
#include<QMessageBox>
#include<QDebug>
#include<QTextCodec>
#include"useritem.h"
#include<QInputDialog>

#define NetProtocalMap(a) m_netProtocalMap[a-_DEF_PROTOCAL_BASE-10]

//初始化协议映射表
void CKernel::setProtocalMap()
{
    memset(m_netProtocalMap,0,_DEF_PROTOCAL_COUNT);
    NetProtocalMap(_DEF_PACK_REGISTER_RS)=&CKernel::slot_RegisterRs;
    NetProtocalMap(_DEF_PACK_ONLINE_RS)=&CKernel::slot_LoginRs;
    NetProtocalMap(_DEF_PACK_FRIEND_INFO)=&CKernel::slot_FriendInfoRq;
    NetProtocalMap(_DEF_PACK_CHAT_RQ)=&CKernel::slot_ChatRq;
    NetProtocalMap(_DEF_PACK_CHAT_RS)=&CKernel::slot_ChatRs;
    NetProtocalMap(_DEF_PACK_ADDFRIEND_RQ)=&CKernel::slot_AddFriendRq;
    NetProtocalMap(_DEF_PACK_ADDFRIEND_RS)=&CKernel::slot_AddFriendRs;
    NetProtocalMap(_DEF_PACK_OFFLINE_RQ)=&CKernel::slot_OfflineRq;
}
//QString->GB2312
void CKernel::utf8ToGB2312(char *gbBuf, int nlen, QString &utf8)
{
    QTextCodec* gb2312=QTextCodec::codecForName("gb2312");
    QByteArray ba=gb2312->fromUnicode(utf8);
    strcpy_s(gbBuf,nlen,ba.data());
}

//GB2312->QString
QString CKernel::GB2312ToUtf8(char *gbBuf)
{
    QTextCodec* gb2312=QTextCodec::codecForName("gb2312");
    return gb2312->toUnicode(gbBuf);
}

CKernel::CKernel(QObject *parent) : QObject(parent),m_id(5)
{
    //主窗口new对象，并显示
    m_mainWnd=new myChatDialog;
    //m_mainWnd->show();
    //绑定添加好友的信号和槽
    connect(m_mainWnd,SIGNAL(SIG_AddFriend()),
            this,SLOT(slot_AddFriend()));
    //绑定窗口关闭的信号和槽
    connect(m_mainWnd,SIGNAL(SIG_Close()),
            this,SLOT(slot_offLine()));

    //登录界面
    m_loginDialog=new LoginDialog;
    m_loginDialog->showNormal();

    //绑定处理登录信息提交的信号和槽
    connect(m_loginDialog,SIGNAL(SIG_LoginCommit(QString , QString )),
            this,SLOT(slot_LoginCommit(QString , QString )));
    //绑定处理登录信息提交的信号和槽
    connect(m_loginDialog,SIGNAL(SIG_RegisterCommit(QString , QString , QString )),
            this,SLOT(slot_RegisterCommit(QString , QString , QString )));

    //绑定处理窗口关闭事件的信号和槽
    connect(m_loginDialog,SIGNAL(SIG_Close()),
            this,SLOT(slot_DestroyInstance()));

    //中介者类对象
    m_tcpClientMediator =new TcpClientmediator;

    //绑定处理数据的信号和槽函数
    connect(m_tcpClientMediator,SIGNAL(SIG_ReadyData(long,char*,int)),
            this,SLOT(slot_ReadyData(long,char*,int)));

    if(!m_tcpClientMediator->OpenNet()){
        QMessageBox::about(m_mainWnd,"提示","打开网络失败");
        exit(0);
    }

    setProtocalMap();

    //测试代码
    //m_tcpClientMediator->SendData(0,"hello world",strlen("hello world")+1);
//    STRU_TCP_REGISTER_RQ rq;
//    m_tcpClientMediator->SendData(0,(char*)&rq,sizeof(rq));
    //添加5个在线好友，五个离线好友
//    for(int i=0;i<5;i++){
//        UserItem* item=new UserItem;
//        item->slot_setInfo(i+1,QString("用户%1").arg(i),1,i+1);
//        m_mainWnd->slot_addFriend(item);
//    }
//    for(int i=5;i<10;i++){
//        UserItem* item=new UserItem;
//        item->slot_setInfo(i+1,QString("用户%1").arg(i),0,i+1);
//        m_mainWnd->slot_addFriend(item);
//    }
}

CKernel::~CKernel(){
    //回收主窗口对象
    if(m_mainWnd){
        m_mainWnd->hide();
        delete m_mainWnd;
        m_mainWnd=NULL;
    }
    //回收中介者类对象
    if(m_tcpClientMediator){
        m_tcpClientMediator->CloseNet();
        delete m_tcpClientMediator;
        m_tcpClientMediator=NULL;
    }
}

//处理接收的数据
void CKernel::slot_ReadyData(long lSendIP, char *buf, int nlen)
{
    //1. 获取协议头
    int nType=*(int*)buf;
    //2. 判断协议头是否在映射范围内
    if(nType>=_DEF_PROTOCAL_BASE+10&&nType<=_DEF_PROTOCAL_BASE+10+_DEF_PROTOCAL_COUNT){
        //3. 计算数组下标，获取对应函数
        FUNC pf=NetProtocalMap(nType);
        //4. 如果指针存在，调用对应的处理函数
        if(pf){
            (this->*pf)(buf,nlen);
        }
    }
   //5.回收buf
    delete []buf;
}

//处理注册请求回复
void CKernel::slot_RegisterRs(char* buf,int nLen){
    //拆包
    STRU_TCP_REGISTER_RS* rs=(STRU_TCP_REGISTER_RS*)buf;
    //根据注册结果提示用户
    switch(rs->result){
    case user_is_exist:
        QMessageBox::about(m_loginDialog,"提示","注册失败，用户已存在");
        break;
    case register_success:
        QMessageBox::about(m_loginDialog,"提示","注册成功");
        break;
    default:

        break;
    }
}

//处理登录请求回复
void CKernel::slot_LoginRs(char* buf,int nLen){
    STRU_TCP_LOGIN_RS* rs=(STRU_TCP_LOGIN_RS*)buf;
    m_id=rs->userid;
    switch(rs->result){
    case user_not_exit:
        QMessageBox::about(m_loginDialog,"提示","登录失败，用户不存在");
        break;
    case password_error:
        QMessageBox::about(m_loginDialog,"提示","登录失败，密码错误");
        break;
    case login_success:
        QMessageBox::about(m_loginDialog,"提示","登录成功");
        //TODO:页面跳转：好友列表
        m_loginDialog->hide();
        m_mainWnd->showNormal();
        break;
    default:
        break;
    }
}

//处理好友信息请求
void CKernel::slot_FriendInfoRq(char *buf, int nLen)
{
    //1.拆包
    STRU_TCP_FRIEND_INFO* info=(STRU_TCP_FRIEND_INFO*)buf;
    //2.进行编码转换
    QString name=GB2312ToUtf8(info->name);
    QString feeling=GB2312ToUtf8((info->feeling));

    //3.判断是不是自己
    if(m_id==info->userid){
        //3.1 是自己，更新登录用户界面

        m_mainWnd->slot_setInfo(name,feeling,info->icon);

        //3.2 保存用户信息
        m_name=name;

        //3.3 处理完直接返回
        return;

    }else{
        //4.不是自己，查看当前窗口是否已经有这个好友了
        if(m_mapIdToUserItem.find(info->userid)==m_mapIdToUserItem.end()){
            //4.1 如果没有，把好友添加到控件上
            //4.1.1 创建一个useritem
            UserItem* item=new UserItem;

            //4.1.2 给控件赋值
            item->slot_setInfo(info->userid,name,info->state,info->icon,feeling);

            //4.1.3 绑定控件点击事件的信号和槽
            connect(item,SIGNAL(SIG_UserItemClicked(int)),
                    this,SLOT(slot_UserItemClicked(int)));

            //4.1.4 创建聊天窗口
            ChatDialog* chat=new ChatDialog;
            chat->setInfo(name,info->userid);

            //4.1.5 绑定聊天窗口发送数据的信号和槽函数
            connect(chat,SIGNAL(SIG_SendChatMsg(int , QString )),
                   this,SLOT(slot_SendChatMsg(int , QString )));

            //4.1.6 把聊天窗口放在map中管理
            m_mapIdToChatDialog[info->userid]=chat;

            //4.1.7 把useritem添加到好友列表里
            m_mainWnd->slot_addFriend(item);

            //4.1.8 把useritem放在map中管理
            m_mapIdToUserItem[info->userid]=item;
        }else{
            //4.2 如果有，更新控件信息
            UserItem* item=m_mapIdToUserItem[info->userid];
            if(item&&item->m_state==0&&info->state==1){//之前是离线，现在是上线
                //可以弹窗提示用户xxx已上线
                qDebug()<<QString("用户[%1]已上线").arg(name);
            }
            //更新控件
            item->slot_setInfo(info->userid,name,info->state,info->icon,feeling);
        }
    }
}

//处理聊天请求
void CKernel::slot_ChatRq(char *buf, int nLen)
{
    //1.拆包
    STRU_TCP_CHAT_RQ* rq=(STRU_TCP_CHAT_RQ*)buf;

    //2.查看聊天窗口是否存在
    if(m_mapIdToChatDialog.find(rq->userid)!=m_mapIdToChatDialog.end()){
        //3.如果存在，根据id查找窗口并显示
        ChatDialog* chat=m_mapIdToChatDialog[rq->userid];

        //4.设置聊天内容
        chat->slot_setChatMsg(rq->content);
    }
}

//处理聊天回复
void CKernel::slot_ChatRs(char *buf, int nLen)
{
    qDebug()<<"回复";
    //1.拆包
    STRU_TCP_CHAT_RS* rs=(STRU_TCP_CHAT_RS*)buf;

    //2.查看聊天窗口是否存在
    if(m_mapIdToChatDialog.find(rs->friendid)!=m_mapIdToChatDialog.end()){
        //3.如果存在，根据id查找窗口并显示
        ChatDialog* chat=m_mapIdToChatDialog[rs->friendid];
        //4.设置聊天内容
        chat->slot_UserOffline();
    }
}

//处理好友添加请求
void CKernel::slot_AddFriendRq(char *buf, int nLen)
{
    //1.拆包
    STRU_TCP_ADDFRIEND_RQ* rq=(STRU_TCP_ADDFRIEND_RQ*)buf;

    //2.弹窗提示用户，选择是否同意
    STRU_TCP_ADDFRIEND_RS rs;
    QString str=QString("用户[%1]想添加你为好友，是否同意？").arg(rq->username);
    if(QMessageBox::question(m_mainWnd,"添加好友",str)==QMessageBox::Yes){
        rs.result=add_success;
    }else{
        rs.result=user_refuse;
    }

    //3.构造请求结果，发给服务器
    rs.friendid=m_id;
    rs.userid=rq->userid;
    strcpy(rs.friendname,rq->friendname);
    m_tcpClientMediator->SendData(0,(char*)&rs,sizeof(rs));

    qDebug()<<"222";
}

//处理好友添加回复
void CKernel::slot_AddFriendRs(char *buf, int nLen)
{
    qDebug()<<"333";
    //1.拆包
    STRU_TCP_ADDFRIEND_RS* rs=(STRU_TCP_ADDFRIEND_RS*)buf;

    //2.好友昵称解码
    QString name= GB2312ToUtf8(rs->friendname);

    //2.根据结果提示不同信息
    switch(rs->result){
    case no_this_user:
        QMessageBox::about(m_mainWnd,"提示",QString("添加好友失败，没有该用户[%1]").arg(name));
        qDebug()<<"haha";
        break;
    case user_refuse:
        QMessageBox::about(m_mainWnd,"提示",QString("添加好友失败，用户[%1]拒绝添加好友").arg(name));
        break;
    case user_offline:
        QMessageBox::about(m_mainWnd,"提示",QString("添加好友失败，用户[%1]不在线").arg(name));
        break;
    case add_success:
        QMessageBox::about(m_mainWnd,"提示",QString("添加好友成功"));
        break;
    default:
        break;
    }
}

//处理下线请求
void CKernel::slot_OfflineRq(char *buf, int nLen)
{
    //1.拆包
    STRU_TCP_OFFLINE_RQ* rq=(STRU_TCP_OFFLINE_RQ*)buf;

    //2.根据好友id获取useritem，把好友头像设置成灰色
    if(m_mapIdToUserItem.count(rq->userid)>0){
        UserItem* item=m_mapIdToUserItem[rq->userid];
        item->slot_setOffline();
    }

}

//处理登录信息提交
void CKernel::slot_LoginCommit(QString tel, QString password)
{
    //QString转换成std::string
    std::string strTel=tel.toStdString();
    std::string strPassword=password.toStdString();

    //封包
    STRU_TCP_LOGIN_RQ rq;
    strcpy(rq.tel,strTel.c_str());
    strcpy(rq.password,strPassword.c_str());

    //发送登录请求
    m_tcpClientMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//处理注册信息提交
void CKernel::slot_RegisterCommit(QString tel, QString password, QString name)
{
    //QString转换成std::string
    std::string strTel=tel.toStdString();
    std::string strPassword=password.toStdString();
    std::string strName=name.toStdString();

    //封包
    STRU_TCP_REGISTER_RQ rq;
    strcpy(rq.tel,strTel.c_str());
    strcpy(rq.password,strPassword.c_str());
    //strcpy(rq.name,strName.c_str());
    utf8ToGB2312(rq.name,sizeof(rq.name),name);

    //发送登录请求
    m_tcpClientMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//处理点击用户头像
void CKernel::slot_UserItemClicked(int id)
{
    //显示聊天窗口
    if(m_mapIdToChatDialog.find(id)!=m_mapIdToChatDialog.end()){
        ChatDialog* chat=m_mapIdToChatDialog[id];
        chat->showNormal();
    }
}

//处理聊天内容发送请求
void CKernel::slot_SendChatMsg(int id, QString content)
{
    //1.拆包
    STRU_TCP_CHAT_RQ rq;
    rq.userid=m_id;
    rq.friendid=id;

    //2.兼容中文
    std::string strContent=content.toStdString();
    strcpy(rq.content,strContent.c_str());

    //3.聊天内容发给服务器
    m_tcpClientMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//处理添加好友请求
void CKernel::slot_AddFriend()
{
    //1.弹出窗口，输入好友的昵称
    QString strName=QInputDialog::getText(m_mainWnd,"添加好友","输入好友名字");

    //2.校验用户输入的昵称，昵称长度不能超过20
    if(strName.length()<=0||strName.length()>20){
        QMessageBox::about(m_mainWnd,"提示","输入名字不合法");
        return;
    }

    //3.校验昵称是不是自己，如果是自己，提示
    if(strName==m_name){
        QMessageBox::about(m_mainWnd,"提示","不可以添加自己为好友");
        return;
    }

    //4.校验昵称是不是已经是好友，如果已经是好友，提示
    for(auto ite=m_mapIdToUserItem.begin();ite!=m_mapIdToUserItem.end();ite++){
        UserItem* item=*ite;
        if(strName==item->m_name){
            QMessageBox::about(m_mainWnd,"提示","已经是好友啦");
            return;
        }
    }

    //5.发送添加好友请求到服务器
    STRU_TCP_ADDFRIEND_RQ rq;
    rq.userid=m_id;
    //好友的昵称需要编码，因为需要查数据库
    utf8ToGB2312(rq.friendname,sizeof(rq.friendname),strName);
    //用户名不需要编码，只用于ui显示，不涉及服务器和数据库处理
    strcpy(rq.username,m_name.toStdString().c_str());
    m_tcpClientMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//处理登录/注册窗口关闭事件
void CKernel::slot_DestroyInstance()
{
    //1.好友列表对象
    if(m_mainWnd){
        m_mainWnd->hide();
        delete m_mainWnd;
        m_mainWnd=NULL;
    }

    //2.登录/注册窗口对象
    if(m_loginDialog){
        m_loginDialog->hide();
        delete m_loginDialog;
        m_loginDialog=NULL;
    }

    //3.聊天窗口对象
    for(auto ite=m_mapIdToChatDialog.begin();ite!=m_mapIdToChatDialog.end();){
        ChatDialog* chat=*ite;
        chat->hide();
        delete chat;
        chat=NULL;
        ite=m_mapIdToChatDialog.erase(ite);
    }

    //4.中介者类对象
    if(m_tcpClientMediator){
        m_tcpClientMediator->CloseNet();
        delete m_tcpClientMediator;
        m_tcpClientMediator=NULL;
    }

    exit(0);
}

//处理好友列表窗口关闭事件
void CKernel::slot_offLine()
{
    //1.通知服务器下线了
    STRU_TCP_OFFLINE_RQ rq;
    rq.userid=m_id;
    m_tcpClientMediator->SendData(0,(char*)&rq,sizeof(rq));

    //2.回收资源
    slot_DestroyInstance();
}


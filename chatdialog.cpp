#include "chatdialog.h"
#include "ui_chatdialog.h"
#include<QTime>
#include<QDebug>

ChatDialog::ChatDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);

    //创建菜单对象，指定父控件，父控件负责子控件的回收
    m_menu=new QMenu(this);

    //添加菜单项
    m_menu->addAction("详细信息");
    m_menu->addAction("删除好友");

    //绑定菜单项的点击事件和处理函数
    connect(m_menu,SIGNAL(triggered(QAction*)),
            this,SLOT(slot_dealMenu(QAction*)));
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::setInfo(QString name,int id)
{
    m_name=name;
    m_id=id;
    //设置窗口标题：与【IP】的聊天
    setWindowTitle(QString("与[%1]的聊天").arg(m_name));
}

//在控件上显示收到的聊天内容
void ChatDialog::slot_setChatMsg(QString content)
{
    ui->tb_chat->append(QString("[%1] %2").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}

//处理聊天对象不在线
void ChatDialog::slot_UserOffline()
{
    //格式：[用户名] 时间 用户不在线
    QString str=QString("[%1] %2 用户不在线").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss"));
    ui->tb_chat->append(str);
}

void ChatDialog::on_pb_send_clicked()
{
    //1.获取输入的内容QString
    QString content=ui->te_chat->toPlainText();
    if(content.isEmpty()){
        return;
    }
    //2.清空te控件
    ui->te_chat->clear();
    //3.显示到上面tb控件
    //append换行追加
    ui->tb_chat->append(QString("[我] %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
    //4.发送数据到ckernal类
    Q_EMIT SIG_SendChatMsg(m_id,content);
}

void ChatDialog::on_pb_show_clicked()
{
    //按照鼠标点击的位置显示菜单
    QPoint p=QCursor::pos();
    //获取菜单的绝对高度
    QSize size=m_menu->sizeHint();
    m_menu->exec(QPoint(p.x(),p.y()-size.height()));
}

//点击菜单项的处理函数
void ChatDialog::slot_dealMenu(QAction *action)
{
    if(action->text()=="详细信息"){

    }else if(action->text()=="删除好友"){
        qDebug()<<"删除好友！！";
    }
}

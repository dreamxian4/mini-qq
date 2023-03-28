#include "mychatdialog.h"
#include "ui_mychatdialog.h"
#include<QIcon>
#include<QPoint>
#include<QCursor>
#include<QDebug>
#include<QMessageBox>


myChatDialog::myChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::myChatDialog)
{
    ui->setupUi(this);
    setWindowTitle("通信软件v1.0");

    //初始化QVBoxLayout
    m_layout=new QVBoxLayout;
    m_layout->setContentsMargins(0,0,0,0);  //控件上下左右的间距
    m_layout->setSpacing(3);        //设置每个控件彼此间的距离
    //设置控件的层
    ui->wdg_list->setLayout(m_layout);

    //创建菜单对象，指定父控件，父控件负责子控件的回收
    m_menu=new QMenu(this);

    //添加菜单项
    m_menu->addAction("添加好友");
    m_menu->addAction("系统设置");

    //绑定菜单项的点击事件和处理函数
    connect(m_menu,SIGNAL(triggered(QAction*)),
            this,SLOT(slot_dealMenu(QAction*)));
}

myChatDialog::~myChatDialog()
{
    delete ui;
}

//添加好友
void myChatDialog::slot_addFriend(UserItem *userItem)
{
    //往层上添加控件
    m_layout->addWidget(userItem);
}

//设置ui属性
void myChatDialog::slot_setInfo(QString name, QString feeling, int icon)
{
    ui->lb_name->setText(name);
    ui->le_feeling->setText(feeling);
    ui->pb_icon->setIcon(QIcon(QString(":/tx/%1.png").arg(icon)));
}

//重写关闭窗口事件
void myChatDialog::closeEvent(QCloseEvent *event)
{
    //1.忽略关闭事件，在kernel里面控制窗口关闭
    event->ignore();

    //2.提示窗口，确认是否关闭窗口
    if(QMessageBox::question(this,"退出提示","是否确认退出")==QMessageBox::Yes){
        //通知kernel类关闭窗口，回收资源
        Q_EMIT SIG_Close();
    }
}


void myChatDialog::on_pb_tool1_clicked()
{
    //按照鼠标点击的位置显示菜单
    QPoint p=QCursor::pos();
    //获取菜单的绝对高度
    QSize size=m_menu->sizeHint();
    m_menu->exec(QPoint(p.x(),p.y()-size.height()));
}

//点击菜单项的处理函数
void myChatDialog::slot_dealMenu(QAction *action)
{
    if(action->text()=="添加好友"){
        //发送添加好友信号给kernel
        Q_EMIT SIG_AddFriend();
        qDebug()<<"哈哈";

    }else if(action->text()=="系统设置"){
        //处理系统设置
    }
}

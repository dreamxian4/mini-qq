#include "logindialog.h"
#include "ui_logindialog.h"
#include<QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pb_clear_clicked()
{
    ui->le_tel->setText("");
    ui->le_password->setText("");
}

void LoginDialog::on_pb_commit_clicked()
{
    //1.获取控件中的数据
    QString tel=ui->le_tel->text();
    QString password=ui->le_password->text();

    //2.校验数据合法性
    //tel不能为空，不能为空格，密码不能为空
    QString telTemp=tel;
    if(tel.isEmpty() || password.isEmpty()
            || telTemp.remove(" ").isEmpty()){/*去掉空格是否为空*/
        QMessageBox::about(this,"提示","手机号或密码不能为空");
        return;
    }
    //长度限制，手机号必须是11位，密码不能超过10位
    if(tel.length() != 11 || password.length() > 10){
        QMessageBox::about(this,"提示","手机号必须为11位，密码不能超过10位");
        return;
    }
    //内容校验：手机号只能是数字，密码只能是数字、字母、下划线（需要使用正则表达式，暂时不做）

    //3.把数据发给kernel类
    Q_EMIT SIG_LoginCommit( tel, password);
}

void LoginDialog::on_pb_clear_register_clicked()
{
    ui->le_tel_register->setText("");
    ui->le_name_register->setText("");
    ui->le_password_register->setText("");
}

void LoginDialog::on_pb_commit_register_clicked()
{
    //1.获取控件中的数据
    QString tel=ui->le_tel_register->text();
    QString password=ui->le_password_register->text();
    QString name=ui->le_name_register->text();

    //2.校验数据合法性
    //tel不能为空，不能为空格，密码不能为空
    QString telTemp=tel;
    QString nameTemp=name;
    if(tel.isEmpty() || password.isEmpty() || name.isEmpty()
            || telTemp.remove(" ").isEmpty() || nameTemp.remove(" ").isEmpty()){/*去掉空格是否为空*/
        QMessageBox::about(this,"提示","手机号或密码或昵称不能为空");
        return;
    }
    //长度限制，手机号必须是11位，密码不能超过10位
    if(tel.length() != 11 || password.length() > 10 || name.length() > 20){
        QMessageBox::about(this,"提示","手机号必须为11位，密码不能超过10位，昵称长度不能超过20位");
        return;
    }
    //内容校验：手机号只能是数字，密码只能是数字、字母、下划线（需要使用正则表达式，暂时不做）

    //3.把数据发给kernel类
    Q_EMIT SIG_RegisterCommit( tel, password, name);
}

//重写关闭窗口事件
void LoginDialog::closeEvent(QCloseEvent *event)
{
    //1.接收关闭时间
    event->accept();

    //2.给kernel发信号，通知kernel窗口关闭
    Q_EMIT SIG_Close();
}

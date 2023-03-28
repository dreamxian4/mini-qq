#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include<QCloseEvent>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

signals:
    //登录提交
    void SIG_LoginCommit(QString tel,QString password);
    //注册提交
    void SIG_RegisterCommit(QString tel,QString password,QString name);
    //关闭窗口信号
    void SIG_Close();

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_pb_clear_clicked();

    void on_pb_commit_clicked();

    void on_pb_clear_register_clicked();

    void on_pb_commit_register_clicked();

    //重写关闭窗口事件
    void closeEvent(QCloseEvent* event);


private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H

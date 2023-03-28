#ifndef MYCHATDIALOG_H
#define MYCHATDIALOG_H

#include <QDialog>
#include<QVBoxLayout>
#include"useritem.h"
#include<QMenu>
#include<QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class myChatDialog; }
QT_END_NAMESPACE

class myChatDialog : public QDialog
{
    Q_OBJECT

signals:
    //添加好友的信号
    void SIG_AddFriend();
    //关闭窗口信号
    void SIG_Close();

public:
    myChatDialog(QWidget *parent = nullptr);
    ~myChatDialog();
    //添加好友
    void slot_addFriend(UserItem* userItem);
    //设置ui属性
    void slot_setInfo(QString name,QString feeling,int icon);
    //重写关闭窗口事件
    void closeEvent(QCloseEvent * event);

private slots:
    void on_pb_tool1_clicked();
    //点击菜单项的处理函数
    void slot_dealMenu(QAction* action);

private:
    Ui::myChatDialog *ui;
    //QVBoxLayout是一个垂直布局的层
    QVBoxLayout* m_layout;
    QMenu* m_menu;
};
#endif // MYCHATDIALOG_H

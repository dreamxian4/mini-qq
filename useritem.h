#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>
#include<QMenu>

namespace Ui {
class UserItem;
}

class UserItem : public QWidget
{
    Q_OBJECT

public:
    explicit UserItem(QWidget *parent = nullptr);
    ~UserItem();
    //设置用户信息
    void slot_setInfo(int id,QString name,int state,int iconid=1,QString feeling="比较懒，什么也没写");
    //设置用户下线
    void slot_setOffline();

signals:
    void SIG_UserItemClicked(int id);

private slots:
    void on_pb_icon_clicked();

private:
    Ui::UserItem *ui;
    int m_id;       //用户id
    int m_iconid;   //用户头像
    QString m_feeling;//用户签名
public:
    int m_state;     //用户状态
    QString m_name; //昵称
};

#endif // USERITEM_H

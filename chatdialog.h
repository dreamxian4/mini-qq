#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>
#include <QMenu>
#include <QPoint>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QWidget
{
    Q_OBJECT

signals:

    void SIG_SendChatMsg(int id,QString content);

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    //设置窗口信息
    void setInfo(QString name,int id);
    //在控件上显示收到的消息内容
    void slot_setChatMsg(QString content);
    //处理聊天对象不在线
    void slot_UserOffline();
    //处理详细信息菜单
    void slot_dealMenu(QAction *action);

private slots:
    void on_pb_send_clicked();
    void on_pb_show_clicked();


private:
    Ui::ChatDialog *ui;
    QString m_name;
    int m_id;
    QMenu* m_menu;
};

#endif // CHATDIALOG_H

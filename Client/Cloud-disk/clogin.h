#ifndef CLOGIN_H
#define CLOGIN_H

#include <QWidget>
#include<QString>
#include"Packdef.h"
namespace Ui {
class CLogin;
}

class CLogin : public QWidget
{
    Q_OBJECT

public:
    explicit CLogin(QWidget *parent = 0);
    ~CLogin();

signals:
    void SIG_Register_Login_commit(QString,QString);
    void SIG_Login_commit(QString , QString);

public:
    void is_autoLogin();
    void flush_CheckCode();     //刷新验证码
    void keep_Info();


private slots:
    void on_pb_checkcode_clicked();     //点击刷新验证码



    void on_pb_login_clicked();         //登录提交
    void on_pb_commit_clicked();        //注册提交

private:
    Ui::CLogin *ui;
};

#endif // CLOGIN_H

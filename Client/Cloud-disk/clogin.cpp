#include "clogin.h"
#include "ui_clogin.h"

CLogin::CLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CLogin)
{
    ui->setupUi(this);

    flush_CheckCode();
    LPTSTR  lpPath = new wchar_t[INI_MAX_PATH_SIZE];
    MultiByteToWideChar(CP_ACP,0,INI_PATH,-1,lpPath,INI_MAX_PATH_SIZE);
    LPTSTR number = new wchar_t[INI_NUMBER_SIZE];
    LPTSTR password = new wchar_t[INI_PASSWORD_SIZE];
    GetPrivateProfileString(L"1", L"number", L"", number, INI_NUMBER_SIZE, lpPath);
    GetPrivateProfileString(L"1", L"password", L"", password, INI_PASSWORD_SIZE, lpPath);
    ui->le_nunber->setText(QString::fromWCharArray(number));
    ui->le_password->setText(QString::fromWCharArray(password));
    delete number;
    delete password;
    if(GetPrivateProfileInt(L"1",L"rb_keepwd",0,lpPath))
    {
        ui->rb_keeppw->setChecked(true);
    }
    if(GetPrivateProfileInt(L"1",L"rb_autologin",0,lpPath))
    {
        ui->rb_autologin->setChecked(true);
    }
}

CLogin::~CLogin()
{
    delete ui;
}

//刷新随机数
#include <QTime>
void CLogin::flush_CheckCode()
{
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()*1000+time.second());
    int n = qrand() % 10000;
    ui->pb_checkcode->setText(QString::number(n));


}

void CLogin::keep_Info()
{
    LPTSTR  lpPath = new wchar_t[INI_MAX_PATH_SIZE];
    MultiByteToWideChar(CP_ACP,0,INI_PATH,-1,lpPath,INI_MAX_PATH_SIZE);

    if(ui->rb_keeppw->isChecked()) //保存密码信息
    {

        WritePrivateProfileString(L"1", L"number", ui->le_nunber->text().toStdWString().c_str(), lpPath);
        WritePrivateProfileString(L"1", L"password", ui->le_password->text().toStdWString().c_str(), lpPath);

    }
    //保存按钮信息
    if(ui->rb_keeppw->isChecked())
        WritePrivateProfileString(L"1", L"rb_keepwd", L"1", lpPath);
    else
        WritePrivateProfileString(L"1", L"rb_keepwd", L"0", lpPath);
    if(ui->rb_autologin->isChecked())
        WritePrivateProfileString(L"1", L"rb_autologin", L"1", lpPath);
    else
        WritePrivateProfileString(L"1", L"rb_autologin", L"0", lpPath);

}

void CLogin::on_pb_checkcode_clicked()
{
    flush_CheckCode();
}

#include<QMessageBox>
#include<QDebug>
void CLogin::on_pb_login_clicked()
{
    QString name = ui->le_nunber->text();
    QString password = ui->le_password->text();
    if(name.isEmpty() || name.remove("").isEmpty() || password.isEmpty())
    {
        QMessageBox::about(this,"提示","用户名或密码不能为空");
        return;
    }
    QRegExp reg("[0-9a-zZ-A-Z]{1,10}");
    bool res = reg.exactMatch(name);
    if(!res)
    {
        QMessageBox::about(this,"提示","用户名非法");
        return;
    }
    if(name.length()>10 || password.length()>10)
    {
        QMessageBox::about(this,"提示","用户名或密码过长");
        return;
    }

//    if(ui->le_checkcode->text() != ui->pb_checkcode->text())
//    {
//        QMessageBox::about(this,"提示","验证码错误");
//        flush_CheckCode();
//        return;
//    }

    Q_EMIT SIG_Login_commit(name , password);
}



void CLogin::on_pb_commit_clicked()
{
    QString name = ui->le_nunber->text();
    QString password = ui->le_password->text();
    if(name.isEmpty() || name.remove("").isEmpty() || password.isEmpty() )
    {
        QMessageBox::about(this,"提示","用户名或密码不能为空");
        return;
    }
    QRegExp reg("[0-9a-zZ-A-Z]{1,10}");
    bool res = reg.exactMatch(name);
    if(!res)
    {
        QMessageBox::about(this,"提示","用户名非法");
        return;
    }
    if(name.length()>10 || password.length()>10)
    {
        QMessageBox::about(this,"提示","用户名或密码过长");
        return;
    }

    Q_EMIT SIG_Register_Login_commit(name , password);
}
void CLogin::is_autoLogin()
{
    LPTSTR  lpPath = new wchar_t[INI_MAX_PATH_SIZE];
    MultiByteToWideChar(CP_ACP,0,INI_PATH,-1,lpPath,INI_MAX_PATH_SIZE);
    if(GetPrivateProfileInt(L"1",L"rb_autologin",0,lpPath))
    {
        Q_EMIT SIG_Login_commit(ui->le_nunber->text() , ui->le_password->text());
    }
}

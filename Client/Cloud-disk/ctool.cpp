#include "ctool.h"
#include "ui_ctool.h"

CTool::CTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CTool)
{
    ui->setupUi(this);
}

CTool::~CTool()
{
    delete ui;
}

void CTool::clear()
{
    ui->le_filename->setText("");
}
#include<QMessageBox>
void CTool::on_pb_ok_clicked()
{
    if(ui->le_filename->text() == "")
           return;
    if(!illegal_check(ui->le_filename->text()))
    {
        QMessageBox::about(this,"提示","文件名中不能包含'.'或':'");
        return;
    }
    this->hide();
    Q_EMIT  SIG_OK(ui->le_filename->text());
    clear();
}

void CTool::on_pushButton_2_clicked()
{
    this->hide();
}

bool CTool::illegal_check(QString str)
{
    if(str.startsWith('.')||str.startsWith(':'))
    {
        return false;
    }
    return true;
}

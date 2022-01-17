#include "onlinedialog.h"
#include "ui_onlinedialog.h"

OnlineDialog::OnlineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OnlineDialog)
{
    ui->setupUi(this);
}

OnlineDialog::~OnlineDialog()
{
    delete ui;
}

void OnlineDialog::on_pb_ok_clicked()
{
    QString rtmp = ui->le_rtmp->text();
    Q_EMIT SIG_SEND_RTMP(rtmp);
}

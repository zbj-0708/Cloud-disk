#ifndef ONLINEDIALOG_H
#define ONLINEDIALOG_H

#include <QDialog>

namespace Ui {
class OnlineDialog;
}

class OnlineDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_SEND_RTMP(QString);
public:
    explicit OnlineDialog(QWidget *parent = 0);
    ~OnlineDialog();

private slots:
    void on_pb_ok_clicked();

private:
    Ui::OnlineDialog *ui;
};

#endif // ONLINEDIALOG_H

#ifndef CTOOL_H
#define CTOOL_H

#include <QWidget>

namespace Ui {
class CTool;
}

class CTool : public QWidget
{
    Q_OBJECT
signals:
    void SIG_OK(QString);
//    void SIG_CREATE_GROUP(QString);
//    void SIG_ADD_GROUP(QString);
public:
    explicit CTool(QWidget *parent = 0);
    ~CTool();
    void clear();
private slots:
    void on_pb_ok_clicked();

    void on_pushButton_2_clicked();
    bool illegal_check(QString str);

private:
    Ui::CTool *ui;
};

#endif // CTOOL_H

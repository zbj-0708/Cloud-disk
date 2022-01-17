#ifndef CGROUPITEM_H
#define CGROUPITEM_H

#include <QWidget>
#include"cfileitem.h"
namespace Ui {
class cgroupitem;
}

class cgroupitem : public QWidget
{
    Q_OBJECT

public:
    explicit cgroupitem(QWidget *parent = 0);
    ~cgroupitem();
public:
    void setgroupname(QString name);
    void addFile(QString filename);
private:
    Ui::cgroupitem *ui;
    QString m_groupname;
};

#endif // CGROUPITEM_H

#ifndef CFILEITEM_H
#define CFILEITEM_H

#include <QWidget>
#include"Packdef.h"


namespace Ui {
class CFileItem;
}

class CFileItem : public QWidget
{
    Q_OBJECT

public:
    explicit CFileItem(QWidget *parent = 0);
    ~CFileItem();

signals:
    void SIG_CLICK_EVENT(QString ,QString);
    void SIG_DOUBLE_CLICK_EVENT(QString );
public:
    void Info(QString filename, int type , int type2, QString filepath="");
    QString getfilename();
    void setfilename(QString filename);
public slots:
    void mouseDoubleClickEvent(QMouseEvent *event);     //文件下载   文件夹打开
    void mousePressEvent(QMouseEvent *event);
private:
    Ui::CFileItem *ui;
public:
    QString m_filename;
    QString m_filepath;
    int m_type;

};

#endif // CFILEITEM_H

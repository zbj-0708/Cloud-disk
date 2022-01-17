#ifndef CPROGRESSBAR_H
#define CPROGRESSBAR_H

#include <QWidget>
#include<semaphore.h>
#include<qDebug>
#include<QProcess>
#include<Packdef.h>
namespace Ui {
class CProgressBar;
}

class CProgressBar : public QWidget
{
    Q_OBJECT

public:
    explicit CProgressBar(QWidget *parent = 0);
    ~CProgressBar();
signals:
    void SIG_CHANGE_STATE(int);
    void SIG_CLOSE();
    void SIG_PRORESS_FINISH();
public slots:

public:
    void InfoBar(QString &filename , int beginpos, int filesize ,int number , int type);        //初始化
    void setProgressBar(double size);           //设置进度条显示
    void setTotalSize(int size);                //设置文件总长度显示
    void setNetworkSpeed(int speed);            //设置上传下载速度显示
    void setFinshiState(QString finishtime );       //上传下载完成后切换状态
public slots:
    void on_pb_static_clicked();        //暂停、播放
    void on_pb_cancel_clicked();        //取消

    void on_pb_cancel_2_clicked();  //完成后取消


    void on_pb_openfile2_clicked();     //打开文件目录

public:
    Ui::CProgressBar *ui;
    double m_current_size;
    double m_total_size;
    int m_state;
    QString m_filename;
    QString m_cloudname;
    //pthread_mutex_t m_mutex;
    sem_t m_sem;
    int m_close;
    int m_number;
    int m_type;
    int m_semCount;
    int m_is_finish;
    QString m_path;         //本地
    QString m_filepath;     //云盘
    QProcess process;
    QString m_finishData;
};

#endif // CPROGRESSBAR_H

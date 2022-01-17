#include "cprogressbar.h"
#include "ui_cprogressbar.h"

CProgressBar::CProgressBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CProgressBar)
{
    ui->setupUi(this);
    m_state;

}

CProgressBar::~CProgressBar()
{
    qDebug()<<"关闭进度条";
    delete ui;
    //pthread_mutex_destroy(&m_mutex);
    sem_destroy (&m_sem);

}

void CProgressBar::setProgressBar(double size )
{
    m_current_size += size;
    int process = m_current_size/m_total_size*100;
    //qDebug()<<(int)m_current_size<<"   "<<(int)m_total_size;
    if(m_current_size == m_total_size)
    {
        process = 100;
        m_is_finish = 1;
        Q_EMIT SIG_PRORESS_FINISH();
    }
    if(!m_close)
        ui->progress_bar->setValue(process);
}
void CProgressBar::setTotalSize(int size)
{
    m_total_size = size;
    QString str;
    if(size>=1024*1024)
    {
        size /= 1024;
        size /= 1024;
        str = QString::number(size);
        str += "Mb";
        qDebug()<<"MB";
    }
    else if(size >= 1024)
    {
        size /=1024;
        str = QString::number(size);
        str += "Kb";
        qDebug()<<"KB";
    }
    else
    {
        str = QString::number(size);
        str += "b";
        qDebug()<<"bB";

    }
    ui->lb_fileSize->setText(str);
}

void CProgressBar::InfoBar(QString &filename, int beginpos, int filesize, int number,int type)
{
    m_type = type;
    m_close = 0;
    m_number = number;
    m_state = 1;
    m_total_size = filesize;
    m_filename = filename;
    m_current_size = beginpos;
    m_is_finish = 0;
    sem_init (&m_sem, 0, 1);
    m_semCount = 1;
    ui->lb_filename->setText(filename);
    setTotalSize(filesize);
    ui->progress_bar->setValue(m_current_size*100/m_total_size);
    ui->pb_static->setIcon(QIcon(QString(":/picture/wait.webp")));
}

void CProgressBar::setNetworkSpeed(int speed)
{
    QString str = QString::number(speed);
    if(speed >= 1024)
    {
        speed /= 1024;
        str = QString::number(speed);
        str += "Mb/s";
    }
    else
    {
        str = QString::number(speed);
        str += "Kb/s";
    }
    ui->lb_networkspeed->setText(str);
}
#include<QDateTime>
void CProgressBar::setFinshiState( QString finishtime)
{
    m_finishData = finishtime;

    ui->lb_filename_2->setText(m_filename);
    ui->lb_fileSize_2->setText( QString::number((int)m_total_size));
    //QDateTime curDateTime=QDateTime::currentDateTime();
    ui->lb_finish_time->setText(/*curDateTime.toString("yyyy-MM-dd hh:mm:ss")*/finishtime);
    if(m_type== progress_up || m_type==progress_up_finish)
    {
        ui->lb_finish_type->setText( "上传完成" );
        m_type = progress_up_finish;
    }
    if(m_type == progress_down || m_type==progress_down_finish)
    {
        ui->lb_finish_type->setText( "下载完成" );
        m_type = progress_down_finish;
    }
    ui->stackedWidget->setCurrentIndex(1);
}
#include<qDebug>
void CProgressBar::on_pb_static_clicked()
{
    /*
     * //ui
    #define progress_up 0
    #define progress_down 1
    #define progress_finish 2*/
    if(m_state)
    {
        //pthread_mutex_lock(&m_mutex);

        int n = m_semCount;

        while(n-- && !m_type)
            sem_wait(&m_sem);
        m_state = 0;
        ui->pb_static->setIcon(QIcon(QString(":/picture/continue.webp")));

    }
    else
    {
        m_state = 1;
        //pthread_mutex_unlock(&m_mutex);
        int n=m_semCount;
        while(n-- && !m_type)
            sem_post(&m_sem);
        ui->pb_static->setIcon(QIcon(QString(":/picture/wait.webp")));
    }
    if(m_type)
        Q_EMIT SIG_CHANGE_STATE(m_state);
}
#include<QMessageBox>
void CProgressBar::on_pb_cancel_clicked()
{

//    if(ui->progress_bar->value() == 100)
//    {
//        this->close();
//        delete this;
//        return;
//    }
    if( QMessageBox::Yes == QMessageBox::question(this,"提示","您确定取消传输此次任务？"))
    {
        if(!m_state)
            while(!m_type && m_semCount--)
            sem_post(&m_sem);
            //pthread_mutex_unlock(&m_mutex);
        if(m_state && m_type)
            on_pb_static_clicked();
        m_close = 1;
        Q_EMIT SIG_CLOSE();
    }

}

void CProgressBar::on_pb_cancel_2_clicked()
{
    Q_EMIT SIG_CLOSE();
}
#include <QDesktopServices>
#include <QUrl>

void CProgressBar::on_pb_openfile2_clicked()
{
    m_path.replace("/", "\\"); // 只能识别 "\"
    QString path = m_path+"\\"+m_filename;
    qDebug()<<path;
    QString cmd = QString("explorer.exe /select,\"%1\"").arg(path);
    process.startDetached(cmd);

}

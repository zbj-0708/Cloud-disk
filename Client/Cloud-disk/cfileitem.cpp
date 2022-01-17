#include "cfileitem.h"
#include "ui_cfileitem.h"

CFileItem::CFileItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFileItem)
{
    ui->setupUi(this);
}

CFileItem::~CFileItem()
{
    delete ui;
}

void CFileItem::Info(QString filename, int type,int type2, QString filepath)
{
    ui->lb_filename->setText(filename);
    m_filename = filename;
    m_filepath = filepath;
    m_type = type2;
    switch(type)
    {
        case TYPE_FILE:
            ui->pb_fileicon->setIcon(QIcon(QString(":/picture/file.webp")));
            break;
        case TYPE_FOLDER:
            ui->pb_fileicon->setIcon(QIcon(QString(":/picture/folder.webp")));
            break;
        default:
            break;
    }
}

QString CFileItem::getfilename()
{
    return ui->lb_filename->text();
}

void CFileItem::setfilename(QString filename)
{
    ui->lb_filename->setText(filename);
}

void CFileItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_EMIT SIG_DOUBLE_CLICK_EVENT(m_filename);
}

void CFileItem::mousePressEvent(QMouseEvent *event)
{
    Q_EMIT SIG_CLICK_EVENT(m_filename,m_filepath);
}

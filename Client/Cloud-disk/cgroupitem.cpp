#include "cgroupitem.h"
#include "ui_cgroupitem.h"

cgroupitem::cgroupitem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cgroupitem)
{
    ui->setupUi(this);

    //文件的显示列
    ui->gridLayout->setColumnStretch(0,1);
    ui->gridLayout->setColumnStretch(1,1);
    ui->gridLayout->setColumnStretch(2,1);
    ui->gridLayout->setColumnStretch(3,1);
    ui->gridLayout->setColumnStretch(4,1);
    ui->gridLayout->setColumnStretch(5,1);

    //文件的显示行
    ui->gridLayout->setRowStretch(0,1);
    ui->gridLayout->setRowStretch(1,1);
    ui->gridLayout->setRowStretch(2,1);
    ui->gridLayout->setRowStretch(3,1);
}

cgroupitem::~cgroupitem()
{
    delete ui;
}

void cgroupitem::setgroupname(QString name)
{
//    ui->->setText(name);
//    m_groupname = name;
}

void cgroupitem::addFile(QString filename)
{
//    CFileItem *item = new CFileItem;
//    if(filename.contains(".",Qt::CaseSensitive))
//    {
//        item->Info(filename,TYPE_FILE);
//    }
//    else
//    {
//        item->Info(filename,TYPE_FOLDER);
//    }

//    connect(item,SIGNAL(SIG_CLICK_EVENT( QString)),this,SLOT(slot_deal_click_event(QString)));
//    connect(item,SIGNAL(SIG_DOUBLE_CLICK_EVENT(QString)),this,SLOT(slot_deal_double_click_event(QString)));

//    m_mapFilenameToFileitem[filename] = item;
}

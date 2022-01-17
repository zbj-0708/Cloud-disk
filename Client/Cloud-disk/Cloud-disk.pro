#-------------------------------------------------
#
# Project created by QtCreator 2021-07-24T13:01:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(./netapi/netapi.pri)
include(./sqlapi/sqlapi.pri)

TARGET = Cloud-disk
TEMPLATE = app

INCLUDEPATH += ./netapi/
INCLUDEPATH += ./sqlapi/

INCLUDEPATH += $$PWD/ffmpeg-4.2.2/include \
               $$PWD/SDL2-2.0.10/include

SOURCES += main.cpp\
    clogin.cpp \
    ckernel.cpp \
    cclouddisk.cpp \
    ctool.cpp \
    cfileitem.cpp \
    IMToolBox.cpp \
    cprogressbar.cpp \
    Thread_pool.cpp \
    cgroupitem.cpp  \
    videoshow.cpp \
    showplayer.cpp \
    videoitem.cpp \
    packetqueue.cpp \
    onlinedialog.cpp \
    videoslider.cpp




HEADERS  += \
    clogin.h \
    ckernel.h \
    cclouddisk.h \
    ctool.h \
    cfileitem.h \
    IMToolBox.h \
    cprogressbar.h \
    Thread_pool.h \
    cgroupitem.h  \
    videoshow.h \
    showplayer.h \
    videoitem.h \
    packetqueue.h \
    onlinedialog.h \
    videoslider.h


FORMS    += \
    clogin.ui \
    cclouddisk.ui \
    ctool.ui \
    cfileitem.ui \
    cprogressbar.ui \
    cgroupitem.ui \
    videoshow.ui \
    videoitem.ui \
    onlinedialog.ui

LIBS += $$PWD/ffmpeg-4.2.2/lib/avcodec.lib\
     $$PWD/ffmpeg-4.2.2/lib/avdevice.lib\
     $$PWD/ffmpeg-4.2.2/lib/avfilter.lib\
    $$PWD/ffmpeg-4.2.2/lib/avformat.lib\
    $$PWD/ffmpeg-4.2.2/lib/avutil.lib\
    $$PWD/ffmpeg-4.2.2/lib/postproc.lib\
    $$PWD/ffmpeg-4.2.2/lib/swresample.lib\
    $$PWD/ffmpeg-4.2.2/lib/swscale.lib\
    $$PWD/SDL2-2.0.10/lib/x86/SDL2.lib

RESOURCES += \
    src.qrc \
    source.qrc





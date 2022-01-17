TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH +=./include

LIBS += -lpthread
LIBS += -lmysqlclient


LIBS += -L/usr/local/lib/ -lhiredis \


SOURCES += \
    src/Mysql.cpp \
    src/TCPKernel.cpp \
    src/TCPNet.cpp \
    src/Thread_pool.cpp \
    src/err_str.cpp \
    src/main.cpp \
    src/redis.cpp  \

HEADERS += \
    include/Mysql.h \
    include/TCPKernel.h \
    include/TCPNet.h \
    include/Thread_pool.h \
    include/err_str.h \
    include/packdef.h \
    include/redis.h \

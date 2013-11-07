#-------------------------------------------------
#
# Project created by QtCreator 2013-09-29T21:59:58
#
#-------------------------------------------------

QT += core widgets gui network

TARGET = Qirc
TEMPLATE = app


SOURCES += main.cpp \
    model/IrcChannel.cpp \
    model/IrcMessage.cpp \
    model/IrcSocket.cpp \
    model/IrcUser.cpp \
    view/IrcMainWindow.cpp \
    view/IrcConnectionWindow.cpp \
    view/QircDialogWindow.cpp \
    IrcController.cpp \

HEADERS += \
    model/IrcChannel.h \
    model/IrcMessage.h \
    model/IrcSocket.h \
    model/IrcUser.h \
    model/IrcSessionTree.h \
    view/IrcMainWindow.h \
    view/IrcConnectionWindow.h \
    view/QircDialogWindow.h \
    IrcController.h \

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chat.cpp \
    ../common/config.cpp \
    file.cpp \
    friend.cpp \
    index.cpp \
    main.cpp \
    client.cpp \
    msghandle.cpp \
    onlineuser.cpp \
    ../common/protocol.cpp \
    ../common/packet.cpp \
    ../common/password.cpp \
    ../common/pathsafety.cpp \
    ../common/messagedispatcher.cpp \
    sharefile.cpp \
    uploader.cpp

HEADERS += \
    chat.h \
    ../common/config.h \
    client.h \
    file.h \
    friend.h \
    index.h \
    msghandle.h \
    onlineuser.h \
    ../common/protocol.h \
    ../common/packet.h \
    ../common/password.h \
    ../common/pathsafety.h \
    ../common/messagedispatcher.h \
    sharefile.h \
    uploader.h

FORMS += \
    chat.ui \
    client.ui \
    file.ui \
    friend.ui \
    index.ui \
    onlineuser.ui \
    sharefile.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    config.qrc \
    icon.qrc

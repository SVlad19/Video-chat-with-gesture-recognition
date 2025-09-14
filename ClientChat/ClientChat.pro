QT       += core gui network multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatitemwidget.cpp \
    clientmanager.cpp \
    connectionwidget.cpp \
    filemanager.cpp \
    main.cpp \
    mainwindow.cpp \
    ../Utility/protocol.cpp \
    progressdialogwidget.cpp

HEADERS += \
    chatitemwidget.h \
    clientmanager.h \
    connectionwidget.h \
    filemanager.h \
    mainwindow.h \
    ../Utility/protocol.h \
    progressdialogwidget.h

FORMS += \
    chatitemwidget.ui \
    connectionwidget.ui \
    mainwindow.ui \
    progressdialogwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

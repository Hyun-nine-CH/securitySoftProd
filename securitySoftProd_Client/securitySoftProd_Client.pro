QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    admininfoform.cpp \
    admininfoform_prod.cpp \
    clientinfoform.cpp \
    clientinfoform_chat.cpp \
    clientinfoform_prod.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow_admin.cpp

HEADERS += \
    admininfoform.h \
    admininfoform_prod.h \
    clientinfoform.h \
    clientinfoform_chat.h \
    clientinfoform_prod.h \
    mainwindow.h \
    mainwindow_admin.h

FORMS += \
    admininfoform.ui \
    admininfoform_prod.ui \
    clientinfoform.ui \
    clientinfoform_chat.ui \
    clientinfoform_prod.ui \
    mainwindow.ui \
    mainwindow_admin.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ImagesTool.qrc

#-------------------------------------------------
#
# Project created by QtCreator 2019-11-07T19:54:18
#
#-------------------------------------------------

QT       += core gui sql charts serialport network datavisualization qml quick

QMAKE_LFLAGS += /NODEFAULTLIB:LIBCMTD

RC_ICONS = icon\serial.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = SerialMaster
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -luser32

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        db_ctrl.cpp \
        form_about.cpp \
        form_setting.cpp \
        main.cpp \
        mainwindow.cpp \
        qaesencryption.cpp \
        qcustomplot.cpp

HEADERS += \
        config.h \
        db_ctrl.h \
        form_about.h \
        form_setting.h \
        mainwindow.h \
        qaesencryption.h \
        qcustomplot.h

FORMS += \
        form_about.ui \
        form_setting.ui \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    source.qrc

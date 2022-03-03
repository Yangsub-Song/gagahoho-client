QT       += core gui network serialport multimedia dbus widgets

CONFIG += c++11 debug
TARGET = gagahoho-client

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
    hal/linux/linux.cpp \
    main.cpp \
    mainwindow.cpp \
    hal/audio/audiomanager.cpp \
    util/database.cpp \
    util/dateutil.cpp \
    hal/env/envmonitor.cpp \
    hal/rf/rfmodule.cpp \
    util/filedownloader.cpp \
    util/smsreceiver.cpp \
    hal/gpio/gpio.cpp \
    calendar/calendarview.cpp \
    calendar/cell.cpp \
    calendar/header.cpp \
    time/timemanager.cpp \
    scrollinglabel.cpp \
    broadcastdisplay.cpp \
    groupdisplay.cpp \
    timedisplay.cpp \
    clientapp.cpp 
    
HEADERS += \
    mainwindow.h \
    hal/audio/audiomanager.h \
    util/calendar/category.h \
    util/calendar/date.h \
    util/calendar/event.h \
    util/database.h \
    util/dateutil.h \
    hal/env/envmonitor.h \
    util/filedownloader.h \
    hal/rf/rfmodule.h \
    hal/rf/rfpacket.h \
    hal/linux/linux.h \
    util/smsreceiver.h \
    hal/gpio/gpio.h \
    calendar/calendarview.h \
    calendar/cell.h \
    calendar/header.h \
    time/timemanager.h \
    scrollinglabel.h \
    broadcastdisplay.h \
    groupdisplay.h \
    timedisplay.h \
    clientapp.h 

FORMS += \
    mainwindow.ui \
    broadcastdisplay.ui \
    groupdisplay.ui \
    timedisplay.ui \
    clientapp.ui 
target.path = /usr/bin
INSTALLS += target

RESOURCES += \
    resouces.qrc

QMAKE_LIBS += -lstdc++fs -lsqlite3
QMAKE_CXXFLAGS_DEBUG += -O0
QMAKE_CFLAGS_DEBUG += -O0

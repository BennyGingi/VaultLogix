TEMPLATE = app
CONFIG += c++17

# Qt modules
QT += core gui widgets charts axcontainer printsupport

# Project files
SOURCES += \
    audittrail.cpp \
    helpdialog.cpp \
    itemmanager.cpp \
    loginwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    statistics.cpp \
    usermanager.cpp

HEADERS += \
    audittrail.h \
    helpdialog.h \
    itemmanager.h \
    loginwindow.h \
    mainwindow.h \
    settings.h \
    statistics.h \
    usermanager.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

CONFIG += warn_on
CONFIG += release

# Include path
INCLUDEPATH += ./src

# Definitions
DEFINES += MY_DEFINE_VAR

# Library and linker settings
unix:!macx: LIBS += -lpthread

# Post-build step
unix {
    QMAKE_POST_LINK += cp -f $$OUT_PWD/$$TARGET /some/destination/path/$$TARGET
}

# Target installation path
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialog.cpp \
    dockwidget.cpp \
    hints.cpp \
    main.cpp \
    mainwindow.cpp \
    minetextedit.cpp \
    result.cpp \
    savedialog.cpp

HEADERS += \
    dialog.h \
    dockwidget.h \
    hints.h \
    mainwindow.h \
    minetextedit.h \
    result.h \
    savedialog.h

FORMS += \
    dialog.ui \
    dockwidget.ui \
    hints.ui \
    mainwindow.ui \
    result.ui \
    savedialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

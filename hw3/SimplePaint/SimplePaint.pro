#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T00:56:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimplePaint
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    masksizedialog.cpp \
    maskdialog.cpp \
    gmaskdialog.cpp \
    umaskdialog.cpp \
    imshowdialog.cpp \
    mhedgedialog.cpp \
    smaskdialog.cpp \
    fuzzysetsdialog.cpp \
    thresdialog.cpp

HEADERS  += mainwindow.h \
    masksizedialog.h \
    maskdialog.h \
    gmaskdialog.h \
    umaskdialog.h \
    imshowdialog.h \
    mhedgedialog.h \
    smaskdialog.h \
    fuzzysetsdialog.h \
    thresdialog.h

FORMS    += mainwindow.ui \
    masksizedialog.ui \
    maskdialog.ui \
    gmaskdialog.ui \
    umaskdialog.ui \
    imshowdialog.ui \
    mhedgedialog.ui \
    smaskdialog.ui \
    fuzzysetsdialog.ui \
    thresdialog.ui

# User defined settings
QT       += charts
CONFIG += c++11

# For windows
win32{
    INCLUDEPATH += $$(OPENCV_DIR)/../../include
    DEPENDPATH += $$(OPENCV_DIR)/../../include

    CONFIG(debug, debug|release) {       # For debug
        LIBS += $$(OPENCV_DIR)/lib/opencv_core310d.lib
        LIBS += $$(OPENCV_DIR)/lib/opencv_highgui310d.lib
        LIBS += $$(OPENCV_DIR)/lib/opencv_imgproc310d.lib
        LIBS += $$(OPENCV_DIR)/lib/opencv_imgcodecs310d.lib
    } else {                             # For release
        LIBS += $$(OPENCV_DIR)/lib/opencv_core310.lib
        LIBS += $$(OPENCV_DIR)/lib/opencv_highgui310.lib
        LIBS += $$(OPENCV_DIR)/lib/opencv_imgproc310.lib
        LIBS += $$(OPENCV_DIR)/lib/opencv_imgcodecs310.lib
    }
}

# For linux
unix{
    INCLUDEPATH += /usr/local/include/opencv
    LIBS += `pkg-config opencv --libs`
}

RESOURCES += \
    myres.qrc

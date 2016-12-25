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
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

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
unix:!macx{
    INCLUDEPATH += /usr/local/include/opencv
    LIBS += `pkg-config opencv --libs`
}

# For macOS
macx{
    INCLUDEPATH += /usr/local/include
    LIBS += `/usr/local/bin/pkg-config opencv --libs`
}

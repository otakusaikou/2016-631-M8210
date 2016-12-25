#-------------------------------------------------
#
# Project created by QtCreator 2016-12-08T21:31:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ColImgProcessor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    utils.cpp \
    fcsettingdialog.cpp \
    kmeansdialog.cpp

HEADERS  += mainwindow.h \
    utils.h \
    fcsettingdialog.h \
    kmeansdialog.h

FORMS    += mainwindow.ui \
    fcsettingdialog.ui \
    kmeansdialog.ui

# User defined settings
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

RESOURCES += \
    myres.qrc

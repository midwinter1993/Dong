######################################################################
# Automatically generated by qmake (2.01a) Tue Sep 2 22:16:54 2014
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += data_plot.h main_window.h pixmaps.h settingWindow.h clear.xpm
SOURCES += data_plot.cpp main.cpp main_window.cpp settingWindow.cpp
RESOURCES += curve.qrc resource.qrc source.qrc


INCLUDEPATH += .INCLUDEPATH += /usr/local/qwt-5.2.1/include
#LIBS += -L"/usr/local/qwt-5.2.1/lib/" -lqwt
LIBS += -L"/usr/local/qwt-5.2.1/lib/" -lqwt
QT += network


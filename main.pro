TEMPLATE = app

TARGET = test

QT += widgets
QT +=printsupport
CONFIG += debug
# CONFIG += release

#OBJECTS_DIR = build
#MOC_DIR = build

macx{
    CONFIG -= app_bundle
}
LIBS += -larmadillo
INCLUDEPATH +=./src
HEADERS += src/gui/mainwindow.h \
           src/util/add.h \
           src/parse/parse.h \
           src/circuit/circuit.h \
           src/devices/two_port_device.h \
           src/devices/four_port_device.h \
           src/devices/pulse_device.h \
           src/solver/solver.h \
           src/util/tool.h \
           src/plotter/plotter.h \
           src/plotter/qcustomplot.h

SOURCES += src/main.cpp \
           src/gui/mainwindow.cpp \
           src/util/add.cpp \
           src/parse/parse.cpp \
           src/circuit/circuit.cpp \
           src/devices/two_port_device.cpp \
           src/devices/four_port_device.cpp \
           src/devices/pulse_device.cpp \
           src/solver/solver.cpp \
           src/util/tool.cpp \
           src/plotter/plotter.cpp \
           src/plotter/qcustomplot.cpp


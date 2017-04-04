#-------------------------------------------------
#
# Project created by QtCreator 2016-09-21T01:38:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtqt
TEMPLATE = app

SOURCES += provided/main.cpp\
    provided/client.cpp \
    provided/window361.cpp \
    provided/renderarea361.cpp \
    test/assignment1/common.cpp \
    lib/src/light/color.cpp \
    lib/src/gutils.cpp \
    lib/src/polygons/convexfiller.cpp \
    lib/src/polygons/convexwirer.cpp \
    lib/src/polygons/polygon.cpp \
    lib/src/parse/ifreader.cpp \
    lib/src/parse/objreader.cpp \
    lib/src/parse/simpreader.cpp \
    lib/src/models/transformation.cpp \
    lib/src/lines/antialias.cpp \
    lib/src/lines/bresenham.cpp \
    lib/src/lines/ddanalyzer.cpp \
    lib/src/lines/line.cpp \
    lib/src/camera.cpp \
    lib/src/shaper.cpp \
    lib/src/light/light.cpp \
    lib/src/models/model.cpp

HEADERS  += \
    provided/drawable.h \
    provided/pageturner.h \
    provided/client.h \
    provided/window361.h \
    provided/renderarea361.h \
    test/panel.h \
    test/tester.h \
    test/shared/polygontest.h \
    test/assignment1/a1test.h \
    test/assignment1/common.h \
    test/assignment1/parallelogram.h \
    test/assignment1/randomtest.h \
    test/assignment1/starburst.h \
    test/assignment2/a2test.h \
    test/assignment2/triangletest.h \
    test/assignment3/a3test.h \
    lib/include/light/color.h \
    lib/include/gutils.h \
    lib/include/shaper.h \
    lib/include/polygons/convexfiller.h \
    lib/include/polygons/convexwirer.h \
    lib/include/polygons/polygon.h \
    lib/include/parse/ifreader.h \
    lib/include/parse/objreader.h \
    lib/include/parse/simpreader.h \
    lib/include/lines/antialias.h \
    lib/include/lines/bresenham.h \
    lib/include/lines/ddanalyzer.h \
    lib/include/lines/line.h \
    lib/include/models/matrix.h \
    lib/include/models/model.h \
    lib/include/models/transformation.h \
    lib/include/camera.h \
    lib/include/light/light.h

DISTFILES += \
    lib/src/models/matrix.ipp \



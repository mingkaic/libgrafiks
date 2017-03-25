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
    lib/src/lines/line.cpp

HEADERS  += \
    provided/drawable.h \
    provided/pageturner.h \
    provided/client.h \
    provided/window361.h \
    provided/renderarea361.h \
    test/panel.h \
    test/tester.h \
    test/shared/polygontest.h \
    test/assignment2/a2test.h \
    test/assignment2/triangletest.h \
    test/assignment1/a1test.h \
    test/assignment1/common.h \
    test/assignment1/parallelogram.h \
    test/assignment1/randomtest.h \
    test/assignment1/starburst.h \
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
    test/assignment3/a3test.h

DISTFILES += \
    lib/src/models/matrix.ipp \
    test/simp/circleleft.simp \
    test/simp/circleright.simp \
    test/simp/equitriangle.simp \
    test/simp/page4.simp \
    test/simp/page5.simp \
    test/simp/z0_face.simp



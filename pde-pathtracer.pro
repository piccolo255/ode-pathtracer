#-------------------------------------------------
#
# Project created by QtCreator 2017-04-27T02:35:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pde-pathtracer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
   main.cpp \
   plot_window.cpp \
   runge_kutta_stepper.cpp \
   render_view.cpp \
   simulation_loop.cpp

HEADERS  += \
   plot_window.hpp \
   runge_kutta_stepper.hpp \
   render_view.hpp \
   simulation_loop.hpp \
    ode_pathtracer.hpp

FORMS    += plot_window.ui

# include muParser
# TODO: rewrite this to no longer be system-specific :/

compiling {
   win32:CONFIG(release, debug|release): LIBS += -LE:/Coding/libraries/qt-libs/muparser-2.2.5/lib/ -lmuparser
   else:win32:CONFIG(debug, debug|release): LIBS += -LE:/Coding/libraries/qt-libs/muparser-2.2.5/lib/ -lmuparserd

   QMAKE_CXXFLAGS += -isystem E:/Coding/libraries/qt-libs/muparser-2.2.5/include
   DEPENDPATH += E:/Coding/libraries/qt-libs/muparser-2.2.5/include

   win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += E:/Coding/libraries/qt-libs/muparser-2.2.5/lib/libmuparser.a
   else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += E:/Coding/libraries/qt-libs/muparser-2.2.5/lib/libmuparserd.a
   else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += E:/Coding/libraries/qt-libs/muparser-2.2.5/lib/muparser.lib
   else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += E:/Coding/libraries/qt-libs/muparser-2.2.5/lib/muparserd.lib
}

INCLUDEPATH += E:/Coding/libraries/qt-libs/muparser-2.2.5/include

## include Boost 
## NO LONGER USED

##INCLUDEPATH += E:/Coding/libraries/boost_1_55_0

## This variable is set as "CONFIG += compiling"
## The assignment is done in qmake command line argument
#compiling {
#   # This block takes effect during real compilation
#   QMAKE_CXXFLAGS += -isystem E:/Coding/libraries/boost_1_55_0
#}

#INCLUDEPATH += E:/Coding/libraries/boost_1_55_0

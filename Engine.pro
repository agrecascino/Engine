TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

QMAKE_CXXFLAGS += -I/usr/include/bullet -I/usr/include/freetype2 -std=c++14

LIBS +=  -lGLEW -lGLU -lGL -lLinearMath -lglfw -lBulletCollision -lBulletDynamics -lbmpread -lfreetype

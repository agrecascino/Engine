TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

QMAKE_CXXFLAGS += -pthread -I/usr/include/bullet -I/usr/include/freetype2 -std=c++14 -O2

LIBS +=  -lalut -lopenal -lopenmpt -lpthread -lGLEW -lGLU -lGL -lLinearMath -lglfw -lBulletCollision -lBulletDynamics -lbmpread -lfreetype

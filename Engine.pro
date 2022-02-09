TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    deps/bmpread.c \
    deps/libfont.cpp \
    video/schemagui.cpp \
    video/renderer.cpp \
    game/camera.cpp \
    game/physcube.cpp \
    game/polyobject.cpp \
    game/sky.cpp \
    core/entityman.cpp \
    core/collisionman.cpp \
    deps/audiohack.cpp

QMAKE_CXXFLAGS += -fcommon -pthread -I/usr/include/bullet -I/usr/include/freetype2 -std=c++14 -O0 -g

LIBS +=  -lstdc++fs -lalut -lopenal -lopenmpt -lpthread -lGLU -lGL -lLinearMath -lglfw -lBulletCollision -lBulletDynamics -lfreetype

HEADERS += \
    deps/dr_wav.h \
    deps/rapidxml.hpp \
    deps/rapidxml_iterators.hpp \
    deps/rapidxml_print.hpp \
    deps/rapidxml_utils.hpp \
    deps/bmpread.h \
    deps/utilityfunc.h \
    audio/media.h \
    core/entity.h \
    game/physcube.h \
    game/sky.h \
    game/polyobject.h \
    core/collisionman.h \
    core/entityman.h \
    game/camera.h \
    core/assetman.h \
    deps/libfont.h \
    video/renderer.h \
    video/schemagui.h \
    core/resourcetypes.h

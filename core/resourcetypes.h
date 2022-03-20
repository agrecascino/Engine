#ifndef RESOURCETYPES_H
#define RESOURCETYPES_H
#include <string>
#include <vector>
#include "deps/bmpread.h"
#include <libopenmpt/libopenmpt.h>
#include <libopenmpt/libopenmpt_ext.h>
#include "deps/dr_wav.h"
#include <memory>
#include "core/entity.h"
#include <glm/vec4.hpp>

#define DEFAULT_MAINWINDOW_COLOR glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
#define DEFAULT_SUBWINDOW_COLOR glm::vec4(0.4f, 0.4f, 0.4f, 0.85f)

enum MediaType {
    MODULE,
    WAVE,
    UNLOADED
};

enum TextAlignment {
    CENTER,
    LEFT,
    RIGHT
};

struct Text {
    Text(float x, float y, float scl) : x(x), y(y), scl(scl) {}
    Text() {}
    float x = 0.0f;
    float y = 0.0f;
    float scl = 1.0f;
    bool hidden;
    std::string data;
    std::string action;
    TextAlignment align = CENTER;
};

struct Window {
    Window *parent;
    float x = 0.0f;
    float y = 0.0f;
    float xsz = 0.0f;
    float ysz = 0.0f;
    glm::vec4 color;
    bool hidden;
    std::vector<Window*> subwindows;
    std::vector<Text> text;
};

struct LoadedSchema {
    Window *main;
};

struct MediaFile {
    MediaFile(void) { type = UNLOADED; }

    MediaFile(openmpt_module *mod) {
        type = MODULE;
        repmod = mod;
    }

    MediaFile(drwav loaded) {
        type = MODULE;
        repwav = loaded;
    }

    MediaType type;
    openmpt_module *repmod;
    drwav repwav;
};

enum ResourceType {
    MAP,
    TEXTURE,
    SOUND,
    SCHEMA,
    INCOMPLETE
};

struct LoadedTexture {
    bmpread_t loaded;
};


struct LoadedMap {
    std::vector<std::shared_ptr<CollidableEntity>> collidables;
    std::vector<std::shared_ptr<RenderedEntity>> renderables;
    std::vector<std::shared_ptr<Entity>> entities;
};

struct Resource {
    Resource(void) { type = INCOMPLETE; }

    Resource(LoadedMap m) { type = MAP; maprep = m; }

    Resource(LoadedTexture t) { type = TEXTURE; texturerep = t; }

    Resource(LoadedSchema s) { type = SCHEMA; schemarep = s; }

    Resource(MediaFile f) { type = SOUND; mediarep = f; }

    LoadedMap maprep;
    LoadedTexture texturerep;
    LoadedSchema schemarep;
    MediaFile mediarep;
    ResourceType type;
    std::string filename;
    bool newres = true;
    int users = 0;
    int generation = 0;
};

#endif // RESOURCETYPES_H

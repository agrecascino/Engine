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

enum MediaType {
    MODULE,
    WAVE,
    UNLOADED
};

struct Text {
    Text(float x, float y, float scl) : x(x), y(y), scl(scl) {}
    Text() {}
    float x = 0.0f;
    float y = 0.0f;
    float scl = 1.0f;
    std::string data;
    std::string action;
};

struct LoadedSchema {
    std::vector<Text> text;
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

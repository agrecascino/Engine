#ifndef ASSETMAN_H
#define ASSETMAN_H
#define DR_WAV_IMPLEMENTATION
#include "deps/dr_wav.h"
#include "audio/media.h"
#include "deps/bmpread.h"
#include "video/schemagui.h"
#include "entity.h"
#include "deps/rapidxml.hpp"
#include "deps/rapidxml_utils.hpp"
#include "game/polyobject.h"
#include <iostream>
#include "game/sky.h"
#include <experimental/filesystem>

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

    Resource(MediaFile f) { type = SOUND; f = f; }

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

class AssetManager {
public:
    unsigned long loadTextureFromFile(std::string filename) {
        std::string uri = "textures/" + filename;
        LoadedTexture t;

        for(auto &kv : assets) {
            if(kv.second->filename == filename)
                return kv.first;
        }
        if (!std::experimental::filesystem::exists(uri))
            return 0;

        if(!bmpread(uri.c_str(), 0, &(t.loaded)))
            return 0;

        std::shared_ptr<Resource> newtex = std::make_shared<Resource>(t);
        newtex->filename = filename;
        assets[id] = newtex;
        return id++;
    }

    unsigned long loadSoundFromFile(std::string filename) {
        std::string uri = "audio/" + filename;
        MediaFile f;

        for(auto &kv : assets) {
            if(kv.second->filename == filename)
                return kv.first;
        }
        if (!std::experimental::filesystem::exists(uri))
            return 0;

        std::ifstream file(filename, std::ios::binary);
        size_t bytecount = openmpt_probe_file_header_get_recommended_size();
        std::vector<uint8_t> filestart(bytecount);
        file.read((char*)filestart.data(), bytecount);
        if(openmpt_probe_file_header(OPENMPT_PROBE_FILE_HEADER_FLAGS_DEFAULT, &filestart,
                                     bytecount, NULL, NULL, NULL, NULL, NULL, NULL, NULL) ==
           OPENMPT_PROBE_FILE_HEADER_RESULT_SUCCESS) {
            file.seekg(0, std::ios::end);
            size_t filesize = file.tellg();
            filestart.resize(filesize);
            file.seekg(0);
            file.read((char*)filestart.data(), filesize);
            f.type = MODULE;
            f.repmod = openmpt_module_create_from_memory2(filestart.data(), filesize, NULL, NULL,
                                               NULL, NULL, NULL, NULL, NULL);
            if(!f.repmod)
                return 0;
        } else {
            if(!drwav_init_file(&f.repwav, uri.c_str(), NULL))
                return 0;
            f.type = WAVE;
        }

        std::shared_ptr<Resource> newsnd = std::make_shared<Resource>(f);
        newsnd->filename = filename;
        assets[id] = newsnd;
        return id++;
    }

    unsigned long loadSchemaFromFile(std::string filename) {
        std::string uri = "schemas/" + filename;
        LoadedSchema s;

        for(auto &kv : assets) {
            if(kv.second->filename == filename)
                return kv.first;
        }
        if (!std::experimental::filesystem::exists(uri))
            return 0;

        s = loadSchemaXML(uri);
        std::shared_ptr<Resource> newschema = std::make_shared<Resource>(s);
        newschema->filename = filename;
        assets[id] = newschema;
        return id++;
    }

    unsigned long loadMapFromFile(std::string filename) {
        std::string uri = "maps/" + filename;
        LoadedMap m;

        for(auto &kv : assets) {
            if(kv.second->filename == filename)
                return kv.first;
        }
        if (!std::experimental::filesystem::exists(uri))
            return 0;
        loadMapFromXML(uri.c_str(), m);
        std::shared_ptr<Resource> newmap = std::make_shared<Resource>(m);
        newmap->filename = filename;
        assets[id] = newmap;
        return id++;
    }

    std::shared_ptr<Resource> getResource(unsigned long id) {
        if(assets.find(id) != assets.end()) {
            if(assets[id]->newres)
                assets[id]->newres = false;
            assets[id]->users++;
            return assets[id];
        }
        return NULL;
    }

    void returnResource(unsigned long id) {
        if(assets.find(id) != assets.end()) {
            assert(!assets[id]->newres);
            assert(assets[id]->users > 0);
            assets[id]->users--;
        }
    }

    void gc() {
        for(auto &kv : assets) {
            if(kv.second->users)
                continue;
            if(!kv.second->newres)
                kv.second->generation++;
            if(kv.second->generation > 10)
                assets.erase(kv.first);
        }
    }

private:

    LoadedSchema loadSchemaXML(std::string filename) {
        LoadedSchema s;
        rapidxml::file<> xmlfile(filename.c_str());
        rapidxml::xml_document<> document;
        document.parse<0>(xmlfile.data());
        rapidxml::xml_node<> *currentnode = (document.first_node());
        if(currentnode->name() == "schema") {
            currentnode = currentnode->first_node();
            while (!currentnode) {
                std::cout << currentnode->name() << std::endl;
                std::cout << strlen(currentnode->name()) << std::endl;
                if (strcmp(currentnode->name(),"text") == 0) {
                    Text t;
                    rapidxml::xml_attribute<> *attr = currentnode->first_attribute();
                    while(!attr) {
                        if(attr->name() == "x") {
                            t.x = std::stof(attr->value());
                        } else if(attr->name() == "y") {
                            t.y = std::stof(attr->value());
                        } else if(attr->name() == "scl") {
                            t.scl = std::stof(attr->value());
                        } else if(attr->name() == "action") {
                            t.action = attr->value();
                        }
                    }
                    t.data = currentnode->value();
                    s.text.push_back(t);
                }
                currentnode = currentnode->next_sibling();
            }
        }
        return s;
    }

    void loadEngineInline(const char* name, LoadedMap &map) {
        if (strcmp(name,"sky") == 0) {
            std::shared_ptr<RenderedEntity> sky;
            RenderedEntity *skyptr = new Sky();
            sky.reset(skyptr);
            map.renderables.push_back(sky);
        }
    }

    void loadMapFromXML(const char* filename, LoadedMap &map) {
        rapidxml::file<> xmlfile(filename);
        rapidxml::xml_document<> document;
        document.parse<0>(xmlfile.data());
        rapidxml::xml_node<> *currentnode = (document.first_node());
        while (currentnode != NULL) {
            if (strcmp(currentnode->name(), "engineinline") == 0) {
                loadEngineInline(currentnode->value(), map);
            }
            if (strcmp(currentnode->name(), "song") == 0) {
                //FILE *file = fopen(currentnode->value(), "rb");
                //if (file != NULL) {
                    //openmpt_module *mod = openmpt_module_create(openmpt_stream_get_file_callbacks(), file, NULL, NULL, NULL);
                    //audio.playModule(mod);
                //}
            }

            if (strcmp(currentnode->name(), "mapgeometry") == 0) {
                rapidxml::xml_node<> *verticenode = currentnode->first_node();
                std::vector<glm::vec3> vertices;
                std::vector<glm::vec3> colors;
                std::vector<GLuint> textures;
                bool textured = false;
                bool quad = false;
                while (verticenode != NULL) {
                    if (strcmp(verticenode->name(), "quad") == 0) {
                        quad = true;
                    }
                    if (strcmp(verticenode->name(), "texture") == 0) {
                        bmpread_t bitmap;
                        GLuint loaded_texture;
                        if (!bmpread(verticenode->value(), 0, &bitmap)) {
                            std::cout << "failed to load texture: " << verticenode->value() << std::endl;
                            assert(false);
                        }
                        glGenTextures(1, &loaded_texture);
                        glBindTexture(GL_TEXTURE_2D, loaded_texture);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                        glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap.width, bitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap.data);
                        bmpread_free(&bitmap);
                        textures.push_back(loaded_texture);
                        textured = true;
                    }
                    if (strcmp(verticenode->name(), "vertice") == 0) {
                        char *vcopy = new char[verticenode->value_size() + 1];
                        memset(vcopy, 0, verticenode->value_size() + 1);
                        strncpy(vcopy,verticenode->value(), verticenode->value_size() + 1);
                        char *ptr = strtok(vcopy, ",");
                        std::vector<float> values;
                        while (ptr != NULL) {
                            values.push_back(strtof(ptr,NULL));
                            ptr = strtok(NULL, ",");
                        }
                        assert(values.size() == 3);
                        vertices.push_back(glm::vec3(values[0],values[1],values[2]));
                    }
                    else if (strcmp(verticenode->name(), "color") == 0) {
                        char *vcopy = new char[verticenode->value_size() + 1];
                        memset(vcopy, 0, verticenode->value_size() + 1);
                        strncpy(vcopy, verticenode->value(), strlen(verticenode->value()));
                        char *ptr = strtok(vcopy, ",");
                        std::vector<float> values;
                        while (ptr != NULL) {
                            values.push_back(strtof(ptr, NULL));
                            ptr = strtok(NULL, ",");
                        }
                        assert(values.size() == 3);
                        colors.push_back(glm::vec3(values[0], values[1], values[2]));
                    }
                    verticenode = verticenode->next_sibling();
                }
                PolyObject *p = new PolyObject(glm::vec3(0.0f, 0.0f, 0.0f));
                p->setGeometry(vertices, quad);
                p->setColor(colors, true);
                if (textured) {
                    p->setTextures(textures);
                }
                std::shared_ptr<CollidableEntity> ptr;
                ptr.reset(p);
                map.collidables.push_back(ptr);
            }
            currentnode = currentnode->next_sibling();
        }
    }

    unsigned long id = 1;

    std::map<unsigned long, std::shared_ptr<Resource>> assets;

};

#endif // ASSETMAN_H

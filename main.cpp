/* Fixes compile errors related to include ordering. */
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS 1
#include "core/entityman.h"
#include "core/assetman.h"
#include "video/renderer.h"
#include "audio/media.h"
#include "deps/rapidxml.hpp"
#include "deps/rapidxml_utils.hpp"

class Engine {
    friend class glRenderer;
public:
    Engine() : video(manager, audio, assets) {}

    void loadMapEntities(std::shared_ptr<Resource> &r) {
        manager.clearAllObjects();
        LoadedMap &m = r->maprep;
        for(std::shared_ptr<CollidableEntity> &c : m.collidables)
            manager.addEntity(c);
        for(std::shared_ptr<RenderedEntity> &c : m.renderables)
            manager.addEntity(c);
        for(std::shared_ptr<Entity> &c : m.entities)
            manager.addEntity(c);
    }

    void runStartupScript() {
        rapidxml::file<> xmlfile("startup.xml");
        rapidxml::xml_document<> document;
        document.parse<0>(xmlfile.data());
        rapidxml::xml_node<> *currentnode = (document.first_node());
        while (currentnode != NULL) {
            std::cout << currentnode->name() << std::endl;
            std::cout << strlen(currentnode->name()) << std::endl;
            if (strcmp(currentnode->name(),"loadmap") == 0) {
                unsigned long mapid = assets.loadMapFromFile(currentnode->value());
                std::shared_ptr<Resource> r = assets.getResource(mapid);
                loadMapEntities(r);
            }
            if (strcmp(currentnode->name(),"loadschema") == 0) {
                unsigned long guiid = assets.loadSchemaFromFile(currentnode->value());
                std::shared_ptr<Resource> r = assets.getResource(guiid);
                std::string name = std::string(currentnode->value()).substr(0, strlen(currentnode->value()) - 4);
                video.gui.addNewSchema(name, r->schemarep);
                video.gui.activateSchema(name);
                assets.returnResource(guiid);
            }
            currentnode = currentnode->next_sibling();
        }
        video.spawnGhostEntity();
        video.gui.takeControl();
    }

    void gameLoop() {
        this->runStartupScript();
        while (true) {
            if (video.handleInput() == -1) {
                break;
            }
            video.drawScreen();
        }
    }

private:
    AssetManager assets;
    EntityManager manager;
    glRenderer video;
    MediaStreamer audio;
};

int main()
{
    Engine game;
    game.gameLoop();
    return 0;
}


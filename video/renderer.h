#ifndef RENDERER_H
#define RENDERER_H
#include <GLFW/glfw3.h>
#include "core/entityman.h"
#include "audio/media.h"
#include "video/schemagui.h"

class glRenderer {
    friend class EntityManager;
public:
    glRenderer(EntityManager &manager, MediaStreamer &audio);

    void spawnPlayerEntity();

    void spawnGhostEntity();

    void drawScreen();

    int handleInput();

    ~glRenderer();
private:
    unsigned xscr, yscr;
    unsigned long cameraid = 0;
    double dt = 0.0;
    double lastphysrun = 0.0;
    double oldtime = 0.0;
    double newtime = 0.0;
    MediaStreamer &audio;
    EntityManager &manager;
    GLFWwindow *window = NULL;
    schemaGUI gui;
};

#endif // RENDERER_H

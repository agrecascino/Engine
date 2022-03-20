#include "renderer.h"
#include "game/camera.h"

glRenderer::glRenderer(EntityManager &manager, MediaStreamer &audio, AssetManager &assets) : audio(audio), manager(manager), assets(assets) {
    glfwInit();
    xscr = 1280;
    yscr = 720;
    window = glfwCreateWindow(xscr, yscr, "Cultural Enrichment", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPos(window, xscr/2, yscr/2);
    glfwSwapInterval(1);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    oldtime = glfwGetTime();
    newtime = glfwGetTime();
    lastphysrun = glfwGetTime();
    gui.updateScreenResolution(xscr, yscr);
}

void glRenderer::spawnPlayerEntity() {
    AbstractCameraEntity* p = (AbstractCameraEntity*)manager.getCollidableEntity(cameraid);
    if(p)
        manager.collision.removeCollidableEntity(cameraid);
    std::shared_ptr<CollidableEntity> camera;
    camera = std::make_shared<PlayerEntity>(manager.collision.bt_world, audio, manager, assets);
    camera->setPosition(glm::vec3(3,20,3));
    cameraid = manager.addEntity(camera);
}

void glRenderer::spawnGhostEntity() {
    AbstractCameraEntity* p = (AbstractCameraEntity*)manager.getCollidableEntity(cameraid);
    if(p)
        manager.collision.removeCollidableEntity(cameraid);
    std::shared_ptr<CollidableEntity> camera;
    camera = std::make_shared<GhostEntity>(glm::vec3(40,20,-15), glm::vec3(3,3,3));
    cameraid = manager.addEntity(camera);
}

void glRenderer::drawScreen() {
    manager.drawAllObjects();
    gui.drawGUI();
    gui.handleControl(window);
    glFlush();
    glfwSwapBuffers(window);
}

int glRenderer::handleInput() {
    newtime = glfwGetTime();
    //std::cout << (newtime - oldtime) << std::endl;
    dt += ((double)newtime - (double)oldtime);
    AbstractCameraEntity* p = nullptr;
    if(dt > 0.016) {
        manager.collision.stepSimulation(dt);
        p = (AbstractCameraEntity*)manager.getCollidableEntity(cameraid);
        lastphysrun = glfwGetTime();
        glfwPollEvents();
        if(gui.inControl()) {
            goto gui_running;
        }
        if(p && !(p->getAttribute("type").repstring == "GhostEntity"))
            p->updateCamera(window, dt);
        else if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            spawnPlayerEntity();
            gui.activateSchema("");
        } else if(!p) {
            spawnGhostEntity();
            gui.activateSchema("death");
        }
        gui_running:
        dt = 0;
    }
    p = (AbstractCameraEntity*)manager.getCollidableEntity(cameraid);
    if(p)
        p->loadCameraMatrices();
    if (glfwWindowShouldClose(window) || glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return -1;
    }
    oldtime = glfwGetTime();
    return 0;

}

glRenderer::~glRenderer() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

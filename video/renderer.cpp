#include "renderer.h"
#include "game/camera.h"

glRenderer::glRenderer(EntityManager &manager, MediaStreamer &audio) : audio(audio), manager(manager) {
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
    LoadedSchema l;
    for (float x = 0.1; x < 0.9001f; x += 0.1f) {
        for(float y = 0.1; y < 0.9001f; y += 0.1f) {
            Text t(x, y, 1.0);
            t.data = "(" + std::to_string(x).substr(0,3) + ", " + std::to_string(y).substr(0,3) + ")";
            l.text.push_back(t);
        }
    }
    spawnPlayerEntity();
    gui.addNewSchema("piss", l);
    gui.activateSchema("piss");
    Text t2(0.5, 0.95f, 1.0f);
    t2.data = "Your corpse appears to be missing. Respawn?";
    LoadedSchema death;
    death.text.push_back(t2);
    gui.addNewSchema("death", death);
    gui.activateSchema("");
}

void glRenderer::spawnPlayerEntity() {
    AbstractCameraEntity* p = (AbstractCameraEntity*)manager.getCollidableEntity(cameraid);
    if(p)
        manager.collision.removeCollidableEntity(cameraid);
    std::shared_ptr<CollidableEntity> camera;
    camera = std::make_shared<PlayerEntity>(manager.collision.bt_world, audio, manager);
    camera->setPosition(glm::vec3(3,20,3));
    cameraid = manager.addEntity(camera);
}

void glRenderer::spawnGhostEntity() {
    AbstractCameraEntity* p = (AbstractCameraEntity*)manager.getCollidableEntity(cameraid);
    if(p)
        manager.collision.removeCollidableEntity(cameraid);
    std::shared_ptr<CollidableEntity> camera;
    camera = std::make_shared<GhostEntity>(glm::vec3(18,10,-6), glm::vec3(3,3,3));
    camera->setPosition(glm::vec3(9,40,9));
    cameraid = manager.addEntity(camera);
}

void glRenderer::drawScreen() {
    manager.drawAllObjects();
    gui.drawGUI();
    glFlush();
    glfwSwapBuffers(window);
}

int glRenderer::handleInput() {
    newtime = glfwGetTime();
    //std::cout << (newtime - oldtime) << std::endl;
    dt += ((double)newtime - (double)oldtime);
    AbstractCameraEntity* p = nullptr;
    if(dt > 0.016) {
        double phystime = newtime - lastphysrun;
        //std::cout << (phystime * 1000) << std::endl;
        manager.collision.stepSimulation(dt);
        p = (AbstractCameraEntity*)manager.getCollidableEntity(cameraid);
        lastphysrun = glfwGetTime();
        glfwPollEvents();
        if(p && !(p->getAttribute("type").repstring == "GhostEntity"))
            p->updateCamera(window, dt);
        else if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            spawnPlayerEntity();
            gui.activateSchema("");
        } else {
            spawnGhostEntity();
            gui.activateSchema("death");
        }
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

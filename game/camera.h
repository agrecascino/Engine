#ifndef CAMERA_H
#define CAMERA_H

#include "core/entity.h"
#include "core/entityman.h"
#include "audio/media.h"
#include "core/collisionman.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class AbstractCameraEntity : public CollidableEntity {
public:
    virtual void updateCamera(GLFWwindow *windowptr, double dt) {}
    virtual void loadCameraMatrices() {}
protected:
    glm::mat4 viewmatrix;
};

class PlayerEntity : public AbstractCameraEntity {
public:
    PlayerEntity(btDynamicsWorld *bt_world, MediaStreamer &audio, EntityManager &manager);

    virtual void updateCamera(GLFWwindow *windowptr, double dt);

    void loadCameraMatrices();

private:
    EntityManager &manager;
    int16_t* pSampleData;
    MediaStreamer &audio;
    drwav pWav;
    bool cap_on = false;
    int boxctr = 0;
    unsigned long captured_obj = 0;
    int framectr = 0;
    int cctr = 0;
    bool jumpEnable = true;
    bool cubeEnable = true;
    bool no = false;
    float speed = 20.0f;
    float horizontal = 3.14f;
    float vertical = 0.0f;
    float mspeed = 0.05f;
    btDynamicsWorld *current_world;
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    glm::mat4 projectionmatrix = glm::perspective(65.0f, 16.0f / 9.0f, 0.1f, 10000.0f);
};


class GhostEntity : public AbstractCameraEntity {
public:
    GhostEntity(glm::vec3 position, glm::vec3 lookat);

    virtual void loadCameraMatrices();

protected:
    glm::mat4 projectionmatrix = glm::perspective(65.0f, 16.0f / 9.0f, 0.1f, 10000.0f);
};

#endif // CAMERA_H

#include "camera.h"
#include "game/physcube.h"
#include <iostream>

PlayerEntity::PlayerEntity(btDynamicsWorld *bt_world, MediaStreamer &audio, EntityManager &manager) : audio(audio), manager(manager) {
    obj->setMassProps(2.0, btVector3(0,0,0));
    obj->setCollisionShape(new btCapsuleShape(1, 2));
    obj->setFriction(1);
    obj->activate(true);
    obj->setMotionState(groundMotionState);
    current_world = bt_world;
    fixed = false;
    drwav_init_file(&pWav, "audio/bang.wav", NULL);
    pSampleData = new int16_t[pWav.totalPCMFrameCount];
    drwav_read_pcm_frames_s16(&pWav, pWav.totalPCMFrameCount, pSampleData);
    std::cout << "Testing...." << std::endl;
}

void PlayerEntity::updateCamera(GLFWwindow *windowptr, double dt) {
    obj->activate(true);
    btVector3 location = obj->getWorldTransform().getOrigin();
    location.setY(location.getY() + 0.5f);
    glEnable(GL_LIGHT0);
    int xscr, yscr;
    glfwGetWindowSize(windowptr, &xscr, &yscr);
    double xpos = 0, ypos = 0;
    if (!no) {
        glfwGetCursorPos(windowptr, &xpos, &ypos);

    }
    else {
        xpos = xscr/2;
        ypos = yscr/2;
    }
    glfwSetCursorPos(windowptr, xscr/2, yscr/2);
    horizontal += dt * mspeed * ((xscr/2) - xpos);
    vertical += dt * mspeed * ((yscr/2) - ypos);

    if (vertical > 1.5f) {
        vertical = 1.5f;
    }
    else if (vertical < -1.5f) {
        vertical = -1.5f;
    }
    glm::vec3 direction(cos(vertical) * sin(horizontal), sin(vertical), cos(horizontal) * cos(vertical));
    glm::vec3 right = glm::vec3(sin(horizontal - 3.14f / 2.0f), 0, cos(horizontal - 3.14f / 2.0f));
    glm::vec3 up = glm::cross(right, direction);
    float position[] = { location.getX(),location.getY(),location.getZ(),1.0f };
    float ambient[] = { 0.2f, (51 / 255.0),(51 / 255.0) };
    //float light_direction[] = { position.x, position.y, position.z, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    bool moved = false;
    if (glfwGetKey(windowptr, GLFW_KEY_W) == GLFW_PRESS) {
        moved = true;
        glm::vec3 temporary = direction * speed;
        //temporary.y = 0;
        //btVector3 a(convert(temporary));
        //a.setY(obj->getLinearVelocity().getY());
        //a.setX(a.getX() * 25);
        //a.setZ(a.getZ() * 25);
        //obj->setLinearVelocity(a);
        //obj->setLinearVelocity(obj->getLinearVelocity() + convert(temporary) * btVector3(4,4,4));
        obj->applyCentralImpulse(btVector3(temporary.x, temporary.y, temporary.z));
    }

    if (glfwGetKey(windowptr, GLFW_KEY_S) == GLFW_PRESS) {
        moved = true;
        glm::vec3 temporary = -(direction * speed);
        temporary.y = 0;
        btVector3 a(convert(temporary));
        a.setY(obj->getLinearVelocity().getY());
        a.setX(a.getX() * 25);
        a.setZ(a.getZ() * 25);
        obj->setLinearVelocity(a);
        //obj->setLinearVelocity(obj->getLinearVelocity() + convert(temporary) * btVector3(4,4,4));
        //obj->applyCentralImpulse(btVector3(temporary.x, temporary.y, temporary.z));
    }

    if (glfwGetKey(windowptr, GLFW_KEY_D) == GLFW_PRESS) {
        moved = true;
        glm::vec3 temporary = right * speed;
        temporary.y = 0;
        btVector3 a(convert(temporary));
        a.setY(obj->getLinearVelocity().getY());
        a.setX(a.getX() * 25);
        a.setZ(a.getZ() * 25);
        obj->setLinearVelocity(a);
        //obj->setLinearVelocity(obj->getLinearVelocity() + convert(temporary) * btVector3(4,4,4));
        //obj->applyCentralImpulse(btVector3(temporary.x, temporary.y, temporary.z));

    }

    if (glfwGetKey(windowptr, GLFW_KEY_A) == GLFW_PRESS) {
        moved = true;
        glm::vec3 temporary = -(right * speed);
        temporary.y = 0;
        btVector3 a(convert(temporary));
        a.setY(obj->getLinearVelocity().getY());
        a.setX(a.getX() * 25);
        a.setZ(a.getZ() * 25);
        obj->setLinearVelocity(a);
        //obj->setLinearVelocity(obj->getLinearVelocity() + convert(temporary) * btVector3(4,4,4));
        //obj->applyCentralImpulse(btVector3(temporary.x, temporary.y, temporary.z));
    }

    if (glfwGetKey(windowptr, GLFW_KEY_N) == GLFW_PRESS) {
        no = true;
        if (current_world->getGravity().getY() == 0) {
            current_world->setGravity(btVector3(0, -9, 0));
            goto bail;
        }
        current_world->setGravity(btVector3(0, 0, 0));
    }

    if (glfwGetKey(windowptr, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (jumpEnable) {
            obj->applyCentralImpulse(btVector3(0, 24, 0));
            jumpEnable = false;
        }
    }

    if (cubeEnable && (glfwGetKey(windowptr, GLFW_KEY_E) == GLFW_PRESS)) {
        audio.QueueNewSound(pSampleData, pWav.totalPCMFrameCount, false);
        PhysCube *cube = new PhysCube(glm::vec3(location.getX(), location.getY(), location.getZ()), direction);
        cube->setPosition(direction*btScalar(4.0) + glm::vec3(obj->getWorldTransform().getOrigin().getX(), obj->getWorldTransform().getOrigin().getY(), obj->getWorldTransform().getOrigin().getZ()));
        cube->getRigidBody()->setLinearVelocity(convert(direction)*btScalar(5.f));
        std::shared_ptr<CollidableEntity> physc;
        physc.reset(cube);
        manager.addEntity(physc);
        manager.collision.setSimSpeed(4.0f);
        cubeEnable = false;
    }
    if(glfwGetMouseButton(windowptr, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && cap_on) {
        cap_on = false;
        CollidableEntity* ent = (CollidableEntity*)manager.getCollidableEntity(captured_obj);
        ent->getRigidBody()->clearForces();
        ent->getRigidBody()->activate(true);
        ent->getRigidBody()->setLinearVelocity(convert(glm::normalize(direction)*50.0f));
        captured_obj = 0;


    }
    if(glfwGetMouseButton(windowptr, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS && !cap_on) {
        struct Ray r;
        r.position = glm::vec3(location.getX(), location.getY(), location.getZ());
        r.direction = glm::normalize(direction);
        unsigned long id  = manager.collision.pickEntity(r);
        std::cout << id << std::endl;
        if(id) {
            CollidableEntity* ent = (CollidableEntity*)manager.getCollidableEntity(id);
            if(ent && ent->getAttribute("type").repstring == "PhysCube") {
                glm::vec3 pos = ent->getPosition();
                glm::vec3 cam_f = r.position + 5.0f*r.direction;
                glm::vec3 diff = pos - cam_f;
                diff = diff/10.0f;

                ent->setPosition(ent->getPosition() - diff);
                if(captured_obj != id) {
                    captured_obj = id;
                    boxctr = 0;
                }
                int axyz = (std::fabs(diff.x) < 0.05f) + (std::fabs(diff.z) < 0.05f) + (std::fabs(diff.y) < 0.05f);
                if(axyz > 1) {
                    cap_on = true;
                }
                boxctr++;
            }
        }
    }
    if(cap_on) {
        CollidableEntity* ent = (CollidableEntity*)manager.getCollidableEntity(captured_obj);
        if(!ent) {
            cap_on = false;
            goto no_cap;
        }
        ent->setPosition(glm::vec3(location.getX(), location.getY(), location.getZ()) + 5.0f*glm::normalize(direction));
        ent->getRigidBody()->setAngularVelocity(btVector3(0,0,0));
        ent->getRigidBody()->setLinearVelocity(btVector3(0,0,0));

    }
no_cap:
    if (!cubeEnable) {
        cctr++;
    }
    //std::cout << "y" << obj->getLinearVelocity().getY() << std::endl;
    if (!jumpEnable && (obj->getLinearVelocity().getY() < 0.001f) && (obj->getLinearVelocity().getY() > -0.001f)) {
        std::cout << "iframe" << std::endl;
        framectr++;
    }
    if (framectr == 3) {
        jumpEnable = true;
        framectr = 0;
    }

    if (cctr == 12) {
        cubeEnable = true;
        manager.collision.setSimSpeed(1.0f);
        cctr = 0;
    }

bail:
    btVector3 vel = obj->getLinearVelocity();
    vel.setY(clip(vel.getY(), -32, 32));
    vel.setX(clip(vel.getX(), -12, 12));
    vel.setZ(clip(vel.getZ(), -12, 12));
    obj->setLinearVelocity(vel);
    viewmatrix = glm::lookAt(glm::vec3(location.getX(), location.getY(), location.getZ()), glm::vec3(location.getX(), location.getY(), location.getZ()) + direction, up) * glm::mat4(1.0);
    //std::cout << location.getY() << std::endl;
    obj->setAngularVelocity(btVector3(0,0,0));
    speed = 20.0f;
}

void PlayerEntity::loadCameraMatrices() {
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&viewmatrix[0][0]);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(&projectionmatrix[0][0]);
}

GhostEntity::GhostEntity(glm::vec3 position, glm::vec3 lookat) {
    viewmatrix = glm::lookAt(position, lookat, glm::vec3(0,1,0));
    attributes["type"] = DatObj("GhostEntity");
}

void GhostEntity::loadCameraMatrices() {
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&viewmatrix[0][0]);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(&projectionmatrix[0][0]);
}

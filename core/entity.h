#ifndef ENTITY_H
#define ENTITY_H

#include <glm/common.hpp>
#include <unordered_map>
#include "deps/utilityfunc.h"
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

enum DatumType {
    STRING,
    INTEGER,
    FLOAT,
    NONE
};

struct DatObj {
    DatumType type;

    DatObj(std::string s) {
        type = STRING;
        repstring = s;
    }

    DatObj(int i) {
        type = INTEGER;
        repint = i;
    }

    DatObj(float f) {
        type = FLOAT;
        repfloat = f;
    }

    DatObj() {
        type = NONE;
    }

    std::string repstring;
    int repint;
    float repfloat;
};

class Entity {
public:
    Entity() { attributes["type"] = DatObj("Entity"); }
    virtual void onTick(double dt) {}
    ~Entity() {}
    DatObj getAttribute(std::string name) { return attributes[name]; }
protected:
    std::unordered_map<std::string, DatObj> attributes;
private:
    // std::unordered_map<std::string, std::function<
};

class RenderedEntity : public Entity {
public:
    RenderedEntity() { attributes["type"] = DatObj("RenderedEntity"); }
    virtual void drawSelf(void) {}
    virtual glm::vec3 getPosition(void) { return xyz; }
    virtual void setPosition(glm::vec3 vec) { xyz = vec; }
protected:
    glm::vec3 xyz;
};

class CollidableEntity : public RenderedEntity {
public:
    CollidableEntity() {
        attributes["type"] = DatObj("CollidableEntity");
        obj->setUserPointer(this);
    }
    virtual void setPosition(glm::vec3 vec) { xyz = vec; obj->getWorldTransform().setOrigin(btVector3(xyz.x,xyz.y,xyz.z)); }
    virtual glm::vec3 getPosition() { return convert(obj->getWorldTransform().getOrigin()); }
    virtual void onCollision() {}
    virtual bool isFixed() { return fixed; }
    virtual btRigidBody* getRigidBody() { return obj; }
protected:
    bool fixed = true;
    btDefaultMotionState *defaultMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1)));
    btCollisionShape *shape = new btEmptyShape();
    btRigidBody *obj = new btRigidBody(0.0f,defaultMotionState,shape);

};


#endif // ENTITY_H

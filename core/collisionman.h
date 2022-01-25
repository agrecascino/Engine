#ifndef COLLISION_H
#define COLLISION_H
#include "core/entity.h"
#include <map>
#include <memory>

struct Ray {
    glm::vec3 position;
    glm::vec3 direction;
};

class CollisionManager {
    friend class EntityManager;
    friend class glRenderer;
public:
    CollisionManager();

    unsigned long collisionPointerToID(void *entity);

    void removeCollidableEntity(unsigned long id);

    unsigned long pickEntity(struct Ray r);

    float setSimSpeed(float speed);

    void stepSimulation(float dt);

private:
    float simSpeed = 1.0f;
    btCollisionConfiguration *bt_collision_config;
    btCollisionDispatcher *bt_dispatcher;
    btBroadphaseInterface *bt_broadphase;
    btSequentialImpulseConstraintSolver* bt_solver;
    btDiscreteDynamicsWorld *bt_world;
    std::map<unsigned long, std::shared_ptr<CollidableEntity>> collidables;
};


#endif // COLLISION_H

#ifndef ENTITYMAN_H
#define ENTITYMAN_H

#include "core/entity.h"
#include "core/collisionman.h"

class EntityManager {
    friend class CollisionManager;
    friend class glRenderer;
public:
    unsigned long addEntity(std::shared_ptr<CollidableEntity> &entity);

    unsigned long addEntity(std::shared_ptr<RenderedEntity> &entity);

    unsigned long addEntity(std::shared_ptr<Entity> &entity);

    void removeEntity(unsigned long id);

    void removeRenderableEntity(unsigned long id);

    void removeCollidableEntity(unsigned long id);

    Entity* getCollidableEntity(unsigned long id);

    void drawAllObjects();

    void tickAllObjects(float dt);

    void clearAllObjects();

    CollisionManager collision;
private:
    unsigned long ids[3] = { 1, 1, 1 };
    std::map<unsigned long, std::shared_ptr<Entity>> gameobjects;
    std::map<unsigned long, std::shared_ptr<RenderedEntity>> renderobjects;
    std::map<unsigned long, std::shared_ptr<CollidableEntity>> &collidables = collision.collidables;
};

#endif // ENTITYMAN_H

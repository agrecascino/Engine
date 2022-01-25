#include "core/entity.h"
#include "core/collisionman.h"
#include "core/entityman.h"

unsigned long EntityManager::addEntity(std::shared_ptr<CollidableEntity> &entity) {
    collidables[ids[2]] = std::move(entity);
    collision.bt_world->addRigidBody(collidables[ids[2]]->getRigidBody());
    ids[2]++;
    return ids[2] - 1;

}
unsigned long EntityManager::addEntity(std::shared_ptr<RenderedEntity> &entity) {
    renderobjects[ids[1]] = std::move(entity);
    ids[1]++;
    return ids[1] - 1;
}

unsigned long EntityManager::addEntity(std::shared_ptr<Entity> &entity) {
    gameobjects[ids[0]] = std::move(entity);
    ids[0]++;
    return ids[0] - 1;
}

void EntityManager::removeEntity(unsigned long id) {
    gameobjects.erase(id);
}

void EntityManager::removeRenderableEntity(unsigned long id) {
    renderobjects.erase(id);
}

void EntityManager::removeCollidableEntity(unsigned long id) {
    collision.removeCollidableEntity(id);
}

Entity* EntityManager::getCollidableEntity(unsigned long id) {
    if(collidables.find(id) != collidables.end())
        return collidables[id].get();
    return NULL;
}

void EntityManager::drawAllObjects() {
    for (auto& kv : renderobjects) {
        kv.second->drawSelf();
    }

    for (auto& kv : collidables) {
        kv.second->drawSelf();
    }
}

void EntityManager::tickAllObjects(float dt) {
    for (auto &kv : gameobjects) {
        kv.second->onTick(dt);
    }

    for (auto &kv : renderobjects) {
        kv.second->onTick(dt);
    }

    for (auto &kv : collidables) {
        kv.second->onTick(dt);
    }
}

void EntityManager::clearAllObjects() {
    for (auto &kv : collidables) {
        collision.bt_world->removeRigidBody(kv.second->getRigidBody());
    }
    gameobjects.clear();
    renderobjects.clear();
    collidables.clear();
}

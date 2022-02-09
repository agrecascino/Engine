#include "core/collisionman.h"
#include <iostream>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/BroadphaseCollision/btAxisSweep3.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/Gimpact/btGImpactShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btEmptyShape.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h>
#include <LinearMath/btDefaultMotionState.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

CollisionManager::CollisionManager() {
    bt_collision_config = new btDefaultCollisionConfiguration();
    bt_dispatcher = new btCollisionDispatcher(bt_collision_config);
    bt_broadphase = new btDbvtBroadphase();
    bt_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    bt_solver = new btSequentialImpulseConstraintSolver;
    bt_world = new btDiscreteDynamicsWorld(bt_dispatcher, bt_broadphase, bt_solver, bt_collision_config);
    bt_world->setGravity(btVector3(0, -29, 0));
    btGImpactCollisionAlgorithm::registerAlgorithm(bt_dispatcher);
    bt_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}

unsigned long CollisionManager::collisionPointerToID(void *entity) {
    for(auto const& item : collidables) {
        if((void*)item.second.get() == entity) {
            return item.first;
        }
    }
    return 0;
}

void CollisionManager::removeCollidableEntity(unsigned long id) {
    bt_world->removeRigidBody(collidables[id]->getRigidBody());
    collidables.erase(id);
}

unsigned long CollisionManager::pickEntity(struct Ray r) {
    btVector3 start = convert(r.position);
    btVector3 end = convert(r.position + 2000.f*r.direction);
    btCollisionWorld::ClosestRayResultCallback ray_callback(start, end);
    bt_world->rayTest(start, end, ray_callback);
    if(ray_callback.hasHit()) {
        std::cout << "Hit Detected!" << std::endl;
        return collisionPointerToID(ray_callback.m_collisionObject->getUserPointer());
    }
    return 0;
}

float CollisionManager::setSimSpeed(float speed) {
    simSpeed = speed;
}

void CollisionManager::stepSimulation(float dt) {
    bt_world->stepSimulation(dt/simSpeed,0, 1/480.f);
    for(auto &kv : collidables) {
        if(kv.second->getPosition().y < -10.f) {
            removeCollidableEntity(kv.first);
        }
    }
}

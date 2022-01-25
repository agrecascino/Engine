#include "game/physcube.h"


PhysCube::PhysCube(glm::vec3 position, glm::vec3 direction) {
    obj->setMassProps(3.0, btVector3(1,1,1));
    obj->setCollisionShape(new btBoxShape(btVector3(1, 1, 1)));
    this->setPosition(direction*btScalar(4.0) + position);
    this->obj->applyCentralImpulse(btVector3(direction.x*16, direction.y*16, direction.z*16));
    attributes["type"] = DatObj("PhysCube");
}

void PhysCube::drawSelf() {
    btScalar m[16];
    obj->getWorldTransform().getOpenGLMatrix(m);
    GLfloat matrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(m);
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < 108; i += 3) {
        btVector4 vec(g_vertex_buffer_data[i], g_vertex_buffer_data[i + 1], g_vertex_buffer_data[i + 2], 1.0);
        glVertex3f(vec.getX(), vec.getY(), vec.getZ());
    }
    glEnd();
    glPopMatrix();
    glLoadMatrixf(matrix);
}

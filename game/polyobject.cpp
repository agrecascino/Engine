#include "game/polyobject.h"
#include "deps/utilityfunc.h"
#include <GLFW/glfw3.h>

PolyObject::PolyObject(glm::vec3 pos) {
    xyz = pos;
    obj->getWorldTransform().setOrigin(btVector3(xyz.x, xyz.y, xyz.z));
    attributes["type"] = DatObj("PolyObject");
}

void PolyObject::setGeometry(std::vector<glm::vec3> polygons, bool isquad) {
    if (isquad) {
        for (size_t i = 0; i < polygons.size(); i += 4) {
            vertices.push_back(polygons[i]);
            vertices.push_back(polygons[i + 1]);
            vertices.push_back(polygons[i + 2]);
            vertices.push_back(polygons[i + 1]);
            vertices.push_back(polygons[i + 2]);
            vertices.push_back(polygons[i + 3]);
        }
    }
    else {
        vertices = polygons;
    }
    quad = isquad;
    btTriangleMesh *mesh = new btTriangleMesh;
    if (!isquad) {
        assert((polygons.size() % 3) == 0);
    }
    else {
        assert((polygons.size() % 4) == 0);
    }
    if (!isquad) {
        for (size_t i = 0; i < polygons.size(); i += 3) {
            mesh->addTriangle(btVector3(polygons[i].x, polygons[i].y, polygons[i].z), btVector3(polygons[i + 1].x, polygons[i + 1].y, polygons[i + 1].z), btVector3(polygons[i + 2].x, polygons[i + 2].y, polygons[i + 2].z));
        }
    }
    else {
        for (size_t i = 0; i < vertices.size(); i += 3) {
            mesh->addTriangle(convert(vertices[i]), convert(vertices[i + 1]), convert(vertices[i + 2]));
        }
    }
    obj->setCollisionShape(new btBvhTriangleMeshShape(mesh, false));
}

void PolyObject::setTextures(std::vector<GLuint> textures) {
    if (quad) {
        for (size_t i = 0; i < textures.size(); i++) {
            gltex.push_back(textures[i]);
            gltex.push_back(textures[i]);
            gltex.push_back(textures[i]);
            gltex.push_back(textures[i]);
            gltex.push_back(textures[i]);
            gltex.push_back(textures[i]);

        }
        //assert(((textures.size() * (2 / 3)) / 4) >= C);
        textured = true;
        return;
    }
    else {
        assert((textures.size() / 3) >= textures.size());
    }
    gltex = textures;
    textured = true;

}

void PolyObject::setColor(std::vector<glm::vec3> c, bool pv) {
    if (!quad) {
        colors = c;
    }
    else {
        for (size_t i = 0; i < c.size(); i += 4) {
            colors.push_back(c[i]);
            colors.push_back(c[i + 1]);
            colors.push_back(c[i + 2]);
            colors.push_back(c[i + 1]);
            colors.push_back(c[i + 2]);
            colors.push_back(c[i + 3]);
        }
    }
    pervertexcoloring = pv;
}

void PolyObject::drawSelf(void) {
    //std::cout << obj->getWorldTransform().getOrigin().getY() << std::endl;
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);
    if (colors.size()) {
        if (!pervertexcoloring) {
            assert((colors.size() % 3) == 0);
            assert(colors.size() >= (vertices.size() / 3));
        }
        else {
            assert(colors.size() >= vertices.size());
        }
    }
    for (size_t i = 0; i+2 < vertices.size(); i+= 3) {
        glBindTexture(GL_TEXTURE_2D, gltex[i]);
        btScalar m[16];
        obj->getWorldTransform().getOpenGLMatrix(m);
        glPushMatrix();
        glMultMatrixf(m);
        glBegin(GL_TRIANGLES);
        if (!textured) {
            if (colors.size()) {
                if (pervertexcoloring) {
                    glColor3f(colors[i].x, colors[i].y, colors[i].z);
                }
                else {
                    if (((i + 1) % 3) == 0) {
                        glColor3f(colors[i / 3].x, colors[i / 3].y, colors[i / 3].z);
                    }
                }
            }
            else {
                glColor3f(0.8, 0.8, 0.8);
            }
        }
        //glm::vec3 normal = computeTriangleNormal(vertices[i], vertices[i + 1], vertices[i + 2]);
        //glNormal3f(normal.x, normal.y, normal.z);
        setQuadUV(i);
        glVertex3f(vertices[i].x + xyz.x, vertices[i].y + xyz.y, vertices[i].z + xyz.z);
        setQuadUV(i+1);
        glVertex3f(vertices[i+1].x + xyz.x, vertices[i+1].y + xyz.y, vertices[i+1].z + xyz.z);
        setQuadUV(i+2);
        glVertex3f(vertices[i+2].x + xyz.x, vertices[i+2].y + xyz.y, vertices[i+2].z + xyz.z);
        glEnd();
        glPopMatrix();
    }
    if (textured) {
        glDisable(GL_TEXTURE_2D);
    }
    glDisable(GL_NORMALIZE);
    glDisable(GL_COLOR_MATERIAL);
}

void PolyObject::setQuadUV(int i) {
    size_t k = i - (floor(i / 6.0)) * 6;

    if (k == 5) {
        glTexCoord2f(1.0f, 1.0f);
    }
    else if (k == 4) {
        glTexCoord2f(1.0f, 0.0f);
    }
    else if (k == 3) {
        glTexCoord2f(0.0f, 1.0f);
    }
    else if (k == 2) {
        glTexCoord2f(1.0f, 0.0f);
    }
    else if (k == 1) {
        glTexCoord2f(0.0f, 1.0f);
    }
    else if (k == 0) {
        glTexCoord2f(0.0f, 0.0f);
    }
}

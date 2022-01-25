#ifndef POLYOBJECT_H
#define POLYOBJECT_H

#include "core/entity.h"
#include <vector>

class PolyObject : public CollidableEntity {
public:
    PolyObject(glm::vec3 pos);

    void setGeometry(std::vector<glm::vec3> polygons, bool isquad = false);

    void setTextures(std::vector<GLuint> textures);

    void setColor(std::vector<glm::vec3> c, bool pv);

    void drawSelf(void);

private:
    void setQuadUV(int i);

    bool textured = false;
    bool quad = true;
    bool pervertexcoloring = true;
    std::vector<GLuint> gltex;
    std::vector<float[2]> uvs;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> vertices;
};

#endif // POLYOBJECT_H

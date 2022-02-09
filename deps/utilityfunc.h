#ifndef UTILITYFUNC_H
#define UTILITYFUNC_H

#include <bullet/btBulletCollisionCommon.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

inline bool isEqual(double x, double y)
{
    const double epsilon = 1e-3/* some small number such as 1e-5 */;
    return std::abs(x - y) <= epsilon * std::abs(x);
    // see Knuth section 4.2.2 pages 217-218
}

inline float clip(float n, float lower, float upper) {
    return std::max(lower, std::min(n, upper));
}

inline btVector3 convert(glm::vec3 v) {
    return btVector3(v.x, v.y, v.z);
}

inline glm::vec3 convert(btVector3 v) {
    return glm::vec3(v.getX(), v.getY(), v.getZ());
}

inline void test(float x, float y) {
    glColor3f(x, y, 0);
}

inline glm::vec3 computeTriangleNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    return glm::normalize(glm::cross(b - a, c - a));
}

#endif // UTILITYFUNC_H

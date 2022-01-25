#include "game/sky.h"

Sky::Sky() { attributes["type"] = DatObj("Sky"); }

void Sky::drawSelf(void) {
    glClearColor((98/255.0), (150/255.0), (234/255.0), 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

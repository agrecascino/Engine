#include "schemagui.h"
#include <glm/common.hpp>
#include <cstring>
#include <GLFW/glfw3.h>
#include "deps/libfont.h"

void schemaGUI::updateScreenResolution(unsigned xscr, unsigned yscr) {
    this->yscr = yscr;
    this->xscr = xscr;
}

void schemaGUI::addNewSchema(std::string name, LoadedSchema s) {
    schemas[name] = s;
}

void schemaGUI::removeSchema(std::string name) {
    schemas.erase(name);
}

void schemaGUI::clearSchemas() {
    schemas.clear();
}

void schemaGUI::activateSchema(std::string name) {
    activeschema = name;
}

void schemaGUI::drawGUI() {
    if(schemas.find("activeschema") != schemas.end())
        return;
    LoadedSchema &s = schemas[activeschema];
    for(Text &t : s.text) {
        drawTextToFB(t.scl, t.data.c_str(), t.x, t.y, 1.0f);
    }
}

void schemaGUI::drawTextToFB(float scl, const char *t, float xposc, float yposc, float brightness) {
    struct Texture {
        uint8_t *data;
        float x;
        float y;
        float scale;
        float xoff;
        float yoff;
    };
    /* Disable 3D features. */
    glDisable(GL_DEPTH);
    glDisable(GL_LIGHTING);
    /* Load matrices */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /* Ortho projection so we can draw GUI text in screenspace.
         *        ________________(xscr, yscr)
         *       |                |
         *       |                |
         *       |                |
         *       |                |
         *       |                |
         *       |________________|
         *  (0, 0)
         */
    glOrtho(0, xscr, 0, yscr, -1, 1);
    /* Clear depth buffer so text is drawn in front of 3D */
    glClear(GL_DEPTH_BUFFER_BIT);
    /* Empty MODELVIEW */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /* Enable blending for transparency */
    glEnable(GL_BLEND);
    /* The default blend function for existence */
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    /* Default alpha */
    glm::vec4 alpha(1.0f, 1.0f, 1.0f, 1.0f);
    unsigned char *tdata = drawText(t, alpha);
    struct Texture tex;
    tex.data = tdata;
    tex.scale = scl;
    tex.x = xposc;
    tex.y = yposc;
    tex.xoff = 9 * strlen(t);
    tex.yoff = 15;
    float truex = tex.xoff*scl;
    float truey = tex.yoff*scl;
    GLuint gtex;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &gtex);
    glBindTexture(GL_TEXTURE_2D, gtex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.xoff, 15, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data);
    glBindTexture(GL_TEXTURE_2D, gtex);
    glBegin(GL_QUADS);
    float xinscr = xscr*tex.x;
    float yinscr = yscr*tex.y;
    float xadj = xinscr - truex/2;
    float yadj = yinscr - truey/2;
    glm::vec3 v1 = glm::vec3(xadj,  yadj, 0);
    glm::vec3 v2 = glm::vec3(xadj + truex, yadj, 0);
    glm::vec3 v3 = glm::vec3(xadj + truex, yadj + truey, 0);
    glm::vec3 v4 = glm::vec3(xadj, yadj + truey, 0);
    glTexCoord2f(0, 0); glVertex3f(v1.x, v1.y, -1);
    glTexCoord2f(1, 0); glVertex3f(v2.x, v2.y, -1);
    glTexCoord2f(1, 1); glVertex3f(v3.x, v3.y, -1);
    glTexCoord2f(0, 1); glVertex3f(v4.x, v4.y, -1);
    glEnd();
    glDeleteTextures(1, &gtex);
    glDisable(GL_TEXTURE_2D);
    free(tdata);
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH);
    glEnable(GL_LIGHTING);

}

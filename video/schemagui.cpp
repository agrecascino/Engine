#include "schemagui.h"
#include <glm/common.hpp>
#include <cstring>
#include <GLFW/glfw3.h>
#include "deps/libfont.h"
#include <stack>

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

bool schemaGUI::inControl() {
    return control;
}

void schemaGUI::takeControl() {
    control = true;
}

void schemaGUI::loseControl() {
    control = false;
}

bool schemaGUI::AABBTestText(float x, float y, Text &t) {
    int xoff = 9 * t.data.size();
    int yoff = 15;
    float truex = xoff*t.scl;
    float truey = yoff*t.scl;
    float xinscr = xscr*t.x;
    float yinscr = yscr*t.y;
    float adjf_x = 0.0f;
    float adjf_y = 0.0f;
    switch(t.align) {
    case CENTER:
        adjf_x = truex/2;
        adjf_y = truey/2;
        break;
    case LEFT:
        adjf_x = 0;
        adjf_y = 0;
        break;
    case RIGHT:
        adjf_x = truex;
        adjf_y = truey;
        break;
    }
    float xadj = xinscr - adjf_x;
    float yadj = yinscr - adjf_y;
    glm::vec3 a = glm::vec3(xadj,  yadj, 0);
    glm::vec3 b = glm::vec3(truex, truey, 0);
    glm::vec3 p = glm::vec3(x*xscr, y*yscr, 0);
    glm::vec3 diff_pa = p - a;
    if((diff_pa.x < b.x) && (diff_pa.x > 0) &&
       (diff_pa.y < b.y) && (diff_pa.y > 0)   )
        return true;
    return false;
}

void schemaGUI::schemaDebounce(GLFWwindow *windowptr) {
    for(int i = 0; i < 2; i++) {
        int button = GLFW_MOUSE_BUTTON_1 + i;
        int pressed = glfwGetMouseButton(windowptr, button);
        switch(pressed) {
        case GLFW_PRESS:
            switch(buttons[i]) {
            case SCHEMA_LIFTED:
                buttons[i] = SCHEMA_CLICK;
                break;
            case SCHEMA_CLICK:
            case SCHEMA_HOLD:
                buttons[i] = SCHEMA_HOLD;
                break;
            }
            break;
        case GLFW_RELEASE:
            buttons[i] = SCHEMA_LIFTED;
            break;
        }
    }
}

schemaKeyPress schemaGUI::schemaGetDebouncedKey(schemaMouseButton button) {
    return buttons[button];
}


struct wconfig {
    float x;
    float y;
    float xsz;
    float ysz;
    float ratio;
};

wconfig layoutWindowInvariantCoordinates(struct Window *w, float xscr, float yscr) {
    //Walk up tree and find render config.
    std::stack<Window*> wptrs;
    Window *wcopy = w;
    while(wcopy != NULL) {
        wptrs.push(wcopy);
        wcopy = wcopy->parent;
    }
    //Walk down render stack.
    float x = 0.0f;
    float y = 0.0f;
    float xsz = xscr;
    float ysz = yscr;
    float ratio = xsz/ysz;
    if(!w->parent)
        ratio = 1.0f;
    while(wptrs.size() > 0) {
        Window *w2 = wptrs.top();
        x += w2->x*xsz;
        y += w2->y*ysz;
        xsz = w2->xsz*xsz;
        ysz = w2->ysz*ysz;
        wptrs.pop();
    }
    struct wconfig wconf;
    wconf.x = x;
    wconf.y = y;
    wconf.xsz = xsz;
    wconf.ysz = ysz;
    wconf.ratio = ratio;
    return wconf;

}

void schemaGUI::drawWindow(Window *w) {
    if(w->hidden)
        return;
    struct wconfig wconf = layoutWindowInvariantCoordinates(w, xscr, yscr);
    drawWindowToFB(wconf.x, wconf.y, wconf.xsz, wconf.ysz*wconf.ratio, w->color);
    for(Text &t : w->text) {
        float texlocx = wconf.x + wconf.xsz*t.x;
        float texlocy = wconf.y + wconf.ysz*t.y*wconf.ratio;
        drawTextToFB(t.scl, t.data.c_str(), texlocx, texlocy, 1.0f, t.align);
    }
    for(Window *w2 : w->subwindows) {
        drawWindow(w2);
    }
}

bool isCursorInWindow()
Window* schemaGUI::pickWindowMain(Window *w, float x, float y) {
    for(Window *w2 : w->subwindows) {
        if(w2->hidden)
            continue;
        struct wconfig wconf = layoutWindowInvariantCoordinates(w2, xscr, yscr);
        float xcurs = x*xscr;
        float ycurs = y*yscr;
        glm::vec3 a = glm::vec3(wconf.x,  wconf.y, 0);
        glm::vec3 b = glm::vec3(wconf.xsz, wconf.ysz*wconf.ratio, 0);
        glm::vec3 p = glm::vec3(xcurs, ycurs, 0);
        glm::vec3 diff_pa = p - a;
        if(diff_pa.y < .95*b.y)
            continue;
        if((diff_pa.y > .95*b.y) && diff_pa.x > .95*b.x) {
            w2->hidden = true;
            continue;
        }
        if((diff_pa.x < b.x) && (diff_pa.x > 0) &&
           (diff_pa.y < b.y) && (diff_pa.y > 0)   )
            return w2;
    }
    return NULL;
}

Text* schemaGUI::deepItemPick(Window *w, float x, float y) {
    if()
    for(Window *w2 : w->subwindows) {

    }
}


void schemaGUI::handleControl(GLFWwindow *windowptr) {
    double x, y;
    glfwGetCursorPos(windowptr, &x, &y);
    float cx = x/xscr;
    float cy = y/yscr;
    float ratio = ((float)xscr)/yscr;
    drawTextToFB(2.0, "+", xscr*cx, yscr*(1.0f-cy), 1.0f);
    std::string debug = std::to_string(cx*ratio) + ", " + std::to_string(cy);
    drawTextToFB(1.0f, debug.c_str(), xscr*0.5f, yscr*0.95f, 1.0f);
    schemaDebounce(windowptr);
    if(schemaGetDebouncedKey(SCHEMA_BUTTON_1) == SCHEMA_CLICK) {
        if(schemas.find(activeschema) == schemas.end())
            goto bail;
        xcursclick = cx;
        ycursclick = 1.0f-cy;
        LoadedSchema &s = schemas[activeschema];
//        for(Text &t : s.text) {
//            bool click = AABBTestText(cx, 1.0f-cy, t);
//            if(click) {
//                std::string dbg_win = "hit! " + t.data;
//                drawTextToFB(1.0f, dbg_win.c_str(), 0.5f, 0.90f, 1.0f);
//                if(t.action == "exit") {
//                    exit(0);
//                }
//            }
//        }
    }
    if(schemaGetDebouncedKey(SCHEMA_BUTTON_1) == SCHEMA_HOLD) {
        float xdiff = cx-xcursclick;
        float ydiff = (1.0f-cy)-ycursclick;
        if(schemas.find(activeschema) == schemas.end())
            goto bail;
        LoadedSchema &s = schemas[activeschema];
        Window *w2 = pickWindowMain(s.main, cx, 1.0f-cy);
        if(w2) {
            w2->x += xdiff;
            w2->y += ydiff;
        }
        xcursclick = cx;
        ycursclick = 1.0f-cy;
    }
    bail:
    return;
}

void schemaGUI::drawGUI() {
    if(schemas.find(activeschema) == schemas.end())
        return;
    LoadedSchema &s = schemas[activeschema];
    drawWindow(s.main);
}



void schemaGUI::drawWindowToFB(float x, float y, float xsz, float ysz, glm::vec4 color) {
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
    glm::vec4 alpha(color.x, color.y, color.z, color.w);
    glBegin(GL_QUADS);
    glColor4f(alpha.x, alpha.y, alpha.z, alpha.w);
    glm::vec3 v1 = glm::vec3(x,  y, 0);
    glm::vec3 v2 = glm::vec3(x + xsz, y, 0);
    glm::vec3 v3 = glm::vec3(x + xsz, y + ysz, 0);
    glm::vec3 v4 = glm::vec3(x, y + ysz, 0);
    glTexCoord2f(0, 0); glVertex3f(v1.x, v1.y, -1);
    glTexCoord2f(1, 0); glVertex3f(v2.x, v2.y, -1);
    glTexCoord2f(1, 1); glVertex3f(v3.x, v3.y, -1);
    glTexCoord2f(0, 1); glVertex3f(v4.x, v4.y, -1);
    glEnd();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH);
    glEnable(GL_LIGHTING);
}


void schemaGUI::drawTextToFB(float scl, const char *t, float xposc, float yposc, float brightness, TextAlignment align) {
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
    glm::vec4 alpha(1.0f, 1.0f, 1.0f, 0.7f);
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
    float xinscr = tex.x;
    float yinscr = tex.y;
    float adjf_x = 0.0f;
    float adjf_y = 0.0f;
    switch(align) {
    case CENTER:
        adjf_x = truex/2;
        adjf_y = truey/2;
        break;
    case LEFT:
        adjf_x = 0;
        adjf_y = 0;
        break;
    case RIGHT:
        adjf_x = truex;
        adjf_y = truey;
        break;
    }
    float xadj = xinscr - adjf_x;
    float yadj = yinscr - adjf_y;
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

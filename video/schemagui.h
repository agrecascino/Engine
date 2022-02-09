#ifndef SCHEMAGUI_H
#define SCHEMAGUI_H
#include <string>
#include <vector>
#include <map>
#include "core/resourcetypes.h"


class schemaGUI {
public:
    /* called on window size change, and initial startup */
    void updateScreenResolution(unsigned xscr, unsigned yscr);
    /* add new schema to schema cache */
    void addNewSchema(std::string name, LoadedSchema s);
    /* remove schema from schema cache */
    void removeSchema(std::string name);
    /* empty schema cache */
    void clearSchemas();
    /* activate schema from cache */
    void activateSchema(std::string name);
    /* draw active schema */
    void drawGUI();
    /* returns true if gui is in control of HID */
    bool inControl();
    /* takes control */
    void takeControl();
    /* lose control */
    void loseControl();
    /* handle control of window */
    void handleControl(GLFWwindow *windowptr);
private:
    unsigned xscr, yscr;
    bool control = false;
    std::string activeschema;
    std::map<std::string, LoadedSchema> schemas;

    /* draw text to framebuffer */
    void drawTextToFB(float scl, const char *t, float xposc, float yposc, float brightness);
};

#endif // SCHEMAGUI_H

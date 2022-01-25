#ifndef SCHEMAGUI_H
#define SCHEMAGUI_H
#include <string>
#include <vector>
#include <map>

struct Text {
    Text(float x, float y, float scl) : x(x), y(y), scl(scl) {}
    Text() {}
    float x = 0.0f;
    float y = 0.0f;
    float scl = 1.0f;
    std::string data;
    std::string action;
};

struct LoadedSchema {
    std::vector<Text> text;
};


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
    /*draw active schema */
    void drawGUI();

private:
    unsigned xscr, yscr;
    std::string activeschema;
    std::map<std::string, LoadedSchema> schemas;

    /* draw text to framebuffer */
    void drawTextToFB(float scl, const char *t, float xposc, float yposc, float brightness);
};

#endif // SCHEMAGUI_H

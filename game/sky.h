#ifndef SKY_H
#define SKY_H

#include "core/entity.h"

class Sky : public RenderedEntity {
public:
    Sky();

    void drawSelf(void);
};

#endif // SKY_H

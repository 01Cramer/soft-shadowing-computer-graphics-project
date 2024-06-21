#ifndef SHADOW_MAPPING_TECHNIQUE_H
#define SHADOW_MAPPING_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

class ShadowMappingTechnique : public Technique
{
 public:

     ShadowMappingTechnique() {};

    virtual bool Init();

    void SetWVP(const Matrix4f& WVP);

 private:

    GLuint m_WVPLoc = 0xffffffff;
};

#endif

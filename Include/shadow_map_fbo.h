#ifndef SHADOW_MAP_FBO_H
#define SHADOW_MAP_FBO_H

#include <GL/glew.h>

class ShadowMapFBO
{
public:
    ShadowMapFBO();
    ~ShadowMapFBO();

    bool Init(unsigned int Width, unsigned int Height, bool ForPCF = false);

    void BindForWriting();
    void BindForReadingDSA(GLenum TextureUnit);

private:
    bool InitDSA(unsigned int Width, unsigned int Height, bool ForPCF = false);

    unsigned int m_width = 0;
    unsigned int m_height = 0;
    GLuint m_fbo;
    GLuint m_shadowMap;
};
#endif  

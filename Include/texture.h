#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include <GL/glew.h>

class Texture
{
public:

    Texture(GLenum TextureTarget, const std::string& FileName);
    Texture(GLenum TextureTarget);

    // Should be called once to load the texture
    bool Load();
    // Must be called at least once for the specific texture unit
    void BindInternalDSA(GLenum TextureUnit);

private:
    void LoadInternalDSA(const void* pImageData);

    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageBPP = 0;
}; 
#endif

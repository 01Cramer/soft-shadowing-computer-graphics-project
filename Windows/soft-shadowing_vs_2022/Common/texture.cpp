#include <iostream>
#include <math.h>
#include "util.h"
#include "texture.h"
#include "3rdparty/stb_image.h"
#include "3rdparty/stb_image_write.h"
#include<cstdint>

Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
    m_textureTarget = TextureTarget;
    m_fileName      = FileName;
}


Texture::Texture(GLenum TextureTarget)
{
    m_textureTarget = TextureTarget;
}

bool Texture::Load()
{
    stbi_set_flip_vertically_on_load(1);

    unsigned char* pImageData = stbi_load(m_fileName.c_str(), &m_imageWidth, &m_imageHeight, &m_imageBPP, 0);

    if (!pImageData) {
        exit(0);
    }
    LoadInternalDSA(pImageData);

    return true;
}

void Texture::LoadInternalDSA(const void* pImageData)
{
    glCreateTextures(m_textureTarget, 1, &m_textureObj);

    int Levels = std::min(5, (int)log2f((float)std::max(m_imageWidth, m_imageHeight)));

    if (m_textureTarget == GL_TEXTURE_2D) {
        switch (m_imageBPP) {
        case 1:
            glTextureStorage2D(m_textureObj, Levels, GL_R8, m_imageWidth, m_imageHeight);
            glTextureSubImage2D(m_textureObj, 0, 0, 0, m_imageWidth, m_imageHeight, GL_RED, GL_UNSIGNED_BYTE, pImageData);
            break;

        case 2:
            glTextureStorage2D(m_textureObj, Levels, GL_RG8, m_imageWidth, m_imageHeight);
            glTextureSubImage2D(m_textureObj, 0, 0, 0, m_imageWidth, m_imageHeight, GL_RG, GL_UNSIGNED_BYTE, pImageData);
            break;

        case 3:
            glTextureStorage2D(m_textureObj, Levels, GL_RGB8, m_imageWidth, m_imageHeight);
            glTextureSubImage2D(m_textureObj, 0, 0, 0, m_imageWidth, m_imageHeight, GL_RGB, GL_UNSIGNED_BYTE, pImageData);
            break;

        case 4:
            glTextureStorage2D(m_textureObj, Levels, GL_RGBA8, m_imageWidth, m_imageHeight);
            glTextureSubImage2D(m_textureObj, 0, 0, 0, m_imageWidth, m_imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, pImageData);
            break;
        }
    }
    else {
        exit(1);
    }

    glTextureParameteri(m_textureObj, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_textureObj, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameterf(m_textureObj, GL_TEXTURE_BASE_LEVEL, 0);
    glTextureParameteri(m_textureObj, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_textureObj, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateTextureMipmap(m_textureObj);
}

void Texture::BindInternalDSA(GLenum TextureUnit)
{
    glBindTextureUnit(TextureUnit - GL_TEXTURE0, m_textureObj);
}

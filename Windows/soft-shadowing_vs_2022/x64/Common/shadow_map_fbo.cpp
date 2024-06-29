#include <stdio.h>

#include "shadow_map_fbo.h"
#include "util.h"
#include "engine_common.h"

ShadowMapFBO::ShadowMapFBO()
{
    m_fbo = 0;
    m_shadowMap = 0;
}

ShadowMapFBO::~ShadowMapFBO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_shadowMap != 0) {
        glDeleteTextures(1, &m_shadowMap);
    }
}


bool ShadowMapFBO::Init(unsigned int Width, unsigned int Height, bool ForPCF)
{
    bool ret = InitDSA(Width, Height, ForPCF);
    

    return ret;
}

bool ShadowMapFBO::InitDSA(unsigned int Width, unsigned int Height, bool ForPCF)
{
    m_width = Width;
    m_height = Height;

    // Create the FBO
    glCreateFramebuffers(1, &m_fbo);

    // Create the depth buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &m_shadowMap);

    int Levels = 1;
    glTextureStorage2D(m_shadowMap, Levels, GL_DEPTH_COMPONENT32, Width, Height);

    GLint FilterType = ForPCF ? GL_LINEAR : GL_NEAREST;

    glTextureParameteri(m_shadowMap, GL_TEXTURE_MIN_FILTER, FilterType);
    glTextureParameteri(m_shadowMap, GL_TEXTURE_MAG_FILTER, FilterType);

    glTextureParameteri(m_shadowMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(m_shadowMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTextureParameterfv(m_shadowMap, GL_TEXTURE_BORDER_COLOR, borderColor);

    glNamedFramebufferTexture(m_fbo, GL_DEPTH_ATTACHMENT, m_shadowMap, 0);

    // Disable read/writes to the color buffer
    glNamedFramebufferReadBuffer(m_fbo, GL_NONE);
    glNamedFramebufferDrawBuffer(m_fbo, GL_NONE);

    GLenum Status = glCheckNamedFramebufferStatus(m_fbo, GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    return true;
}


void ShadowMapFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);  // set the width/height of the shadow map!
}

void ShadowMapFBO::BindForReadingDSA(GLenum TextureUnit)
{
    glBindTextureUnit(TextureUnit - GL_TEXTURE0, m_shadowMap);
}

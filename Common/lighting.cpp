#include "lighting.h"

#define FAIL_ON_MISSING_LOC

void DirectionalLight::CalcLocalDirection(const WorldTrans& worldTransform)
{
    LocalDirection = worldTransform.WorldDirToLocalDir(WorldDirection);
}

LightingTechnique::LightingTechnique()
{
}

bool LightingTechnique::Init(int SubTech)
{
    if (!Technique::Init()) {
        return false;
    }

    m_subTech = SubTech;

    if (!AddShader(GL_VERTEX_SHADER, "../Common/Shaders/lighting_new.vs")) {
        return false;
    }


    if (!AddShader(GL_FRAGMENT_SHADER, "../Common/Shaders/lighting_new.fs")) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    return InitCommon();
}

bool LightingTechnique::InitCommon()
{
    WVPLoc = GetUniformLocation("gWVP");
    WorldMatrixLoc = GetUniformLocation("gWorld");
    LightWVPLoc = GetUniformLocation("gLightWVP");
    samplerLoc = GetUniformLocation("gSampler");
    shadowMapLoc = GetUniformLocation("gShadowMap");
    shadowCubeMapLoc = GetUniformLocation("gShadowCubeMap");
    shadowMapWidthLoc = GetUniformLocation("gShadowMapWidth");
    shadowMapHeightLoc = GetUniformLocation("gShadowMapHeight");
    shadowMapFilterSizeLoc = GetUniformLocation("gShadowMapFilterSize");
    samplerSpecularExponentLoc = GetUniformLocation("gSamplerSpecularExponent");
    materialLoc.AmbientColor = GetUniformLocation("gMaterial.AmbientColor");
    materialLoc.DiffuseColor = GetUniformLocation("gMaterial.DiffuseColor");
    materialLoc.SpecularColor = GetUniformLocation("gMaterial.SpecularColor");
    dirLightLoc.Color = GetUniformLocation("gDirectionalLight.Base.Color");
    dirLightLoc.AmbientIntensity = GetUniformLocation("gDirectionalLight.Base.AmbientIntensity");
    dirLightLoc.Direction = GetUniformLocation("gDirectionalLight.Direction");
    dirLightLoc.DiffuseIntensity = GetUniformLocation("gDirectionalLight.Base.DiffuseIntensity");
    CameraLocalPosLoc = GetUniformLocation("gCameraLocalPos");
    CameraWorldPosLoc = GetUniformLocation("gCameraWorldPos");
    NumPointLightsLoc = GetUniformLocation("gNumPointLights");
    NumSpotLightsLoc = GetUniformLocation("gNumSpotLights");
    ColorModLocation = GetUniformLocation("gColorMod");
    ColorAddLocation = GetUniformLocation("gColorAdd");
    EnableSpecularExponent = GetUniformLocation("gEnableSpecularExponent");


    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(SpotLightsLocation) ; i++) {
        char Name[128];
        memset(Name, 0, sizeof(Name));
        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.Color", i);
        SpotLightsLocation[i].Color = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.AmbientIntensity", i);
        SpotLightsLocation[i].AmbientIntensity = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.LocalPos", i);
        SpotLightsLocation[i].Position = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Direction", i);
        SpotLightsLocation[i].Direction = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Cutoff", i);
        SpotLightsLocation[i].Cutoff = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.DiffuseIntensity", i);
        SpotLightsLocation[i].DiffuseIntensity = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Constant", i);
        SpotLightsLocation[i].Atten.Constant = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Linear", i);
        SpotLightsLocation[i].Atten.Linear = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Exp", i);
        SpotLightsLocation[i].Atten.Exp = GetUniformLocation(Name);

        if (SpotLightsLocation[i].Color == INVALID_UNIFORM_LOCATION ||
            SpotLightsLocation[i].AmbientIntensity == INVALID_UNIFORM_LOCATION ||
            SpotLightsLocation[i].Position == INVALID_UNIFORM_LOCATION ||
            SpotLightsLocation[i].Direction == INVALID_UNIFORM_LOCATION ||
            SpotLightsLocation[i].Cutoff == INVALID_UNIFORM_LOCATION ||
            SpotLightsLocation[i].DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
            SpotLightsLocation[i].Atten.Constant == INVALID_UNIFORM_LOCATION ||
            SpotLightsLocation[i].Atten.Linear == INVALID_UNIFORM_LOCATION ||
            SpotLightsLocation[i].Atten.Exp == INVALID_UNIFORM_LOCATION) {
#ifdef FAIL_ON_MISSING_LOC
            return false;
#endif
        }
    }

    return true;
}


void LightingTechnique::SetWVP(const Matrix4f& WVP)
{
    glUniformMatrix4fv(WVPLoc, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

void LightingTechnique::SetLightWVP(const Matrix4f& LightWVP)
{
    glUniformMatrix4fv(LightWVPLoc, 1, GL_TRUE, (const GLfloat*)LightWVP.m);
}


void LightingTechnique::SetTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(samplerLoc, TextureUnit);
}


void LightingTechnique::SetShadowMapSize(unsigned int Width, unsigned int Height)
{
    glUniform1i(shadowMapWidthLoc, Width);
    glUniform1i(shadowMapHeightLoc, Height);
}


void LightingTechnique::SetShadowMapFilterSize(unsigned int Size)
{
    glUniform1i(shadowMapFilterSizeLoc, Size);
}


void LightingTechnique::SetShadowMapTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(shadowMapLoc, TextureUnit);
}


void LightingTechnique::SetDirectionalLight(const DirectionalLight& DirLight, bool WithDir)
{
    glUniform3f(dirLightLoc.Color, DirLight.Color.x, DirLight.Color.y, DirLight.Color.z);
    glUniform1f(dirLightLoc.AmbientIntensity, DirLight.AmbientIntensity);
    glUniform1f(dirLightLoc.DiffuseIntensity, DirLight.DiffuseIntensity);

    if (WithDir) {
        UpdateDirLightDirection(DirLight);
    }
}


void LightingTechnique::UpdateDirLightDirection(const DirectionalLight& DirLight)
{
    Vector3f LocalDirection = DirLight.GetLocalDirection();

    LocalDirection.Normalize();

    glUniform3f(dirLightLoc.Direction, LocalDirection.x, LocalDirection.y, LocalDirection.z);

    Vector3f WorldDirection = DirLight.WorldDirection;
    WorldDirection.Normalize();
}


void LightingTechnique::SetCameraLocalPos(const Vector3f& CameraLocalPos)
{
    glUniform3f(CameraLocalPosLoc, CameraLocalPos.x, CameraLocalPos.y, CameraLocalPos.z);
}


void LightingTechnique::SetMaterial(const Material& material)
{
    glUniform3f(materialLoc.AmbientColor, material.AmbientColor.r, material.AmbientColor.g, material.AmbientColor.b);
    glUniform3f(materialLoc.DiffuseColor, material.DiffuseColor.r, material.DiffuseColor.g, material.DiffuseColor.b);
    glUniform3f(materialLoc.SpecularColor, material.SpecularColor.r, material.SpecularColor.g, material.SpecularColor.b);
}


void LightingTechnique::ControlSpecularExponent(bool IsEnabled)
{
    if (IsEnabled) {
        glUniform1i(EnableSpecularExponent, 1);
    } else {
        glUniform1i(EnableSpecularExponent, 0);
    }
}

#ifndef NEW_LIGHTING_TECHNIQUE_H
#define NEW_LIGHTING_TECHNIQUE_H

#include "technique.h"
#include "math_3d.h"
#include "material.h"
#include "world_transform.h"
#include "common.h"

class BaseLight
{
public:
    Vector3f Color = Vector3f(1.0f, 1.0f, 1.0f);
    float AmbientIntensity = 0.0f;
    float DiffuseIntensity = 0.0f;
};


class DirectionalLight : public BaseLight
{
public:
    Vector3f WorldDirection = Vector3f(0.0f, 0.0f, 0.0f);

    void CalcLocalDirection(const WorldTrans& worldTransform);

    const Vector3f& GetLocalDirection() const { return LocalDirection; }

private:
    Vector3f LocalDirection = Vector3f(0.0f, 0.0f, 0.0f);
};

struct LightAttenuation
{
    float Constant = 1.0f;
    float Linear = 0.0f;
    float Exp = 0.0f;
};


class PointLight: public BaseLight
{
public:
    Vector3f WorldPosition = Vector3f(0.0f, 0.0f, 0.0f);
    LightAttenuation Attenuation;

    const Vector3f& GetLocalPosition() const { return LocalPosition; }

private:
    Vector3f LocalPosition = Vector3f(0.0f, 0.0f, 0.0f);
};

class SpotLight : public PointLight
{
public:
    Vector3f WorldDirection = Vector3f(0.0f, 0.0f, 0.0f);
    float Cutoff = 0.0f;

    const Vector3f& GetLocalDirection() const { return LocalDirection; }

private:
    Vector3f LocalDirection = Vector3f(0.0f, 0.0f, 0.0f);
};


struct PBRLight {
    Vector4f PosDir;
    Vector3f Intensity;
};


class LightingTechnique : public Technique, public IRenderCallbacks
{
public:

    static const unsigned int MAX_POINT_LIGHTS = 2;
    static const unsigned int MAX_SPOT_LIGHTS = 2;

    static const int SUBTECH_DEFAULT = 0;
    static const int SUBTECH_PASSTHRU_GS = 1;
    static const int SUBTECH_WIREFRAME_ON_MESH = 2;

    LightingTechnique();

    virtual bool Init(int SubTech = SUBTECH_DEFAULT);

    void SetWVP(const Matrix4f& WVP);
    void SetLightWVP(const Matrix4f& LightWVP);
    void SetTextureUnit(unsigned int TextureUnit);
    void SetShadowMapTextureUnit(unsigned int TextureUnit);
    void SetShadowMapSize(unsigned int Width, unsigned int Height);
    void SetShadowMapFilterSize(unsigned int Size);
    void SetDirectionalLight(const DirectionalLight& DirLight, bool WithDir = true);
    void UpdateDirLightDirection(const DirectionalLight& DirLight);
    void UpdatePointLight(unsigned int Index, const PointLight& Light);
    void UpdateSpotLight(unsigned int Index, const SpotLight& Light);
    void SetCameraLocalPos(const Vector3f& CameraLocalPos);
    virtual void SetMaterial(const Material& material);
    virtual void ControlSpecularExponent(bool IsEnabled);

protected:

    bool InitCommon();

private:

    int m_subTech = SUBTECH_DEFAULT;
    #define INVALID_UNIFORM_LOCATION 0xffffffff

    GLuint WVPLoc = INVALID_UNIFORM_LOCATION;
    GLuint WorldMatrixLoc = INVALID_UNIFORM_LOCATION;
    GLuint ViewportMatrixLoc = INVALID_UNIFORM_LOCATION;
    GLuint LightWVPLoc = INVALID_UNIFORM_LOCATION;
    GLuint samplerLoc = INVALID_UNIFORM_LOCATION;
    GLuint shadowMapLoc = INVALID_UNIFORM_LOCATION;
    GLuint shadowCubeMapLoc = INVALID_UNIFORM_LOCATION;
    GLuint shadowMapWidthLoc = INVALID_UNIFORM_LOCATION;
    GLuint shadowMapHeightLoc = INVALID_UNIFORM_LOCATION;
    GLuint shadowMapFilterSizeLoc = INVALID_UNIFORM_LOCATION;
    GLuint samplerSpecularExponentLoc = INVALID_UNIFORM_LOCATION;
    GLuint CameraLocalPosLoc = INVALID_UNIFORM_LOCATION;
    GLuint CameraWorldPosLoc = INVALID_UNIFORM_LOCATION;
    GLuint NumPointLightsLoc = INVALID_UNIFORM_LOCATION;
    GLuint NumSpotLightsLoc = INVALID_UNIFORM_LOCATION;
    GLuint ColorModLocation = INVALID_UNIFORM_LOCATION;
    GLuint ColorAddLocation = INVALID_UNIFORM_LOCATION;
    GLuint EnableSpecularExponent = INVALID_UNIFORM_LOCATION;


    struct {
        GLuint AmbientColor;
        GLuint DiffuseColor;
        GLuint SpecularColor;
    } materialLoc;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint Direction;
        GLuint DiffuseIntensity;
    } dirLightLoc;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint LocalPos;
        GLuint WorldPos;
        GLuint DiffuseIntensity;

        struct
        {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } PointLightsLocation[MAX_POINT_LIGHTS];

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        GLuint Direction;
        GLuint Cutoff;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } SpotLightsLocation[MAX_SPOT_LIGHTS];


};


#endif

#version 420

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 LocalPos0;
in vec3 WorldPos0;
in vec4 LightSpacePos0; // required only for shadow mapping


out vec4 FragColor;

struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;
    vec3 Direction;
};

struct Attenuation
{
    float Constant;
    float Linear;
    float Exp;
};

struct PointLight
{
    BaseLight Base;
    vec3 LocalPos; // used for lighting calculations
    vec3 WorldPos; // used for point light shadow mapping
    Attenuation Atten;
};

struct SpotLight
{
    PointLight Base;
    vec3 Direction;
    float Cutoff;
};

struct Material
{
    vec3 AmbientColor;
    vec3 DiffuseColor;
    vec3 SpecularColor;
};

uniform DirectionalLight gDirectionalLight;
uniform int gNumPointLights;
uniform PointLight gPointLights[MAX_POINT_LIGHTS];
uniform int gNumSpotLights;
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];
uniform Material gMaterial;
layout(binding = 0) uniform sampler2D gSampler;
layout(binding = 1) uniform sampler2D gSamplerSpecularExponent;
layout(binding = 2) uniform sampler2D gShadowMap;        // required only for shadow mapping (spot/directional light)
layout(binding = 3) uniform samplerCube gShadowCubeMap;  // required only for shadow mapping (point light)
uniform int gShadowMapWidth = 0;
uniform int gShadowMapHeight = 0;
uniform int gShadowMapFilterSize = 0;
uniform vec3 gCameraLocalPos;


float CalcRimLightFactor(vec3 PixelToCamera, vec3 Normal)
{
    float RimFactor = dot(PixelToCamera, Normal);
    RimFactor = 1.0 - RimFactor;
    RimFactor = max(0.0, RimFactor);
    RimFactor = pow(RimFactor, 2);
    return RimFactor;
}


float CalcShadowFactorPointLight(vec3 LightToPixel)
{
    float Distance = length(LightToPixel);

    LightToPixel.y = -LightToPixel.y;

    float SampledDistance = texture(gShadowCubeMap, LightToPixel).r;

    float bias = 0.015;

    if (SampledDistance + bias < Distance)
        return 0.25;
    else
        return 1.0;
}


vec3 CalcShadowCoords()
{
    vec3 ProjCoords = LightSpacePos0.xyz / LightSpacePos0.w;
    vec3 ShadowCoords = ProjCoords * 0.5 + vec3(0.5);
    return ShadowCoords;
}


float CalcShadowFactorBasic(vec3 LightDirection, vec3 Normal)
{
    vec3 ShadowCoords = CalcShadowCoords();

    float Depth = texture(gShadowMap, ShadowCoords.xy).x;

    float DiffuseFactor = dot(Normal, -LightDirection);

    float bias = mix(0.001, 0.0, DiffuseFactor);

    if (Depth + bias < ShadowCoords.z)
        return 0.05;
    else
        return 1.0;
}


float CalcShadowFactorPCF(vec3 LightDirection, vec3 Normal)
{
    if (gShadowMapWidth == 0 || gShadowMapHeight == 0) {
        return 1.0;
    }

    vec3 ProjCoords = LightSpacePos0.xyz / LightSpacePos0.w;
    vec3 ShadowCoords = ProjCoords * 0.5 + vec3(0.5);

    float DiffuseFactor = dot(Normal, -LightDirection);
    float bias = mix(0.001, 0.0, DiffuseFactor);

    float TexelWidth = 1.0 / gShadowMapWidth;
    float TexelHeight = 1.0 / gShadowMapHeight;

    vec2 TexelSize = vec2(TexelWidth, TexelHeight);

    float ShadowSum = 0.0;

    int HalfFilterSize = gShadowMapFilterSize / 2;

    for (int y = -HalfFilterSize ; y < -HalfFilterSize + gShadowMapFilterSize ; y++) {
        for (int x = -HalfFilterSize ; x < -HalfFilterSize + gShadowMapFilterSize ; x++) {
            vec2 Offset = vec2(x, y) * TexelSize;
            float Depth = texture(gShadowMap, ShadowCoords.xy + Offset).x;

            if (Depth + bias < ShadowCoords.z) {
                ShadowSum += 0.0;
            } else {
                ShadowSum += 1.0;
            }
       }
   }

   float FinalShadowFactor = ShadowSum / float(pow(gShadowMapFilterSize, 2));

   return FinalShadowFactor;
}


float CalcShadowFactor(vec3 LightDirection, vec3 Normal)
{
    float ShadowFactor = 0.0;

    if (gShadowMapFilterSize > 0){
        ShadowFactor = CalcShadowFactorPCF(LightDirection, Normal);
    } else {
        ShadowFactor = CalcShadowFactorBasic(LightDirection, Normal);
    }

    return ShadowFactor;
}

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal,
                       float ShadowFactor)
{
    vec4 AmbientColor = vec4(Light.Color, 1.0f) *
                        Light.AmbientIntensity *
                        vec4(gMaterial.AmbientColor, 1.0f);

    float DiffuseFactor = dot(Normal, -LightDirection);

    vec4 DiffuseColor = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);
    vec4 RimColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0) {
        DiffuseColor = vec4(Light.Color, 1.0f) *
                       Light.DiffuseIntensity *
                       vec4(gMaterial.DiffuseColor, 1.0f) *
                       DiffuseFactor;

        vec3 PixelToCamera = normalize(gCameraLocalPos - LocalPos0);
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = dot(PixelToCamera, LightReflect);

        if (SpecularFactor > 0) {
            float SpecularExponent = 128.0;

            SpecularFactor = pow(SpecularFactor, SpecularExponent);
            SpecularColor = vec4(Light.Color, 1.0f) *
                            Light.DiffuseIntensity * // using the diffuse intensity for diffuse/specular
                            vec4(gMaterial.SpecularColor, 1.0f) *
                            SpecularFactor;
        }
    }

    return (AmbientColor + ShadowFactor * (DiffuseColor + SpecularColor + RimColor));
}


vec4 CalcDirectionalLight(vec3 Normal)
{
    float ShadowFactor = CalcShadowFactor(gDirectionalLight.Direction, Normal);
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal, ShadowFactor);
}


vec4 CalcPointLight(PointLight l, vec3 Normal, bool IsSpot)
{
    vec3 LightWorldDir = WorldPos0 - l.WorldPos;
    float ShadowFactor = 0.0;
    
    if (IsSpot) {
        ShadowFactor = CalcShadowFactor(LightWorldDir, Normal);
    } else {
        ShadowFactor = CalcShadowFactorPointLight(LightWorldDir);
    }

    vec3 LightLocalDir = LocalPos0 - l.LocalPos;
    float Distance = length(LightLocalDir);
    LightLocalDir = normalize(LightLocalDir);
    vec4 Color = CalcLightInternal(l.Base, LightLocalDir, Normal, ShadowFactor);
    float Attenuation =  l.Atten.Constant +
                         l.Atten.Linear * Distance +
                         l.Atten.Exp * Distance * Distance;

    return Color / Attenuation;
}


vec4 CalcSpotLight(SpotLight l, vec3 Normal)
{
    vec3 LightToPixel = normalize(LocalPos0 - l.Base.LocalPos);
    float SpotFactor = dot(LightToPixel, l.Direction);

    if (SpotFactor > l.Cutoff) {
        vec4 Color = CalcPointLight(l.Base, Normal, true);
        float SpotLightIntensity = (1.0 - (1.0 - SpotFactor)/(1.0 - l.Cutoff));
        return Color * SpotLightIntensity;
    }
    else {
        return vec4(0,0,0,0);
    }
}

vec4 CalcPhongLighting()
{
    vec3 Normal = normalize(Normal0);
    vec4 TotalLight = CalcDirectionalLight(Normal);

    for (int i = 0 ;i < gNumPointLights ;i++) {
        TotalLight += CalcPointLight(gPointLights[i], Normal, false);
    }

    for (int i = 0 ;i < gNumSpotLights ;i++) {
        TotalLight += CalcSpotLight(gSpotLights[i], Normal);
    }

    vec4 TempColor = texture(gSampler, TexCoord0.xy) * TotalLight;

    vec4 FinalColor =  TempColor;

    return FinalColor;
}


void main()
{
    FragColor = CalcPhongLighting();
}

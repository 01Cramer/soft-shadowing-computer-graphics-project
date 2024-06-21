#ifndef WORLD_TRANSFORM_H
#define WORLD_TRANSFORM_H

#include "math_3d.h"

class WorldTrans {
 public:
    WorldTrans() {}
    void SetPosition(float x, float y, float z);
    void SetScale(float scale);

    Vector3f WorldPosToLocalPos(const Vector3f& WorldPos) const;
    Vector3f WorldDirToLocalDir(const Vector3f& WorldDir) const;
    
    Matrix4f GetMatrix() const;
    Matrix4f GetReversedTranslationMatrix() const;
    Matrix4f GetReversedRotationMatrix() const;

 private:
    float    m_scale    = 1.0f;
    Vector3f m_rotation = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f m_pos      = Vector3f(0.0f, 0.0f, 0.0f);
};

#endif

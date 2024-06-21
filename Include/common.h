#ifndef MESH_COMMON_H
#define MESH_COMMON_H

class IRenderCallbacks
{
public:
    virtual void DrawStartCB(unsigned int DrawIndex) {}

    virtual void ControlSpecularExponent(bool IsEnabled) {}

    virtual void SetMaterial(const Material& material) {}

    virtual void DisableDiffuseTexture() {}
};


class MeshCommon {

 public:
    MeshCommon() {}
    void SetRotation(float x, float y, float z);
    void SetPosition(float x, float y, float z);
    void SetScale(float scale);
    void SetPosition(const Vector3f& pos) { SetPosition(pos.x, pos.y, pos.z); }

    WorldTrans& GetWorldTransform() { return m_worldTransform; }

    Matrix4f GetWorldMatrix() { return m_worldTransform.GetMatrix(); }

 protected:
    WorldTrans m_worldTransform;
};

inline void MeshCommon::SetPosition(float x, float y, float z)
{
    m_worldTransform.SetPosition(x, y, z);
}

inline void MeshCommon::SetScale(float scale)
{
  m_worldTransform.SetScale(scale);
}

#endif

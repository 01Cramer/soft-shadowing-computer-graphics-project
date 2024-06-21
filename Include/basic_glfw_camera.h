#ifndef BASIC_GLFW_CAMERA_H
#define BASIC_GLFW_CAMERA_H

#include "math_3d.h"


class BasicCamera
{
public:

    BasicCamera() {}

    BasicCamera(const PersProjInfo& persProjInfo, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);

    void InitCamera(const PersProjInfo& persProjInfo, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);

    void SetPosition(float x, float y, float z);

    void SetPosition(const Vector3f& pos);

    void SetTarget(float x, float y, float z);

    void SetTarget(const Vector3f& target);

    bool OnKeyboard(int key);

    void OnMouse(int x, int y);

    void OnRender();

    Matrix4f GetMatrix() const;

    const Vector3f& GetPos() const { return m_pos; }

    const Matrix4f& GetProjectionMat() const { return m_projection; }

private:

    void InitInternal();
    void Update();

    Vector3f m_pos;
    Vector3f m_target;
    Vector3f m_up;

    float m_speed = 5.0f;
    int m_windowWidth;
    int m_windowHeight;

    float m_AngleH;
    float m_AngleV;

    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

    Vector2i m_mousePos;
    
    PersProjInfo m_persProjInfo;
    Matrix4f m_projection;
};

#endif

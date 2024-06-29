#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include "basic_glfw_camera.h"

static int MARGIN = 40;
static float EDGE_STEP = 0.5f;

BasicCamera::BasicCamera(const PersProjInfo& persProjInfo, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
    InitCamera(persProjInfo, Pos, Target, Up);
}

void BasicCamera::InitCamera(const PersProjInfo& persProjInfo, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
    m_persProjInfo = persProjInfo;
    m_projection.InitPersProjTransform(persProjInfo);
    m_windowWidth = (int)persProjInfo.Width;
    m_windowHeight = (int)persProjInfo.Height;
    m_pos = Pos;

    m_target = Target;
    m_target.Normalize();

    m_up = Up;
    m_up.Normalize();

    InitInternal();
}

void BasicCamera::InitInternal()
{
    Vector3f HTarget(m_target.x, 0.0, m_target.z);
    HTarget.Normalize();

    m_AngleH = -ToDegree(atan2(m_target.z, m_target.x));

    m_AngleV = -ToDegree(asin(m_target.y));

    m_OnUpperEdge = false;
    m_OnLowerEdge = false;
    m_OnLeftEdge  = false;
    m_OnRightEdge = false;
    m_mousePos.x  = m_windowWidth / 2;
    m_mousePos.y  = m_windowHeight / 2;
}



void BasicCamera::SetPosition(float x, float y, float z)
{
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
}


void BasicCamera::SetPosition(const Vector3f& pos)
{
    SetPosition(pos.x, pos.y, pos.z);
}


void BasicCamera::SetTarget(float x, float y, float z)
{
    m_target.x = x;
    m_target.y = y;
    m_target.z = z;
}


void BasicCamera::SetTarget(const Vector3f& target)
{
    SetTarget(target.x, target.y, target.z);
}


bool BasicCamera::OnKeyboard(int Key)
{
    bool CameraChangedPos = false;

    switch (Key) {

    case GLFW_KEY_W:
        m_pos += (m_target * m_speed);
        CameraChangedPos = true;
        break;

    case GLFW_KEY_S:
        m_pos -= (m_target * m_speed);
        CameraChangedPos = true;
        break;

    case GLFW_KEY_A:
        {
            Vector3f Left = m_target.Cross(m_up);
            Left.Normalize();
            Left *= m_speed;
            m_pos += Left;
            CameraChangedPos = true;
        }
        break;

    case GLFW_KEY_D:
        {
            Vector3f Right = m_up.Cross(m_target);
            Right.Normalize();
            Right *= m_speed;
            m_pos += Right;
            CameraChangedPos = true;
        }
        break;
    }
    return CameraChangedPos;
}


void BasicCamera::OnMouse(int x, int y)
{
    int DeltaX = x - m_mousePos.x;
    int DeltaY = y - m_mousePos.y;

    m_mousePos.x = x;
    m_mousePos.y = y;

    m_AngleH += (float)DeltaX / 20.0f;
    m_AngleV += (float)DeltaY / 20.0f;

    if (x <= MARGIN) {
        m_OnLeftEdge = true;
	    m_OnRightEdge = false;
    }  else if (x >= (m_windowWidth - MARGIN)) {
        m_OnRightEdge = true;
	    m_OnLeftEdge = false;
    } else {
        m_OnLeftEdge = false;
        m_OnRightEdge = false;
    }

    if (y <= MARGIN) {
        m_OnUpperEdge = true;
        m_OnLowerEdge = false;
    } else if (y >= (m_windowHeight - MARGIN)) {
        m_OnLowerEdge = true;
        m_OnUpperEdge = false;
    } else {
        m_OnUpperEdge = false;
        m_OnLowerEdge = false;
    }

    Update();
}


void BasicCamera::OnRender()
{
    bool ShouldUpdate = false;

    if (m_OnLeftEdge) {
        m_AngleH -= EDGE_STEP;
        ShouldUpdate = true;
    }
    else if (m_OnRightEdge) {
        m_AngleH += EDGE_STEP;
        ShouldUpdate = true;
    }

    if (m_OnUpperEdge) {
        if (m_AngleV > -90.0f) {
            m_AngleV -= EDGE_STEP;
            ShouldUpdate = true;
        }
    }
    else if (m_OnLowerEdge) {
        if (m_AngleV < 90.0f) {
           m_AngleV += EDGE_STEP;
           ShouldUpdate = true;
        }
    }

    if (ShouldUpdate) {
        Update();
    }
}

void BasicCamera::Update()
{
    Vector3f Yaxis(0.0f, 1.0f, 0.0f);

    // Rotate the view vector by the horizontal angle around the vertical axis
    Vector3f View(1.0f, 0.0f, 0.0f);
    View.Rotate(m_AngleH, Yaxis);
    View.Normalize();

    // Rotate the view vector by the vertical angle around the horizontal axis
    Vector3f U = Yaxis.Cross(View);
    U.Normalize();
    View.Rotate(m_AngleV, U);

    m_target = View;
    m_target.Normalize();

    m_up = m_target.Cross(U);
    m_up.Normalize();
}



Matrix4f BasicCamera::GetMatrix() const
{
    Matrix4f CameraTransformation;
    CameraTransformation.InitCameraTransform(m_pos, m_target, m_up);

    return CameraTransformation;
}

#ifndef MATH_3D_H
#define MATH_3D_H

#include<cmath>
#include <stdio.h>
#include <cfloat>

#include "util.h"
#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>

#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))


struct Vector2i
{
    int x;
    int y;
};

struct Vector2f
{
    union {
        float x;
        float u;
    };

    union {
        float y;
        float v;
    };

    Vector2f() {}

    Vector2f(float _x, float _y)
    {
        x = _x;
        y = _y;
    }
};

struct Vector4f;

struct Vector3f
{
    union {
        float x;
        float r;
    };

    union {
        float y;
        float g;
    };

    union {
        float z;
        float b;
    };

    Vector3f() {}

    Vector3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f(float f)
    {
        x = y = z = f;
    }

    Vector3f(const Vector4f& v);

    Vector3f& operator+=(const Vector3f& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;

        return *this;
    }

    Vector3f& operator-=(const Vector3f& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;

        return *this;
    }

    Vector3f& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }

    bool operator==(const Vector3f& r)
    {
        return ((x == r.x) && (y == r.y) && (z == r.z));
    }

    bool operator!=(const Vector3f& r)
    {
        return !(*this == r);
    }

    operator const float*() const
    {
        return &(x);
    }


    Vector3f Cross(const Vector3f& v) const;
    float Length() const
    {
        float len = sqrtf(x * x + y * y + z * z);
        return len;
    }

    Vector3f& Normalize();

    void Rotate(float Angle, const Vector3f& Axis);

    Vector3f Negate() const;
};


struct Vector4f
{
    float x;
    float y;
    float z;
    float w;

    Vector4f(){}
    Vector4f(const Vector3f& v, float _w)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = _w;
    }
};

inline Vector3f operator+(const Vector3f& l, const Vector3f& r)
{
    Vector3f Ret(l.x + r.x,
                 l.y + r.y,
                 l.z + r.z);

    return Ret;
}

inline Vector3f operator*(const Vector3f& l, float f)
{
    Vector3f Ret(l.x * f,
                 l.y * f,
                 l.z * f);

    return Ret;
}

inline Vector3f::Vector3f(const Vector4f& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
}


struct PersProjInfo
{
    float FOV;
    float Width;
    float Height;
    float zNear;
    float zFar;
};


struct OrthoProjInfo
{
    float r;        // right
    float l;        // left
    float b;        // bottom
    float t;        // top
    float n;        // z near
    float f;        // z far

    float Width;
    float Height;
};


struct Quaternion
{
    float x, y, z, w;

    Quaternion(float Angle, const Vector3f& V);

    Quaternion(float _x, float _y, float _z, float _w);

    Quaternion Conjugate() const;
};

Quaternion operator*(const Quaternion& l, const Quaternion& r);

Quaternion operator*(const Quaternion& q, const Vector3f& v);


class Matrix4f
{
public:
    float m[4][4];

    Matrix4f(){}

    // constructor from Assimp matrix
    Matrix4f(const aiMatrix4x4& AssimpMatrix)
    {
        m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
        m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
        m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
        m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;
    }

    inline Matrix4f operator*(const Matrix4f& Right) const
    {
        Matrix4f Ret;

        for (unsigned int i = 0 ; i < 4 ; i++) {
            for (unsigned int j = 0 ; j < 4 ; j++) {
                Ret.m[i][j] = m[i][0] * Right.m[0][j] +
                              m[i][1] * Right.m[1][j] +
                              m[i][2] * Right.m[2][j] +
                              m[i][3] * Right.m[3][j];
            }
        }

        return Ret;
    }

    Vector4f operator*(const Vector4f& v) const
    {
        Vector4f r;

        r.x = m[0][0]* v.x + m[0][1]* v.y + m[0][2]* v.z + m[0][3]* v.w;
        r.y = m[1][0]* v.x + m[1][1]* v.y + m[1][2]* v.z + m[1][3]* v.w;
        r.z = m[2][0]* v.x + m[2][1]* v.y + m[2][2]* v.z + m[2][3]* v.w;
        r.w = m[3][0]* v.x + m[3][1]* v.y + m[3][2]* v.z + m[3][3]* v.w;

        return r;
    }

    void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);

    void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
    void InitRotateTransformZYX(float RotateX, float RotateY, float RotateZ);

    void InitTranslationTransform(float x, float y, float z);
    void InitTranslationTransform(const Vector3f& Pos);

    void InitCameraTransform(const Vector3f& Target, const Vector3f& Up);

    void InitCameraTransform(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);

    void InitPersProjTransform(const PersProjInfo& p);

    void InitOrthoProjTransform(const OrthoProjInfo& p);

private:
    void InitRotationX(float RotateX);
    void InitRotationY(float RotateY);
    void InitRotationZ(float RotateZ);
};


class Matrix3f
{
public:
    float m[3][3];

    Matrix3f(){}

    // Initialize the matrix from the top left corner of the 4-by-4 matrix
    Matrix3f(const Matrix4f& a)
    {
        m[0][0] = a.m[0][0]; m[0][1] = a.m[0][1]; m[0][2] = a.m[0][2];
        m[1][0] = a.m[1][0]; m[1][1] = a.m[1][1]; m[1][2] = a.m[1][2];
        m[2][0] = a.m[2][0]; m[2][1] = a.m[2][1]; m[2][2] = a.m[2][2];
    }

    Vector3f operator*(const Vector3f& v) const
    {
        Vector3f r;

        r.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
        r.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
        r.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;

        return r;
    }

    Matrix3f Transpose() const
    {
        Matrix3f n;

        for (unsigned int i = 0 ; i < 3 ; i++) {
            for (unsigned int j = 0 ; j < 3 ; j++) {
                n.m[i][j] = m[j][i];
            }
        }

        return n;
    }
};

#endif

#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <GL/glew.h>

using namespace std;

bool ReadFile(const char* fileName, string& outFile);

inline bool GLCheckError()
{
    return glGetError() == GL_NO_ERROR;
}

string GetDirFromFilename(const string& Filename);

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define SNPRINTF _snprintf_s

#endif

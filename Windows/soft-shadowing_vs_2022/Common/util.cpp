#include <iostream>
#include <fstream>
#include <string>
#include<Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;

bool ReadFile(const char* pFileName, string& outFile)
{
    ifstream f(pFileName);

    if (!f.is_open()) {
        cerr << "Failed to open file: " << pFileName << endl;
        return false;
    }

    string line;
    while (getline(f, line)) {
        outFile.append(line);
        outFile.append("\n");
    }

    f.close();
    return true;
}

string GetDirFromFilename(const string& Filename)
{
    string::size_type SlashIndex;

    SlashIndex = Filename.find_last_of("\\");
    if (SlashIndex == string::npos) {
        SlashIndex = Filename.find_last_of("/");
    }
    if (SlashIndex == string::npos) {
        return ".";
    }
    else if (SlashIndex == 0) {
        return "/";
    }
    else {
        return Filename.substr(0, SlashIndex);
    }
}


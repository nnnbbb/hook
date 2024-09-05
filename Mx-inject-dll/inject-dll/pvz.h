#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <chrono>
#include <ctime>
#include <locale>
#include <codecvt>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class Pvz {
   public:
    Pvz();

    DWORD HookAddress;
    char HookCode[4]{};
    char UnHookCode[4]{};

    void init();

    BOOL OpenAutoGetSun();

    BOOL CloseAutoGetSun();

    BOOL AddSun(int SunNumber);

    BOOL OpenCardClickable();

    BOOL CloseCardClickable();

    void FullScreenPlant(int PlantId);

};

#include "pch.h"
#include "script.h"
#include "natives.h"
#include <fstream>
#include <string>
#include <ctime>

std::ofstream logFile;
const char* LOG_FILENAME = "PlayerInfoLog.txt";

// Helper function to get current timestamp
std::string GetTimestamp()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    localtime_s(&tstruct, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

void InitLogging()
{
    logFile.open(LOG_FILENAME, std::ios::out | std::ios::app);
    if (logFile.is_open())
    {
        logFile << "\n\n===== Player Info Logger Started - " << GetTimestamp() << " =====\n";
    }
}

void LogPlayerInfo(const std::string& info)
{
    if (logFile.is_open())
    {
        logFile << "[" << GetTimestamp() << "] " << info << "\n";
        logFile.flush();
    }
}

void CloseLogging()
{
    if (logFile.is_open())
    {
        logFile << "===== Player Info Logger Stopped - " << GetTimestamp() << " =====\n\n";
        logFile.close();
    }
}

namespace HUD {
    void BEGIN_TEXT_COMMAND_DISPLAY_TEXT(const char* label) {
        UI::BEGIN_TEXT_COMMAND_DISPLAY_TEXT(label);
    }

    void SET_COLOUR_OF_NEXT_TEXT_COMPONENT(int color) {
        UI::SET_COLOUR_OF_NEXT_TEXT_COMPONENT(color);
    }

    void ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(const char* text) {
        UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
    }

    void SET_TEXT_FONT(int font) {
        UI::SET_TEXT_FONT(font);
    }

    void SET_TEXT_SCALE(float scaleX, float scaleY) {
        UI::SET_TEXT_SCALE(scaleX, scaleY);
    }

    void SET_TEXT_COLOUR(int r, int g, int b, int a) {
        UI::SET_TEXT_COLOUR(r, g, b, a);
    }

    void SET_TEXT_WRAP(float start, float end) {
        UI::SET_TEXT_WRAP(start, end);
    }

    void SET_TEXT_CENTRE(bool center) {
        UI::SET_TEXT_CENTRE(center);
    }

    void SET_TEXT_DROPSHADOW(int distance, int r, int g, int b, int a) {
        UI::SET_TEXT_DROPSHADOW(distance, r, g, b, a);
    }

    void SET_TEXT_EDGE(int thickness, int r, int g, int b, int a) {
        UI::SET_TEXT_EDGE(thickness, r, g, b, a);
    }

    void END_TEXT_COMMAND_DISPLAY_TEXT(float x, float y, int unk) {
        UI::END_TEXT_COMMAND_DISPLAY_TEXT(x, y, unk);
    }

    void DrawRect(float x, float y, float width, float height, int r, int g, int b, int a) {
        GRAPHICS::DRAW_RECT(x, y, width, height, r, g, b, a);
    }
}

void DrawTextOnScreen(const char* text, float x, float y, float scale, int font, int r, int g, int b, int a, bool center)
{
    HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    HUD::SET_COLOUR_OF_NEXT_TEXT_COMPONENT(r);  // Set the main color here
    HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
    HUD::SET_TEXT_FONT(font);
    HUD::SET_TEXT_SCALE(scale, scale);
    HUD::SET_TEXT_COLOUR(r, g, b, a);
    HUD::SET_TEXT_WRAP(0.0f, 1.0f);
    HUD::SET_TEXT_CENTRE(center);
    HUD::SET_TEXT_DROPSHADOW(2, 2, 2, 255, 255);
    HUD::SET_TEXT_EDGE(1, 0, 0, 0, 205);
    HUD::END_TEXT_COMMAND_DISPLAY_TEXT(x, y, 0);
}

std::string GetPlayerInfoString()
{
    Ped playerPed = PLAYER::PLAYER_PED_ID();

    float health = ENTITY::GET_ENTITY_HEALTH(playerPed) - 100;
    float maxHealth = ENTITY::GET_ENTITY_MAX_HEALTH(playerPed) - 100;
    float healthPercent = (maxHealth > 0) ? (health / maxHealth) * 100.0f : 0.0f;

    Vector3 coords = ENTITY::GET_ENTITY_COORDS(playerPed, TRUE);
    int wantedLevel = PLAYER::GET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID());

    char buffer[512];
    sprintf_s(buffer, "Coordinates: X=%.2f Y=%.2f Z=%.2f | Health: %.0f/%.0f (%.0f%%) | Wanted: %d/5",
        coords.x, coords.y, coords.z,
        health, maxHealth, healthPercent,
        wantedLevel);

    return buffer;
}

void DrawPlayerInfo()
{
    // Draw background box
    HUD::DrawRect(0.85f, 0.15f, 0.25f, 0.25f, 0, 0, 0, 200);

    // Draw header
    DrawTextOnScreen("Hello World - Player Info", 0.85f, 0.05f, 0.5f, 0, 255, 255, 255, 255, true);

    // Draw player info text
    std::string playerInfo = GetPlayerInfoString();
    DrawTextOnScreen(playerInfo.c_str(), 0.85f, 0.10f, 0.3f, 0, 255, 255, 255, 255, true);

    // Log info to file
    LogPlayerInfo(playerInfo);
}

void ScriptMain()
{
    InitLogging();
    LogPlayerInfo("Player Info Logger initialized");

    while (true)
    {
        DrawPlayerInfo();
        WAIT(0);
    }

    CloseLogging();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        scriptRegister(hModule, &ScriptMain);
        break;
    case DLL_PROCESS_DETACH:
        CloseLogging();
        scriptUnregister(hModule);
        break;
    }
    return TRUE;
}

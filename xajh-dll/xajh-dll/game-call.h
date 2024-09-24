#pragma once


typedef struct _ROLE_INFORMATION {
    float x;
    float y;
    float hp;
    float mp;
} ROLE_INFORMATION, *PROLE_INFORMATION;

void Speak(LPCWSTR content);

DWORD GetNpcGroup(DWORD npcStruct);

ROLE_INFORMATION GetRoleInformation();

void ChoiceMonster();

void MouseSelected();

class GameCall {
};

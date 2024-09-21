#pragma once
#include <Windows.h>
#include "attack-monster.h"
extern AttackMonster attackMonster;

DWORD WINAPI autoAttackThread(LPVOID param);

#include "pch.h"
#include "auto-attack.h"

AttackMonster attackMonster;

DWORD __stdcall autoAttackThread(LPVOID param) {
    while (true) {
        DWORD monsterId = attackMonster.GetCurrentMonsterId();
        if (monsterId != 0) {
        }
        Sleep(300);
    }
    return 0;
}

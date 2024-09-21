#include "pch.h"
#include "attack-monster.h"

AttackMonster::AttackMonster() {
    attackMonsterId = 0;
    InitializeCriticalSection(&cs);
}

AttackMonster::~AttackMonster() {
    DeleteCriticalSection(&cs);
}

DWORD AttackMonster::GetCurrentMonsterId() {
    DWORD id = 0;
    EnterCriticalSection(&cs);
    id = attackMonsterId;
    LeaveCriticalSection(&cs);
    return id;
}

BOOL AttackMonster::SetCurrentMonsterId(DWORD monsterId) {
    EnterCriticalSection(&cs);
    attackMonsterId = monsterId;
    LeaveCriticalSection(&cs);
    return TRUE;
}

BOOL AttackMonster::MonsterDeath() {
    attackMonsterId = 0;
    return 0;
}

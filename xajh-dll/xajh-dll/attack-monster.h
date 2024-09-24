#pragma once
class AttackMonster {
   public:
    AttackMonster();
    ~AttackMonster();
    DWORD attackMonsterId;
    DWORD GetCurrentMonsterId();
    BOOL SetCurrentMonsterId(DWORD monsterId);
    BOOL MonsterDeath();
    CRITICAL_SECTION cs;
};

#include "pch.h"
#include "game-call.h"

void Speak(LPCWSTR content) {
    _asm {
		push 0
		push content;  // ��������
		push 0x100  ;  // ����Ƶ��

        // ecx��Դ
		mov  eax, 0x008C23C0
		call eax
		mov ecx, eax;
        // call
		mov  eax, 0x00CC3010
		call eax
    }
}
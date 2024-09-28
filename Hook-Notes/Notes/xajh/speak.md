00944E93 | 8B09                 | mov ecx,dword ptr ds:[ecx]         | 
00944E95 | 8B7C24 14            | mov edi,dword ptr ss:[esp+14]      | 
00944E99 | 8B5424 10            | mov edx,dword ptr ss:[esp+10]      |
00944E9D | 8B4424 3C            | mov eax,dword ptr ss:[esp+3C]      | 
00944EA1 | 51                   | push ecx                           |
00944EA2 | 52                   | push edx                           | 
00944EA3 | 50                   | push eax                           |
00944EA4 | 8BCD                 | mov ecx,ebp                        |
00944EA6 | E8 65E13700          | call xajh.CC3010                   | 5




push 0
push 0x03DF0000  // 喊话内容
push 0x100       // 喊话频道
call 0x008C23C0  // ecx来源
mov ecx,eax
call 0x00CC3010
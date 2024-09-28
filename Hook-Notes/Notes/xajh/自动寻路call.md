// 自动寻路
先在send下断点进游戏寻路  找到寻路会断下  但是会一直断的地方

寻路时 ecx+8 的地方会写入 寻路相关的类  要找谁往里面写入  ecx来源上一层
```
00797460 | 56                       | push esi                                | [[[ecx+8]+0]+4]==00767DD0
00797461 | 8BF1                     | mov esi,ecx                             |
00797463 | 837E 08 00               | cmp dword ptr ds:[esi+8],0              |
00797467 | 0F84 C2000000            | je xajh.79752F                          |
0079746D | 8B4E 08                  | mov ecx,dword ptr ds:[esi+8]            | 虚函数类对象 
00797470 | 8B01                     | mov eax,dword ptr ds:[ecx]              |
00797472 | 8B5424 08                | mov edx,dword ptr ss:[esp+8]            | 
00797476 | 8B40 04                  | mov eax,dword ptr ds:[eax+4]            |
00797479 | 57                       | push edi                                | 
0079747A | 52                       | push edx                                |
0079747B | FFD0                     | call eax                                | 999   eax==00767DD0

// ecx不会变 但是ecx+8处会改变   下硬件写入   然后游戏内寻路
0075FD96 | E8 85D8D3FF              | call xajh.49D620                        |
0075FD9B | 57                       | push edi                                |
0075FD9C | 8BCE                     | mov ecx,esi                             | ecx 来源
0075FD9E | E8 BD760300              | call xajh.797460                        | 10 10

找到这个位置   接着寻找edi来源
00795B1A | 6A 00                    | push 0                                  |
00795B1C | 50                       | push eax                                |
00795B1D | 897E 08                  | mov dword ptr ds:[esi+8],edi            | xxxx
00795B20 | E8 67599100              | call <JMP.&memset>                      |

edi又来源另一个 虚函数call 虚函数分析太复杂跳过   接着在 00797460 地址下 [[[ecx+8]+0]+4]==00767DD0 条件断点
得到ecx 再对ecx下硬件写入断点 跳转到这里  `00795B20  call <JMP.&memset>`   ctrl+f9 往上返回几层得到不是call寄存器的指令
00795AD0 | 56                       | push esi                                | 
00795AD1 | 8BF1                     | mov esi,ecx                             | 
00795AD3 | 8B4C24 08                | mov ecx,dword ptr ss:[esp+8]            |
00795AD7 | 8B06                     | mov eax,dword ptr ds:[esi]              | 
00795AD9 | 8B50 20                  | mov edx,dword ptr ds:[eax+20]           | 
00795ADC | 57                       | push edi                                |
00795ADD | 51                       | push ecx                                |
00795ADE | 8BCE                     | mov ecx,esi                             | 
00795AE0 | FFD2                     | call edx                                |
00795AE2 | 8BF8                     | mov edi,eax                             | edi 来源

这个不是call寄存器 但是参数太少
0076BDDC | 6A 66                    | push 66                                 |
0076BDDE | 6A 03                    | push 3                                  |
0076BDE0 | E8 BB950200              | call xajh.7953A0                        |

这个call 有0x30个参数
00730DB6 | 8B8B D8030000            | mov ecx,dword ptr ds:[ebx+3D8]          |
00730DBC | D91C24                   | fstp dword ptr ss:[esp]                 | [esp]:SleepEx+4B
00730DBF | 50                       | push eax                                |
00730DC0 | 56                       | push esi                                |
00730DC1 | E8 5A8DFFFF              | call xajh.729B20                        |

这个call有0x28个参数  选择参数少的   为什么会找到这里?
00A5EDAF | 6A 00                    | push 0                                  |
00A5EDB1 | 6A 00                    | push 0                                  |
00A5EDB3 | 6A 00                    | push 0                                  |
00A5EDB5 | 6A 00                    | push 0                                  |
00A5EDB7 | E8 4417CDFF              | call xajh.730500                        |
```

```x86asm
push 0
push 1
push 3F4CCCCD
push 0
push 0F420000  坐标
call 4AEFA0
mov ecx, dword ptr ds:[eax+0x274]
push 48 地图id
push 0
push 0
push 0
push 0
call 730500
```

```cpp
void CMainDialog::OnBnClickedFindWay() {
    typedef struct _CORRDINATES {
        float x;
        float z;
        float y;
    } CORRDINATES, *PCORRDINATES;

    CORRDINATES coordinates = {};
    coordinates.x = -150.0;
    coordinates.y = 50.0;
    coordinates.z = 0;
    DWORD mapId = 0x44;
    _asm { 
        push 0
        push 1
        push 0x3F4CCCCD
        push 0

        // 坐标
        lea eax, coordinates
        push eax;

        // ecx
        mov eax, 0x4AEFA0
        call eax
        mov ecx, dword ptr ds:[eax+0x274]

        // 地图id
        push mapId 
        push 0
        push 0
        push 0
        push 0
        mov eax, 0x730500
        call eax
    }
}

```
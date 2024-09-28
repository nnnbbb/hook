```
00ACE2A3 | 897B 1C              | mov dword ptr ds:[ebx+1C],edi            |

00A209B8 | 8D8E A0020000        | lea ecx,dword ptr ds:[esi+2A0]       | esi+2a0==58916D90   esi==58916AF0

00E59976 | 8B1C03               | mov ebx,dword ptr ds:[eax+10]     | [10+eax]==58916AF0   eax==482A54B0

00E59970 | 8B86 C8000000        | mov eax,dword ptr ds:[esi+C8]      | [esi+c8]==482A54B0  esi==58B953B8

008BBCD9 | 8B8C86 20030000      | mov ecx,dword ptr ds:[esi+0x324] | [esi+1*4+0x320]==58B953B8  esi==23E30048

004AD7AB | 8B4D 08              | mov ecx,dword ptr ss:[ebp+8]         | [ebp+8]==23E30048  ebp==0F6111D8

0048B7D0 | 8B4B 24              | mov ecx,dword ptr ds:[ebx+24]        | [ebx+24]==0F6111D8 ecx==014C2050

004B9920 | B9 50204C01          | mov ecx,xajh.14C2050                 | ecx==014C2050

[10+[[[[ebx+24]+8]+1*4+0x320]+c8]]+2a0+1C

[esi+2A0]+1C


ecx==014C2050   

[  [  [  [014C2050+24]+0x324]  +c8]  +0x10]  +2a0+1c  

ebx==014C2050

ebx==014C2050   [[[[[014C2050+24]+8]+0x324]+c8]+0x10]+2a0+1c   


[[[[[14C2050  +24]+8]+0x324]+C8]+10]+2A0+1C
```
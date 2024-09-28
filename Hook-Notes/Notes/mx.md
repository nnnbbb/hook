战盟，梦想世界，左下角，以兼容模式启动
https://tieba.baidu.com/p/8414627789

```
0063816E | 8881 B1010000            | mov byte ptr ds:[ecx+1B1],al            | ecx+1b1 == 35D39D59 35D39BA8  

00E55ED3 | 8B49 24                  | mov ecx,dword ptr ds:[ecx+24]           | [ecx+24]==35D39BA8     ecx==1C737AB8

00EC183C | 8B4E 18                  | mov ecx,dword ptr ds:[esi+18]           | [esi+18]==1C737AB8  esi==35D86050

00EC1814 | 8B75 08                  | mov esi,dword ptr ss:[ebp+8]            | [ebp+8]==35D86050  ebp==00197D00(开)  00199708(关)

00D302F4 | 53                       | push ebx                               | ebx==35D86050 

00D302BE | 8B5A 0C                  | mov ebx,dword ptr ds:[edx+C]           | [edx+c]==35D86050 


push  0x0019C920
mov ebp,esp
push  0x00000010
push  0
push  0
call dword ptr ds:[0x01554150]
add esp,0xc
pop ebp
ret


push 0x0019C960
mov ebp,esp
push  1
push  0x201F5378
call 0x00D3C7F0
add esp, 0x08
ret


00D39FD6 | FF75 08              | push dword ptr ss:[ebp+8]      | [ebp+8]==201A1378

0019C968   201C9AA8       
0019C96C   0F86E59C       
0019C970   0019C9D8       

push 0x0019C9D8
push 0x0F86E59C
push 0x201C9AA8
call 0x00D3C0A0
add esp, 0xc

push 1
push 20222378
call mxmain.D3C7F0
add esp,8

00D01AB4 | 3B43 04              | cmp eax,dword ptr ds:[ebx+4]   | eax:AmdPowerXpressRequestHighPerformance+53234
00D48459 | FF75 C4              | push dword ptr ss:[ebp-3C]     | [ebp-3c]==202CA378
00D39FD6 | FF75 08              | push dword ptr ss:[ebp+8]                | [ebp+8]==201EEAA8

00D461B1 | 891E                 | mov dword ptr ds:[esi],ebx               | ebp==0019C6EC

[[[29550000]+18]+24]+1B1

jl 指令用于执行有符号整数比较后的跳转 jmp less
cmp eax, ebx
jl less_than
如果 eax < ebx，则跳转到 less_than
015506E0

00D3C867 | 8973 08              | mov dword ptr ds:[ebx+8],esi   |
00D3C86A | 8943 0C              | mov dword ptr ds:[ebx+C],eax   |

00D3C854 | FF73 0C              | push dword ptr ds:[ebx+C]      | 0019C920==ebp 开 0019C8E0==ebp 关


00D3C867 | 8973 08                  | mov dword ptr ds:[ebx+8],esi        |
00D3C86A | 8943 0C                  | mov dword ptr ds:[ebx+C],eax        |


00D3C7F0 | 55                       | push ebp                            | 1
00D3C7F1 | 8BEC                     | mov ebp,esp                         |
00D3C7F3 | 53                       | push ebx                            |
00D3C7F4 | 8B5D 08                  | mov ebx,dword ptr ss:[ebp+8]        | 0019C920==ebp 开 0019C8E0==ebp 关
00D3C7F7 | 56                       | push esi                            |
00D3C7F8 | 8B75 0C                  | mov esi,dword ptr ss:[ebp+C]        |


00D3C103 | 50                       | push eax                            | eax==1
00D3C104 | 56                       | push esi                            | esi==2017C378
00D3C105 | E8 E6060000              | call mxmain.D3C7F0                  |
00D3C10A | 83C4 08                  | add esp,8                           |


00D3C0AB | 33C5                     | xor eax,ebp                         |
00D3C0AD | 8945 FC                  | mov dword ptr ss:[ebp-4],eax        |
00D3C0B0 | 53                       | push ebx                            |
00D3C0B1 | 8B5D 0C                  | mov ebx,dword ptr ss:[ebp+C]        |
00D3C0B4 | 56                       | push esi                            |
00D3C0B5 | 8B75 08                  | mov esi,dword ptr ss:[ebp+8]        | ebp==0019C960 开

00D48459 | FF75 C4                  | push dword ptr ss:[ebp-3C]          | [ebp-3c]==2017C378

00D482FF | 8975 C4                  | mov dword ptr ss:[ebp-3C],esi       | esi==2017C378

00D482F9 | 8B77 0C                  | mov esi,dword ptr ds:[edi+C]        | [edi+c]==2017C378   edi==2112F8C8

00D482CC | 8B3A                     | mov edi,dword ptr ds:[edx]          | edi==2112F8C8  [edx]==2112F8C8

00D482C4 | 8B16                     | mov edx,dword ptr ds:[esi]          |

00D482AE | 8B75 08                  | mov esi,dword ptr ss:[ebp+8]        | [ebp+08]

00D0147D | FFD1                     | call ecx                            |

ebp==0019C9D8 && esp==0019C980



push 0x0019c920
push 0x00000010
push 0x32CF7378     
push 0
call dword ptr ds:[0x01554150]


push 0x0019C8E0  
push 0x00000000
push 0x32CF6428
push 0
call dword ptr ds:[0x01554150]

8B 75 08 85 F6 60 9C B9 00 7D 19 00 39 CD 75 09 8B 55 08 89 15 0F 7C 14 01 9D 61 E9 43 9C D7 FF
8B 75 08 85 F6 60 9C B9 00 7D 19 00 39 CD 75 09 8B 55 08 89 15 xx xx xx xx 9D 61 E9 7B 98 D7 FF


00EC1814 | E9 74672800              | jmp mxmain.1147F8D                     | [ebp+8]==35D86050  ebp==00197D00(开)  00199708(关)


01147F8D | 8B75 08                  | mov esi,dword ptr ss:[ebp+8]           | [ebp+08]:"褒V\t"
01147F90 | 85F6                     | test esi,esi                           |
01147F9E | E9 7698D7FF              | jmp mxmain.EC1819                      |



00638174 | 84C0                     | test al,al                                  | 11


00E55ED0 | 55                       | push ebp                                    | 
00E55ED1 | 8BEC                     | mov ebp,esp                                 |
00E55ED3 | 8B49 24                  | mov ecx,dword ptr ds:[ecx+24]               | 暂停条件([ecx+24] == 35032908)  ecx==2BF20948
00E55ED6 | 85C9                     | test ecx,ecx                                | [2BF20948+24]=35032908


00EC183C | 8B4E 18                  | mov ecx,dword ptr ds:[esi+18]               | [esi+18]==2BF20948    esi==34FD5950


00EC1814 | 8B75 08                  | mov esi,dword ptr ss:[ebp+8]         | [ebp+8]==34FD5950  ebp==00197D00   00197CFC  00199708

00EC1810 | 55                       | push ebp                             |
00EC1811 | 89E5                     | mov ebp,esp                          | esp==00197D00

00D302F4 | 53                       | push ebx                             | ebx==34FD5950
00D302BE | 8B5A 0C                  | mov ebx,dword ptr ds:[edx+C]         |  ebx==34FD5950

```
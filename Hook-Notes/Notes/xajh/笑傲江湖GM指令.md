GM命令代码
笑傲江湖GM命令:

按 Shift+~ 打开GM控制台输入下面的就可以了

刷元宝
d_delcmd 168 数量 数量自己写 不要调太多会变负数的

刷银子
d_cmd 2003 1 数量

刷银票
d_cmd 2003 0 数量

刷等级
d_c2scmd 2000 数量 最高调200级

刷物品
d_c2scmd 2001 物品代码 数量
例子：
物品升级+10
d_c2scmd 2001 63799 数量  这里63799是9级精炼石代码 数量自己写，建议99一次


刷武功（不建议刷，毕竟玩游戏，自己慢慢练才有意思）
d_c2scmd 2013 1开始全部是学习武学秘籍 （这个千万别乱刷）学习武功后再刷不然没效果


d_c2scmd 2013 119开始是少林依次是武当各门派的技能 技能太多 不计其数
（千万别乱刷 一刷就把原有的技能覆盖了 什么招式也使不出 废人一个了）因为这句话不敢乱试


刷经验（这个不建议刷，刷了就没意思了，跟刷等级一样效果）
d_c2scmd 2018 数量 是给自己加经验  后面输入多少加多少


刷功力（这个刷过，刷再多也只能学习1400多的功力）
d_c2scmd 2025 后面加数值  加功力输多少加多少



刷兑换道具 （找地图里带“内”和“外”字的NPC兑换30-90级蓝色装备）
d_c2scmd 2001 44806 一品英雄令
d_c2scmd 2001 57435 二品英雄令
d_c2scmd 2001 57479 三品英雄令
d_c2scmd 2001 57480 四品英雄令
d_c2scmd 2001 57481 五品英雄令
d_c2scmd 2001 57482 六品英雄令
d_c2scmd 2001 57483 七品英雄令
d_c2scmd 2001 57484 八品英雄令
d_c2scmd 2001 70749 九品英雄令


刷包裹（其实这个没必要，因为商城里有，刷元宝后直接购买）
d_c2scmd 2001 72207 帝王背包100格子


刷门派
d_c2scmd 2007 1 少林
d_c2scmd 2007 2 武当
d_c2scmd 2007 3 华山
d_c2scmd 2007 4 恒山
d_c2scmd 2007 5 衡山
d_c2scmd 2007 6 日月
d_c2scmd 2007 7 五仙
d_c2scmd 2007 8 逍遥
d_c2scmd 2007 9 唐门
d_c2scmd 2007 10 峨眉


刷好感度（这个值好像是特殊剧情和排行榜中用的）
d_cmd 2010 111 10000 1万令狐冲好感度  
d_cmd 2010 112 10000 1万任盈盈好感度
d_cmd 2010 113 10000 1万岳灵珊好感度
d_cmd 2010 114 10000 1万林平之好感度
d_cmd 2010 115 10000 1万任我行好感度
d_cmd 2010 116 10000 1万仪琳好感度  
d_cmd 2010 117 10000 1万风清扬好感度
d_cmd 2010 118 10000 1万东方不败好感度  
d_cmd 2010 119 10000 1万蓝凤凰好感度
d_cmd 2010 120 10000 1万田伯光好感度  


加一百万绑定币           d_cmd 2003 0 1000000
加一百万交易币           d_cmd 2003 1 1000000
加一百万经验             d_cmd 2018 1000000
加一百万伙伴经验         d_cmd 2027 1000000
加八千生活名望           d_cmd 2010 3 8000
加一千生产点             d_cmd 2030 0 1000
加一千社交点             d_cmd 2030 1 1000
加一千活动点             d_cmd 2030 2 1000
加一万元宝               d_delcmd 168 1000000
获得十万功力             d_cmd 2078 100000
将目标扣一千万血         d_cmd 50005 10000000000
原地自杀                d_cmd 2019


奔跑加速：打开   d_runspeed 10
奔跑加速：关闭   d_runspeed 4
查看帧数：打开   d_fps 1
查看帧数：关闭   d_fps 0

大回血药          d_cmd 2001 3619
大回蓝药          d_cmd 2001 5217
小怒气药+10怒气   d_cmd 2001 19064
中怒气药+50怒气   d_cmd 2001 19065
大怒气药+200怒气  d_cmd 2001 19066
体力药            d_cmd 2001 28927

状态药：
使用后可以加上状态：攻速提升 d_cmd 2001 3735 100
使用后可以加上状态：闪避提升 d_cmd 2001 3736 100
使用后可以加上状态：速度提升 d_cmd 2001 3737 100
使用后可以加上状态：外攻提升 d_cmd 2001 3738 100
使用后可以加上状态：内攻提升 d_cmd 2001 3739 100
使用后可以加上状态：外防提升 d_cmd 2001 3740 100
使用后可以加上状态：内防提升 d_cmd 2001 3741 100
使用后可以加上状态：冰抗提升 d_cmd 2001 3742 100
使用后可以加上状态：火抗提升 d_cmd 2001 3743 100
使用后可以加上状态：毒抗提升 d_cmd 2001 3744 100
使用后可以加上状态：阴抗提升 d_cmd 2001 3745 100
使用后可以加上状态：阳抗提升 d_cmd 2001 3746 100
使用后可以加上状态：玄抗提升 d_cmd 2001 3747 100
使用后可以加上状态：力量提升 d_cmd 2001 3748 100
使用后可以加上状态：敏捷提升 d_cmd 2001 3749 100
使用后可以加上状态：体力提升 d_cmd 2001 3750 100
使用后可以加上状态：智力提升 d_cmd 2001 3751 100
使用后可以加上状态：精神提升 d_cmd 2001 3752 100
使用后可以加上状态：恢复气血 d_cmd 2001 3753 100
使用后可以加上状态：恢复内力 d_cmd 2001 3754 100

笑傲江湖10级宝石代码:
56539        10级日光石
56549        10级月光石
56559        10级红宝石
56569        10级蓝宝石
56579        10级白水晶
56589        10级红水晶
56599        10级黄水晶
56609        10级墨水晶
56619        10级白玛瑙
56645        10级红玛瑙
56655        10级黄玛瑙
56665        10级绿玛瑙
56675        10级东陵石
56685        10级苍珊瑚
56695        10级刺珊瑚
56705        10级红珊瑚
56715        10级角珊瑚
56725        10级石珊瑚
56735        10级白玉
56745        10级碧玉
56755        10级红玉
56765        10级黄玉
56775        10级青金石
56788        10级黑珍珠
56798        10级红翡翠
56808        10级黄棕翡
56818        10级兰花冰
56828        10级墨翡翠
75287        10级虹彩石
75298        10级天河石


0086BFF6        | E8 05FFFFFF              | call xajh.86BF00                   | gm4

0086BFC7        | 8BF1                     | mov esi,ecx                        |

00E49514        | 8B4E 34                  | mov ecx,dword ptr [esi+0x34]       | 
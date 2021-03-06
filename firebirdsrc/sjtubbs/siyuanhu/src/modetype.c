/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include "modes.h"

char *
ModeType (mode)
int mode;
{
    switch (mode)
    {
    case IDLE:
        return "";
    case NEW:
        return "新站友注册";
    case LOGIN:
        return "进入本站";
    case DIGEST:
        return "浏览精华区";
    case MMENU:
        return "转换阵地中";
    case ADMIN:
        return "管理者选单";
    case SELECT:
        return "选择讨论区";
    case READBRD:
        return "一览众山小";
    case READNEW:
        return "看看新文章";
    case READING:
        return "品味文章";
    case POSTING:
        return "文豪挥笔";
    case MAIL:
        return "处理信笺";
    case SMAIL:
        return "寄语信鸽";
    case RMAIL:
        return "阅览信笺";
    case TMENU:
        return "聊天选单";
    case LUSERS:
        return "东张西望:)";
    case FRIEND:
        return "寻找好朋友";
    case MONITOR:
        return "探视民情";
    case QUERY:
        return "查询网友";
    case TALK:
        return "聊天";
    case PAGE:
        return "呼叫";
    case CHAT1:
        return "国际会议厅";
    case CHAT2:
        return "南洋客栈";
    case LAUSERS:
        return "探视网友";
    case XMENU:
        return "系统资讯";
    case VOTING:
        return "投票中...";
    case BBSNET:
        return "BBSNET";
    case EDITWELC:
        return "编辑Welc";
    case EDITUFILE:
        return "编辑个人档";
    case EDITSFILE:
        return "编修系统档";
    case ZAP:
        return "订阅讨论区";
    case SYSINFO:
        //return "检查系统";
        //by alt : 抽奖临时借用这个状态
        return "幸运抽奖";
    case DICT:
        return "翻查字典";
    case LOCKSCREEN:
        return "萤幕锁定";
    case NOTEPAD:
        return "留言板";
    case GMENU:
        return "工具箱";
    case MSG:
        return "送讯息";
    case USERDEF:
        return "自订参数";
    case EDIT:
        return "修改文章";
    case OFFLINE:
        return "自杀中..";
    case EDITANN:
        return "编修精华";
/*
            case CCUGOPHER:
                return "他站精华";*/
        
    case LOOKMSGS:
        return "察看讯息";
    case WNOTEPAD:
        return "欲走还留";
    case GOODWISH:
        return "给朋友祝福";
    case FRIENDTEST:
        return "友谊测试";
    case FAVOURITE:
        return "个人收藏夹";
    case QAUTHOR:
        return "查询作者";

	case DOWNLINE:
		return "查看下线";

	case M_BLACKJACK:
		return "★黑甲克★";
	case M_XAXB:
		return "★猜数字★";
	case M_DICE:
		return "★西八拉★";
	case M_GP:
		return "金扑克梭哈";
	case M_NINE:
		return "天地九九";
        case WINMINE:
                return "键盘扫雷";
	case M_BINGO:
		return "宾果宾果";
        case FIVE:
		return "决战五子棋";
	case MARKET:	
		return "交易市场";
        case PAGE_FIVE:
		return "邀请下棋";
	case CHICK:
		return "电子小鸡";
	case MARY:
		return "超级玛丽"; 
	case CHICKEN:
		return "星空战斗鸡";
	case FREEIP:
		return "免费IP查询";
	case TT:
		return "打字练习";
	case TETRIS:
		return "俄罗斯方块";
	case WINMINE2:
		return "感应扫雷";
	case CAL:
		return "查询万年历";	
	case SALARY:
		return "工资统计";
	case BANK:
		return "银行查询";
	case ARCHIEVE:
		return "档案登记";
	case AGENTLAND:
		return "住宅销售";
	case ARTICLE:
		return "创建宝物中";
	case CMAGIC:
		return "创建魔法中";
	case 10001:
		return "Web 浏览";
    default:
        return "去了那里!?";
    }
}

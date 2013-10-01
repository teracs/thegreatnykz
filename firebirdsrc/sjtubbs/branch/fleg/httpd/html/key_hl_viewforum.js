<!--
function killErrors() { return true; } 
window.onerror = killErrors; 

self.focus();

document.onkeydown = kd;

function kd()
{
	if ( window.event.keyCode == 75 && !window.event.ctrlKey && !window.event.altKey )
	{
		//k，上
		previous();
	}
	else if ( window.event.keyCode == 74 && !window.event.ctrlKey && !window.event.altKey )
	{
		//j，下
		next();
	}
	else if ( window.event.keyCode == 37 )
	{
		//左
		returnboard();
	}
	else if ( window.event.keyCode == 82 && !window.event.ctrlKey && !window.event.altKey )
	{
		//r，回复帖子
		reforum();
	}
	else if ( window.event.keyCode == 80 && !window.event.ctrlKey && !window.event.altKey )
	{
		//p, 同主题阅读
		readsubject();
	}
	else if ( window.event.keyCode == 70 && !window.event.ctrlKey && !window.event.altKey )
	{
		//f, 转寄/推荐
		forward();
	}
	else if ( window.event.keyCode == 90 && !window.event.ctrlKey && !window.event.altKey )
	{
		//z, 转贴
		cc();
	}
	else if ( window.event.keyCode == 69 && !window.event.ctrlKey && !window.event.altKey )
	{
		//E, 修改
		modify();
	}
	else if ( window.event.keyCode == 68 && !window.event.ctrlKey && !window.event.altKey )
	{
		//d, 删除
		del();
	}
	else if ( window.event.keyCode == 189 && window.event.shiftKey )
	{
		//Shift + -, 设置不可re
		nore();
	}
	else if ( window.event.keyCode == 72 && !window.event.ctrlKey && !window.event.altKey )
	{
		//H, 查看键盘帮助
		showhelp();
	}
	else
	{
		//alert(window.event.keyCode);
	}
}

function next()
{
	if ( nextfilename != "" )
	{
		location.href('bbscon?board=' + board + '&file=' + nextfilename + '&num=' + (start+currentid+1) + '&start=' + start);
		foo();
	}
}

function previous()
{
	if ( prefilename != "" )
	{
		location.href('bbscon?board=' + board + '&file=' + prefilename + '&num=' + (start+currentid-1) + '&start=' + start);
		foo();
	}
}

function reforum()
{
	location.href('bbspst?board=' + board + '&file=' + fileName);
	foo();
}

function readsubject()
{
	location.href('bbstfind?board=' + board + '&title=' + title);
	foo();
}

function returnboard()
{
	location.href('bbsdoc?board=' + board + '&currentpoint=' + currentid + '&start=' + start);
	foo();
}

function forward()
{
	location.href('bbsfwd?board=' + board + '&file=' + filename);
	foo();
}

function cc()
{
	location.href('bbsccc?board=' + board + '&file=' + filename);
	foo();
}

function modify()
{
	location.href('bbsedit?board=' + board + '&file=' + filename);
	foo();
}

function del()
{
	if ( confirm("你真的要删除此文吗？") )
	{
		location.href('bbsdel?board=' + board + '&file=' + filename);
		foo();
	}
}

function nore()
{
	location.href('bbsdocnore?board=' + board + '&file=' + filename + '&start=' + start + '&num=' + (start+currentid));
	foo();
}

function showhelp()
{
	var str	= "[转寄/推荐F] [转贴Z] [删除D] [修改E] [设置可RE属性_]\n";
	str	+= "[上一篇K] [返回讨论区←] [下一篇J]";
	str	+= "[回文章R] [同主题阅读P]";
	alert(str);
}


function foo()
{
	//首先停止事件沸腾
	event.cancelBubble	= true;
	//其次不再接受用户键盘消息，免得造成多次发送请求
	document.onkeydown	= foo2;
}

function foo2()
{
	//啥也不干
}

-->

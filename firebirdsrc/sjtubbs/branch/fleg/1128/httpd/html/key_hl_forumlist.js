<!--
function killErrors() { return true; } 
window.onerror = killErrors; 

self.focus();

document.onkeydown = kd;

function kd()
{
	if ( (window.event.keyCode == 75 && !window.event.ctrlKey && !window.event.altKey) || window.event.keyCode == 38 )
	{
		//K, 上
		moveup();
	}
	else if ( (window.event.keyCode == 74 && !window.event.ctrlKey && !window.event.altKey) || window.event.keyCode == 40 )
	{
		//J, 下
		movedown();
	}
	else if ( window.event.keyCode == 39 )
	{
		//右，看帖子
		viewforum();
	}
	else if ( window.event.keyCode == 37 )
	{
		//左
		//alert("左");
	}
	else if ( window.event.keyCode == 13 )
	{
		//回车，看帖子
		viewforum();
	}
	else if ( window.event.keyCode == 36 || window.event.keyCode == 48 )
	{
		//Home 0, 到达首页
		GoToBegin();
	}
	else if ( window.event.keyCode ==35 || window.event.keyCode == 52 )
	{
		//End 4, 到达末页
		GoToEnd();
	}
	else if ( window.event.keyCode == 33 )
	{
		//PageUp, 向上翻页
		PageUp();
	}
	else if ( window.event.keyCode == 34 )
	{
		//PageDown, 向下翻页
		PageDown();
	}
	else if ( window.event.ctrlKey && window.event.keyCode == 80 )
	{
		//Ctrl+P, 发表文章
		if ( confirm("要在这里发表文章吗？") )
		{
			Post();
		}
	}
	else if ( window.event.keyCode == 90 && !window.event.ctrlKey && !window.event.altKey )
	{
		//Z, 转贴
		cc();
	}
	else if ( window.event.keyCode == 67 && !window.event.ctrlKey && !window.event.altKey )
	{
		//C, 清除阅读标记
		ClearUnread();
	}
	else if ( window.event.keyCode == 70 && !window.event.ctrlKey && !window.event.altKey )
	{
		//f, 转寄/推荐
		forward();
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
	else if ( window.event.keyCode == 191 && !window.event.ctrlKey && !window.event.altKey )
	{
		///, 板内查询
		bfind();
	}
	else if ( window.event.keyCode == 80 && !window.event.ctrlKey && !window.event.altKey )
	{
		//P, 主题模式
		tdoc();
	}
	else if ( window.event.keyCode == 189 && window.event.shiftKey )
	{
		//Shift + -, 设置不可re
		nore();
	}
	else if ( window.event.keyCode == 82 && !window.event.ctrlKey && !window.event.altKey )
	{
		//R, 刷新
		location = location;
	}
	else if ( window.event.keyCode == 71 && !window.event.ctrlKey && !window.event.altKey )
	{
		//G, 文摘区
		gdoc();
	}
	else if ( window.event.keyCode == 88 && !window.event.ctrlKey && !window.event.altKey)
	{
		//X，精华区
		xdoc();
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

function movedown()
{
	var item;

	if ( totalid <= currentid )
	{
		//如果不是尾页,翻页
		if ( start < (total-totalid-1) )
		{
			PageDown();
		}
		return;
	}
	
	//当前的">"去掉
	item = "arrow" + currentid;
	document.all(item).innerText = "";
	
	//把当前游标移动到下一个上
	++currentid;
	item = "arrow" + currentid;
	document.all(item).innerText = ">";
}

function moveup()
{
	var item;

	if ( currentid == 0 )
	{
		//不是首页，翻页
		if ( start > 0 )
		{
			location.href('bbsdoc?board=' + board + '&start=' + (start-totalid-1) + '&currentpoint=' + (totalid-toptotal));
		}
		return;
	}

	//当前的">"去掉
	item = "arrow" + currentid;
	document.all(item).innerText = "";
	
	//把当前游标移动到下一个上
	--currentid;
	item = "arrow" + currentid;
	document.all(item).innerText = ">";
}

function viewforum()
{
	if ( currentid <= (totalid-toptotal) )
	{
		//普通文章
		location.href('bbscon?' + arrayid[currentid]);
	}
	else
	{
		//置顶文章
		location.href('bbstopcon?' + arrayid[currentid]);
	}
	foo();
}

function PageDown()
{
	location.href('bbsdoc?board=' + board + '&start=' + (start+totalid+1));
	foo();
}

function PageUp()
{
	location.href('bbsdoc?board=' + board + '&start=' + (start-totalid-1));
	foo();
}

function GoToEnd()
{
	location.href('bbsdoc?currentpoint=' + (totalid-toptotal) + '&board=' + board);
	foo();
}

function GoToBegin()
{
	location.href('bbsdoc?start=0&board=' + board);
	foo();
}

function Post()
{
	location.href('bbspst?board=' + board);
	foo();
}

function ClearUnread()
{
	location.href('bbsclear?start=' + start + '&board=' + board);
	foo();
}

function forward()
{
	location.href('bbsfwd?' + arrayid[currentid]);
	foo();
}

function cc()
{
	location.href('bbsccc?' + arrayid[currentid]);
	foo();
}

function modify()
{
	location.href('bbsedit?' + arrayid[currentid]);
	foo();
}

function del()
{
	if ( confirm("你真的要删除此文吗？") )
	{
		location.href('bbsdel?' + arrayid[currentid]);
		foo();
	}
}

function bfind()
{
	location.href('bbsbfind?board=' + board);
	foo();
}

function tdoc()
{
	location.href('bbstdoc?board=' + board);
	foo();
}

function gdoc()
{
	location.href('bbsgdoc?board=' + board);
	foo();
}

function xdoc()
{
	location.href('bbs0an?path=' + boardpath);
	foo();
}

function nore()
{
	location.href('bbsdocnore?' + arrayid[currentid]);
	foo();
}

function showhelp()
{
	var str = "键盘操作:\n";
	str += "[↑]/[K] 上移/上翻页  [↓]/[J] 下移/下翻页  [→]/[Enter] 看帖子  [PageUp] 上翻页  [PageDown] 下翻页\n";
	str += "[0]/[Home] 首页  [4]/[End] 末页  [R] 刷新\n";
	str += "[D] 删除文章  [E] 编辑文章 [F] 转寄/推荐  [Z] 转贴  [_] 设置可RE属性\n";
	str += "[Ctrl+P] 发表文章  [R] 刷新  [C] 清除未读  [P]主题模式  [/]版内查询\n";
	str += "[G]文摘区  [X]精华区";
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


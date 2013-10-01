<!--
function killErrors() { return true; } 
window.onerror = killErrors; 

self.focus();

document.onkeydown = kd;

function kd()
{
	if ( window.event.keyCode == 75 && !window.event.ctrlKey && !window.event.altKey )
	{
		//K, 上
		moveup();
	}
	else if ( window.event.keyCode == 74 && !window.event.ctrlKey && !window.event.altKey )
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
	else if ( window.event.keyCode == 82 && !window.event.ctrlKey && !window.event.altKey )
	{
		//r，看帖子
		viewforum();
	}
	else if ( window.event.keyCode == 79 && !window.event.ctrlKey && !window.event.altKey )
	{
		//O, 恢复
		returndel();
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
			PageUp();
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
	location.href('bbscondel?' + arrayid[currentid]);
	foo();
}

function PageDown()
{
	location.href('bbsdocdel?board=' + board + '&start=' + (start+totalid+1));
	foo();
}

function PageUp()
{
	location.href('bbsdocdel?board=' + board + '&start=' + (start-totalid-1));
	foo();
}

function GoToEnd()
{
	location.href('bbsdocdel?currentpoint=' + (totalid-toptotal) + '&board=' + board);
	foo();
}

function GoToBegin()
{
	location.href('bbsdocdel?start=0&board=' + board);
	foo();
}

function returndel()
{
	location.href('bbsdelreturn?' + arrayid[currentid]);
	foo();
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

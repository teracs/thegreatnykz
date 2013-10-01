<!--
function killErrors() { return true; } 
window.onerror = killErrors; 

self.focus();

document.onkeydown = kd;

function kd()
{
	if ( window.event.keyCode == 75 && !window.event.ctrlKey && !window.event.altKey  )
	{
		//k，上
		previous();
	}
	else if ( window.event.keyCode == 74 && !window.event.ctrlKey && !window.event.altKey  )
	{
		//j，下
		next();
	}
	else if ( window.event.keyCode == 37 )
	{
		//左
		returnboard();
	}
	else if ( window.event.keyCode == 79 && !window.event.ctrlKey && !window.event.altKey  )
	{
		//O, 恢复
		returndel();
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
		location.href('bbscondel?board=' + board + '&file=' + nextfilename + '&num=' + (start+currentid+1) + '&start=' + start);
		foo();
	}
}

function previous()
{
	if ( prefilename != "" )
	{
		location.href('bbscondel?board=' + board + '&file=' + prefilename + '&num=' + (start+currentid-1) + '&start=' + start);
		foo();
	}
}

function returnboard()
{
	location.href('bbsdocdel?board=' + board + '&currentpoint=' + currentid + '&start=' + start);
	foo();
}

function returndel()
{
	location.href('bbsdelreturn?board=' + board + '&file=' + filename);
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

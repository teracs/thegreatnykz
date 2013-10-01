function changemn(mn) {
	o = document.getElementById("div" + mn);
	m = document.getElementById("img" + mn);
	if (o == null) return;
	if (o.style.display == 'block') {
		o.style.display = 'none';
		if (m) m.src = 'images/close.gif';
	} else {
		o.style.display = 'block';
		if (m) m.src = 'images/open.gif';
	}
}
function changeinner()
{
    	tempdir=document.getElementById("temp").value;	
    	document.getElementById(tempdir).innerHTML=window.frames["framehide"].document.getElementById(tempdir).innerHTML;
}
function submenu(dirfile,level)
{
	linkurl	=	"bbsshowmenu?level="	+	level	+	"&boardfile="	+	dirfile;
	submenuname	=	dirfile;
	imgname = "img_" + dirfile ;
	submenustatus = document.getElementById(submenuname).style.display;
	if (submenustatus == "block")
	{
		document.getElementById(imgname).src="http://bbs.sjtu.edu.cn/gif/folderclose.gif";
		document.getElementById(submenuname).style.display = "none";
	} else {
		document.getElementById(imgname).src="http://bbs.sjtu.edu.cn/gif/folderopen.gif";
		document.getElementById(submenuname).style.display = "block";
		document.getElementById("temp").value = dirfile;
		document.getElementById(submenuname).innerHTML="<div class=left>waiting......</div>";
		setTimeout("changeinner()",300);
		window.frames["framehide"].document.location.href = linkurl ;
				
	}
}
function Remeber_Last()
{
	$.cookie("sjtubbs_lasturl", parent.window.frames["f3"].location.href)
}
function Restore_Last(){
	return
	//if ($.cookie("sjtubbs_lasturl") != null){
	//	parent.window.frames["f3"].location.href = $.cookie("sjtubbs_lasturl")
	//	f3.document.location = $.cookie("sjtubbs_last_url")
	//}
	//$.cookie("sjtubbs_lasturl",null)
}


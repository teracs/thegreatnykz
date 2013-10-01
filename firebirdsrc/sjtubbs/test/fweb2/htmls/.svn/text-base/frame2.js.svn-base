function parse_params()
{
        if(location.search.length <= 2) return Array(0);
        var search = location.search.substring(1);
        var ps = search.split("&");
        var params = new Array();
        var i = 0;
        for(i = 0;i<ps.length;i++)
        {
                if(ps[i].length==0) continue;
                var pos = ps[i].indexOf("=");
                if(pos < 0)
                        params[ps[i]]="";
                else
                {
                        var a0 = ps[i].substring(0,pos);
                        var a1 = unescape(ps[i].substring(pos+1));
                        params[a0]=a1;
                }
        }
        return params;
}

function close_req(req)
{
	if(req.close) req.close()
}
var httpreq_to_used = 0;
function xmlhttpreq()
{
	if(httpreq_to_used == 0)
	{
		var req;
	    if (window.XMLHttpRequest) 
	    {
	        req = new XMLHttpRequest();
	        httpreq_to_used = 1;
	    } 
	    else if (window.ActiveXObject) 
	    {
	    	try 
	    	{
	            req = new ActiveXObject("Microsoft.XMLHTTP");
	            httpreq_to_used = 2;
	        } 
	        catch (e) 
	        {
                try 
                {
                    req = new ActiveXObject("Msxml2.XMLHTTP");
                    httpreq_to_used = 3;
                } 
                catch (e) 
                {
                	httpreq_to_used = -1;
                }
	        }
	    }
	    return req;
	}
	switch(httpreq_to_used)
	{
		case 1:
			return new XMLHttpRequest();
		case 2:
			return new ActiveXObject("Microsoft.XMLHTTP");
		case 3:
			return new ActiveXObject("Msxml2.XMLHTTP");
		default:
			return null;
	}
}
function test_xmlreq() 
{
	var req = xmlhttpreq();
    if (req) 
    {
    	close_req(req);
		return true;
    }
    else
    {
    	return false;
    }
}

var req_arr = new Array();
function general_process(ix,func)
{
	if (!req_arr[ix]) return;
	if (req_arr[ix].readyState == 4) 
	{
        // only if "OK"
        if (req_arr[ix].status == 200) 
        {
        	func(req_arr[ix],true);
        }
        else 
        {
        	func(req_arr[ix],false);            
        }
        close_req(req_arr[ix]);
        req_arr[ix] = null;
    }
}
function get_div_content(txt,divid)
{
	var start_str = "<div id='"+divid+"'>";
	var stop_str = "</div>";
	var start_pos = txt.indexOf(start_str);
	var stop_pos = txt.indexOf(stop_str);
	if((start_pos != -1) && (stop_pos != -1))
	{
		return txt.substring(start_pos+start_str.length,stop_pos);
	}
	else
		return "";
}
function f4_func(req,success)        
{
    if (success) 
    {
    	var foot = req.responseText;
    	f4.document.getElementById('footcontent').innerHTML = get_div_content(foot,"foot");
    	f4.cmsg();
    }
    setTimeout("get_html(0)",60000);
}
function process_foot()
{
	general_process(0,f4_func);
}
function fmsg_func(req,success)        
{
    if (success) 
    {
    	var msg = req.responseText;
    	fmsg.document.getElementById('getmsgcontent').innerHTML = get_div_content(msg,"getmsg");
    	fmsg.gmsg();
    }
    else 
    {
        setTimeout("get_html(1)",10000);        
    }
}
function process_getmsg()
{
	general_process(1,fmsg_func);
}

var func_arr = new Array(
	process_foot,
	process_getmsg
);
var url_arr = new Array(
	"/bbsfoot",
	"/bbsgetmsg"
);

function get_html(ix)
{	
    var req = xmlhttpreq();
    if (req) {
	    req.onreadystatechange = func_arr[ix];
	    req.open("GET", url_arr[ix], true);
	    req.send("");
	    req_arr[ix] = req;
	    return req;
    }
    else
    {
    	return null;
    }
}
function rightframe_onload()
{
	if(!test_xmlreq())
	{
		top.location = "frame.html";
		return;
	}
	if(f3.document.location.pathname == "" 
	|| f3.document.location.pathname =="/blank"
	|| f3.document.location.pathname == "blank")
	{
		var ps = parse_params();
		if(ps["index"]
		/* only permit "bbs" and "/bbs" as prefix */
		&& (  ps["index"].toLowerCase().indexOf("bbs") == 0
			|| ps["index"].toLowerCase().indexOf("/bbs") == 0 )
		)
			f3.document.location = ps["index"];
		else
			f3.document.location = "/php/bbsindex.html";
	}
	return;
}

var msg_counter = 0;
function refresh_msg()
{
	get_html(1);
}

function display_msg()
{
	msg_counter = 12;
	//firefox do so
	var rframe = document.getElementById("rframe");
	//ie do so
	if(!rframe.rows) rframe = rightframe;
	rframe.rows = "54,*,20";
}

function no_msg()
{
	//firefox do so
	var rframe = document.getElementById("rframe");
	//ie do so
	if(!rframe.rows) rframe = rightframe;
	rframe.rows = "0,*,20";
	msg_counter--;
	if(msg_counter > 0)
		setTimeout("refresh_msg()",10000);
}

function notify_check_msg()
{
	if(msg_counter<=0)
		refresh_msg();
}

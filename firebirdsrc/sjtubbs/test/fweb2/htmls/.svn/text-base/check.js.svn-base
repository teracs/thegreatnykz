var unwelcome_sites = new Array(
    "chinabbs.com",
    "chinay.com",
    "daqi.com"
);
function redirect2frame()
{
    if(location.pathname.toLowerCase()=="/frame2.html")
    {
    	top.location = location.href;
    }
    else
    {
    	var path = escape(location.pathname+location.search);
    	top.location = location.protocol+"//"+location.hostname+"/frame2.html?index="+path;
    }
}
function check_host()
{
    var i;
    var url = "" + document.referrer;
    if(url.length == 0) return;
    for(i=0;i<unwelcome_sites.length;i++)
    {
        if(url.toLowerCase().indexOf( unwelcome_sites[i].toLowerCase()+"/" ) >= 0)
        {
            redirect2frame();
        }
    }
}
check_host();

function rewrite_swf(){
var obs = document.getElementsByTagName("object");

for(var i = 0;i<obs.length;i++)
{
	var ob = obs[i];
	if(ob.className = "swf")
	{
		if(BrowserDetect.browser=="Mozilla" || BrowserDetect.browser=="Firefox")
		{
			var ob2 = document.createElement("object");
			ob2.type="application/x-shockwave-flash";
			ob2.data = get_object_data(ob);
			ob2.className = ob.className;
			ob2.innerHTML = ob.innerHTML;
			ob.parentNode.replaceChild(ob2,ob);
		}
	}
}
}
function get_object_data(ob)
{
	for(var i=0;i<ob.childNodes.length;i++)
	{
		var n = ob.childNodes[i];
		if(n.nodeName=="PARAM" && n.name=="movie")
		{
			return n.value;
		}
	}
}

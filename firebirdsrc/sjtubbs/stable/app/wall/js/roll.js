function addNewTopic(data){
	data=data[0];
	//console.log(data);
	$("#roll_main").append('<div class="span5 well newtopic topic" id="'+data.file_id+'"></div>');
	//$("#"+data.file_id).append('<h2><span class="c32">'+data.content.title+'</span></h2>');
	content=data.content_lines.join("<br/>");
	$("#"+data.file_id).append('<div class="title">主题: '+data.content.title+'</div>').hide();
	$("#"+data.file_id).append('<div class="content">'+content+'</div>').hide();
	$("#"+data.file_id).append('<div class="remove"><i class="icon-remove-sign"></i></div>');
	return data.file_id;
}
function addReTip(data){
	data=data[0];
	//console.log(data)
	$("#"+data.reid+" >.content").prepend('<div class="span4 reTip newretip" id="'+data.file_id+'"></div>');
	content=data.content_lines.join("<br/>");
	$("#"+data.file_id).append('<div>'+content+'</div>');
	return data.file_id;	
	
}

function addTip(data){
	if($("#"+data[0].file_id).length!=0) {console.log("123");return;}
	//console.log(data.reid);
	if(data[0].reid==data[0].file_id) addNewTopic(data);
	else addReTip(data);
}

function loadTips(data){
	//console.log(data);
	data=data[0];
	//console.log(data);
	for(var i=0;i<data.articles.length;i++){
		$("head").append('<script src="/api/article?board='+data.board+'&file='+data.articles[i].file+'&callback=addTip&pretty=1"></script>');
	}
}
var page=230;
var moni=function(){
	$("head").append('<script src="/api/board?board=SJTUNews&format=json&pretty=1&callback=loadTips&page='+page+'"></script>');
	page+=1;
}
setInterval(moni,5000);
setInterval(function(){
	$(".newretip").parents(".topic").addClass("new");
	$(".newtopic").addClass("new");
	$(".new").prependTo("#roll_main");
	
	$(".newretip,.newtopic").hide().slideDown("slow");
	$(".new").removeClass("new");
	$(".newretip").removeClass("newretip");
	$(".newtopic").removeClass("newtopic");

},1000);
$(".remove").live("click",function(){$(this).parents(".topic").remove();})

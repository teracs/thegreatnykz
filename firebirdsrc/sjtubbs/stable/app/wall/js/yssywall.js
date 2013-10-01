  function renderTopicSingle(data){

	var tip_template=_.template($("#topic-template").html());	
	data[0]["alertType"]=alertType(data[0]);
	if($("#main .topic").length >4 ) $("#main .topic:eq(3)").after(tip_template(data[0]));
	else $("#main").append(tip_template(data[0]));
	
	$("#"+data[0].file_id).hide().slideDown("slow");
  }
  function renderReplySingle(data){
	var tip_template=_.template($("#reply-template").html());	
	$("#"+data[0].reid+" .replies").prepend(tip_template(data[0]));
  
	$("#"+data[0].file_id).hide().slideDown("slow");
  }
  
$(".activate").live("click",function(){
	$(".topic_active").removeClass("topic_active");
	$(this).parents(".topic").prependTo("#main").addClass("topic_active");
});
$(".close").live("click",function(){
	$(this).parents(".topic").slideUp("slow",function(){$(this).remove()});
});

board="test";
load_interval=5000;
int_obj=null;
init_page=40;//940;
page_now=init_page;

function resetInt(time){
	if(!time) time=load_interval;
	load_interval=time;
	if(time==-1){
		time=load_interval=parseInt($("#frequency").val());
	}
	if(int_obj) clearInterval(int_obj);
	int_obj=setInterval(loadBoard,time);
}
function loadBoard(){
	$("head").append('<script src="/api/board?board='+board+'&pretty=1&callback=parseBoardData&page='+page_now+'"></script>');
	page_now+=1;
}
function parseBoardData(data){
	_.each(data[0].articles,function(article){
		if($("#"+article.file_id).length==0){
			$("head").append('<script src="/api/article?board='+data[0].board+'&file=M.'+article.file_id+'.A&callback=parseArticle&pretty=1"></script>');
		}		
	});
}
function parseArticle(data){
	if(data[0].file_id == data[0].reid) renderTopicSingle(data);
	else renderReplySingle(data);
}
resetInt();

function alertType(data){
	if(data.content.text_lines[0]=="#scaret好帅#") return "alert-success";
	else if(data.content.text_lines[0]=="#水源墙彩蛋#") return "alert-error";
	else if(data.content.text_lines[0]=="#饮水思源 爱国荣校#") return "";
	return "alert-info";
}

$(function(){
	
	$("#main").sortable();
	$("#frequency").val(load_interval);
});

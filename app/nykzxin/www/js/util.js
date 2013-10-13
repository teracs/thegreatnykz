(function($){  
        $.fn.serializeJson=function(){  
            var serializeObj={};  
            var array=this.serializeArray();  
            var str=this.serialize();  
            $(array).each(function(){  
                if(serializeObj[this.name]){  
                    if($.isArray(serializeObj[this.name])){  
                        serializeObj[this.name].push(this.value);  
                    }else{  
                        serializeObj[this.name]=[serializeObj[this.name],this.value];  
                    }  
                }else{  
                    serializeObj[this.name]=this.value;   
                }  
            });  
            return serializeObj;  
        };  
    })(jQuery);  

pageStack = {}
pageStack.pageids = ["page_login"];
pageStack.historyItr = 0;
pageStack.push = function(pagetext){
  var pageid = "page_" + new Date().getTime();
  pageStack.pageids.push(pageid);
  pageStack.historyItr = pageStack.pageids.length;
  $(pagetext).attr("id",pageid).appendTo("body");
  $.mobile.changePage($("#" + pageid),{transition: "slide"});
  return pageid;
};
pageStack.pop = function(){
  if(pageStack.pageids.length -2 < 0) return false;
  pageStack.historyItr = pageStack.pageids.length -2;
  var pageid = pageStack.pageids[pageStack.historyItr];
  var pageidToDelete = pageStack.pageids.pop();
  $.mobile.changePage($("#" + pageid),{transition: "slide",reverse:true});
  setTimeout(function(){$("#" + pageidToDelete).remove();},5000);
  return pageid;
};

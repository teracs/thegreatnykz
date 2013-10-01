var isIE = checkIE();

function checkIE() {
	var detect = navigator.userAgent.toLowerCase();
	return checkIt('msie')||checkIt('chrome');
	
	
	function checkIt(string)
{
	place = detect.indexOf(string) + 1;
	thestring = string;
	return place;
}

}



function activeUpload() {
		//alert(document.getElementsByName('board')[0].value);
		try {attachIframe.document.getElementById('fileupload_form_board').value = document.getElementsByName('board')[0].value;} catch(e){attachIframe.location='/html/attach.html';}
			
		if(isIE) {
			try {attachIframe.document.getElementById('fileupload_file').click();} catch(e){attachIframe.location='/html/attach.html';}
			
		} else {
			var fileInputText = '<input name=up type=file id="fileupload_file" onchange="gochange();"><br />';
			container = document.getElementById('attach');
			container.innerHTML = fileInputText;
			//try {document.getElementById('fileupload_file').click();} catch(e){alert('err');}
			
		}
		

}	
function gochange(){
	var o = attachIframe.document.getElementById('fileupload_file');
	var p = document.getElementById('fileupload_file').cloneNode(true);
	attachIframe.document.getElementById('fileupload_form').replaceChild(p,o);
	//alert(attachIframe.document.getElementById('fileupload_file').value);
	attachIframe.document.getElementById('fileupload_form').submit();
	document.getElementById('attach').innerHTML = '文件上传中……';
}
	
function applyattach(attachurl)
{	
  document.location = '/html/attach.html';
  parent.document.getElementById('attach').innerHTML = '文件上传成功！<label class="botton" onclick="javascript:activeUpload();">上传新文件</label>';
  container = parent.document.getElementById('attachC');
  subline = parent.document.createElement('div');
  var fdate = new Date();
  subline_div_id = 'attach' + fdate.getTime();
  subline.setAttribute('id', subline_div_id);
  add_string = ' <label onclick="javascript:addAttach(\'' + attachurl + '\');">添加</label>';
  close_string = ' <label onclick="javascript:delAttach(\'' + subline_div_id + '\');">删除</label>';
  if (/jpeg|jpg|gif|png/.exec(attachurl.toLowerCase()))
  {
	  show_img= " <img src="+attachurl+" width=30 /> ";
  }
  else
  {
	  show_img = "";
  }
  subline.innerHTML = attachurl + show_img + add_string + close_string;
  container.appendChild(subline);
  
}
function delAttach(divid)
{
  document.getElementById('attachC').removeChild(document.getElementById(divid));
}

//-------------------------------------------------------------------------

function addAttach(attachurl){
//  original code
//  document.getElementById('text').value += '\n' + attachurl + '\n';
//  code for add picurl at input location
    var text=document.getElementById("text");
    var textLength=text.value.length;
    text.focus();
    if(typeof document.selection !="undefined")
    {
        document.selection.createRange().text='\n' + attachurl + '\n';
    }
    else
    {
        text.value=text.value.substr(0,text.selectionStart)+'\n' + attachurl + '\n'+text.value.substring(text.selectionStart,textLength);
    }
}
document.write('<div id="attach"><label class="botton" onclick="javascript:activeUpload();">上传附件</label></div><div id="attachC"></div>');
document.write('<iframe src="/html/attach.html" id="attachIframe" name="attachIframe" width="0" height="0" scrolling="no" frameborder="0"></iframe>');
	

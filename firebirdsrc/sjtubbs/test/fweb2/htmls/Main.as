package 
{
     import flash.display.Sprite;  
     import adobe.utils.CustomActions;  
     import flash.display.*;  
     import flash.events.*;  
     import flash.text.StyleSheet;  
     import flash.text.TextField;  
     import flash.utils.Timer;  
     import flash.external.ExternalInterface;  
	 import flash.system.System;
	 /**
	 * ...
	 * @author aay
	 */
	public class Main extends Sprite 
	{
		public function Main():void 
		{
             stage.scaleMode = StageScaleMode.NO_SCALE;
			 stage.align = StageAlign.TOP_LEFT;
             init();  
		}
		private function init(e:Event = null):void 
		{
            var txt:TextField = new TextField();
            var css:StyleSheet = new StyleSheet();
            css.parseCSS(".clipboard {color:#014DA2;text-decoration:none;fontSize:12px;} .clipboard:hover{text-decoration:underline overline}");
            txt.styleSheet = css;
			if (loaderInfo.parameters.text != null) {
				  txt.htmlText = "<a href='event:#' class='clipboard'>"+loaderInfo.parameters.text+"</a>";
			}else {
				  txt.htmlText = "<a href='event:#' class='clipboard'>复制</a>";
			}
			txt.addEventListener(MouseEvent.CLICK, clickHandler);
            stage.addChild(txt);
		}
         private function clickHandler(evt:Event):void {  
             var content:String = loaderInfo.parameters.data||" ";  
             System.setClipboard(content);
             ExternalInterface.call("copySuccess");  
         }
	}
}
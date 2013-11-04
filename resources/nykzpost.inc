<?php

include_once 'nykz.class.inc';

function _handler_nykzpost_show($boardName,$filename){
  $post = NYKZpost::getPost($boardName,$filename);
  return array(
    'post' => $post,
    'boardName' => $boardName,
    'filename' => $filename,
    );
}

function _handler_nykzpost_create($boardName,$title,$content,$reid=NULL){
  global $user;
  $time = date('Y年m月d日 H:i:s l', time());
  $the_content = "发信人：".$user->name."， 信区：".$boardName.
  "\n标  题：".$title.
  "\n发信站：南阳客栈BBS (".$time."), 来自南阳客栈信！".
  "\n\n".$content."\n\n--\n\n\033[1;32m※ 来源:·南洋客栈信！·\033[m\n";
  $res = NYKZpost::createPost($boardName,$user->name,$title,$the_content,$reid);
  return $res;
}

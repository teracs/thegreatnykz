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

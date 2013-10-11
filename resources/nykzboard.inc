<?php
include_once 'nykz.class.inc';
// @返回所有南阳客栈板面列表
function _handler_nykzboard_index(){
  $boards = NYKZboard::getAllBoards();
  return $boards;
}

function _handler_nykzboard_show($boardName,$start,$count){
  $postIndexes = NYKZboard::getPostIndex($boardName,$start,$count);
  $postIndexes = $postIndexes["posts"];
  return array(
    "posts" => $postIndexes,
    "board" => $boardName,
    "start" => $postIndexes[0]["index"],
    "count" => count($postIndexes),
    );
}

function _access_nykzboard($boardName){

  global $user;
  $userid = $user->name;

  if(!$userid){
    return FALSE;
  }
  $board = NYKZboard::getPostIndex($boardName);
  $allow = $board['allow'];
  if(!count($allow)) return TRUE;
  if(in_array($userid, $allow)) return TRUE;
  return FALSE;
}

<?php
include_once 'nykz.class.inc';
// @返回所有南阳客栈板面列表
function _handler_nykzboard_index(){
  $boards = NYKZboard::getAllBoards();
  return $boards;
}

function _handler_nykzboard_show($boardName,$start,$count){
  $postIndexes = NYKZboard::getPostIndex($boardName);
  $postIndexes = $postIndexes["posts"];
  $max = count($postIndexes) -1;
  //return $max;
  if($start > $max) $start = $max - $count;
  if($start < 0) $start = 0;
  return array(
    "posts" => array_slice($postIndexes, $start, $count),
    "board" => $boardName,
    "start" => $start,
    "count" => $count,
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

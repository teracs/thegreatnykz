<?php

class NYKZuser{
  // @return {teracs:{userid,teracs,...}...}
  public $info;
  public $userid;
  static function getAllUsers(){
    $users_json = file_get_contents("http://localhost:8080/bbsallusers");
    $users = json_decode($users_json,true);
    $users = $users['message'];
    if(!$users) throw new Exception("fail to call webservice");
    return $users;
  }
  // true of false.
  static function checkPasswd($userid,$pw){
    $check_json = file_get_contents("http://localhost:8080/bbscheck?user=$userid&pw=$pw");
    $check = json_decode($check_json,true);
    if($check['message']['userid']) return true;
    else return false;
  }

  function __construct($userid){
    $this->userid = $userid;
  }
}

class NYKZboard{
  // @return {NetHack:{title,filename,allow}}
  static function getAllBoards(){
    $boards_json = file_get_contents("http://localhost:8080/bbsboard");
    $boards = json_decode($boards_json,true);
    $boards = $boards['message'];
    if(!$boards) throw new Exception("fail to call webservice");
    return $boards;
  }
  // @return {boardName:Nethack,posts:[...],allow:[]}
  static function getPostIndex($boardName){
    $posts_json = file_get_contents("http://localhost:8080/bbspostindex?boardName=$boardName");
    $posts = json_decode($posts_json,true);
    $posts = $posts['message'];
    if(!$posts) throw new Exception("fail to call webservice");
    return $posts;
  }
}

class NYKZpost{
  static function getPost($boardName,$filename){
    $post_json = file_get_contents("http://localhost:8080/bbspost?boardName=$boardName&filename=$filename");
    $post = json_decode($post_json,true);
    $post = $post['message'];
    if(!$post) throw new Exception("fail to call webservice");
    return $post;
  }
}

$__main__ = 1;
if($__main__){
  //print_r(NYKZuser::getAllUsers());
  //print_r(NYKZboard::getAllBoards());
  //$user = new NYKZuser('Teracs','bybicycl');
  //print_r($user->info);
  //print_r(NYKZboard::getPostIndex('NetHack'));
  print_r(NYKZpost::getPost('NetHack','M.1374330501.A'));
}

<?php

include_once "../config.php";

class NYKZuser{
  // @return {teracs:{userid,teracs,...}...}
  public $info;
  public $userid;
  // 返回所有的用户id
  static function getAllUsers(){
    $users_json = file_get_contents(PYURL."/bbsuserindex");
    $users = json_decode($users_json,true);
    $users = $users['message'];
    if(!$users) throw new Exception("fail to call webservice");
    return $users;
  }
  static function getUser($userid){
    $user_json = file_get_contents(PYURL."/bbsuser/".$userid);
    $user = json_decode($user_json);
    return $user->message;
  }

  // true of false.
  static function checkPasswd($userid,$pw){
    $check_json = file_get_contents(PYURL."/bbscheck.json?user=$userid&pw=$pw");
    $check = json_decode($check_json,true);
    return (int)in_array($userid,array_keys($check['message']));
  }

  function __construct($userid){
    $this->userid = $userid;
  }
}

class NYKZboard{
  // @return {NetHack:{title,filename,allow}}
  static function getAllBoards(){
    $boards_json = file_get_contents(PYURL."/bbsboard");
    $boards = json_decode($boards_json,true);
    $boards = $boards['message'];
    if(!$boards) throw new Exception("fail to call webservice");
    return $boards;
  }
  static function boardIndex(){
    $boards_json = file_get_contents(PYURL."/boardindex");
    $boards = json_decode($boards_json);
    return $boards;
  }
  // @return {boardName:Nethack,posts:[...],allow:[]}
  static function getPostIndex($boardName,$startfrom1=999999,$count=20){
    $posts_json = file_get_contents(PYURL."/bbspostindex?boardName=$boardName&startfrom1=$startfrom1&count=$count");
    $posts = json_decode($posts_json,true);
    $posts = $posts['message'];
    if(!$posts) throw new Exception("fail to call webservice");
    return $posts;
  }
}

class NYKZpost{
  static function getPost($boardName,$filename){
    $post_json = file_get_contents(PYURL."/bbspost?boardName=$boardName&filename=$filename");
    $post = json_decode($post_json,true);
    $post = $post['message'];
    if(!$post) throw new Exception("fail to call webservice");
    return $post;
  }
  static function createPost($boardName,$author,$title,$content,$reid=NULL){
    $req = array(
      'boardName' => $boardName,
      'user' => $author,
      'title' => $title,
      'content' => $content,
      'reid' => $reid,
      );
    $options = array(
      'http' => array(
        'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
        'method'    => 'POST',
        'content'   => http_build_query($req),
        ),
      );
    $context  = stream_context_create($options);
    // print_r($context);
    $result_json = file_get_contents(PYURL."/bbspost.json",false,$context);
    $result = json_decode($result_json);
    return $result->message;
  }
}

$__main__ = 0;
if($__main__){
  //print_r(NYKZuser::getAllUsers());
  //print_r(NYKZboard::getAllBoards());
  //$user = new NYKZuser('Teracs','bybicycl');
  //print_r($user->info);
  //print_r(NYKZboard::getPostIndex('NetHack'));
  //print_r(NYKZpost::getPost('NetHack','M.1374330501.A'));
}

<?php
include 'nykz.class.inc';

// @调用此函数，输入南阳客栈的用户名密码，就能够在drupal中登陆。如
// drupal中没有此用户，也会自动创建。
function _handler_login($userid,$pw){
  if(NYKZuser::checkPasswd($userid,$pw)){
    // valid user
    $account = user_load_by_name($userid);
    if(!$account)
      $account = _handler_register($userid,$pw);
    global $user;
    $user = $account;
    user_login_finalize();
    services_remove_user_data($user);
    return $user;
  }
  return False;
}

// @返回所有南阳客栈用户列表
function _handler_nykzuser_index(){
  $users = NYKZuser::getAllUsers();
  return $users;
}
function _handler_nykzuser_retrieve($userid){
  $user = NYKZuser::getUser($userid);
  return $user;
}

function _handler_register($userid,$pw){
  $roles = array();
  $roles[array_search('authenticated user', user_roles())] = 'authenticated user';
  $roles[array_search('nykzuser', user_roles())] = 'nykzuser';
  $fields = array(
    'name' => $userid,
    'mail' => $userid."@nykz.net",
    'pass' => $pw,
    'status' => 1,
    'init' => $userid."@nykz.net",
    'roles' => $roles,
  );
  $account = user_save('',$fields);
  return $account;
}

function _handler_whoami(){
  global $user;
  return array(
    'uid' => $user->uid,
    'name' => $user->name
    );
}

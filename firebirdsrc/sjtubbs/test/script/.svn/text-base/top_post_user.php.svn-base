#!/usr/bin/php
<?php

if($hour == 0) {
            $count_day = date("Yz");
} else {
            $count_day = date("Yz")+1;
}
$today_table = "posts_daily_".$count_day;
echo $today_table;

$db_host = "localhost:/var/run/mysqld/mysqld.sock";
$db_user = "root";
$db_pass = "qwer415263";



$junkboardfile = "/home/bbs/bbshome/etc/junkboards";
$junkfile_handle = fopen($junkboardfile,"r");
$junkboards = "''";
while (!feof ($junkfile_handle)) { 
        $buffer = fgets($junkfile_handle, 4096); 
        if(empty($buffer)) {
                continue;
        }   
        if($buffer[0] == '#') {
                continue;
        }
        $buffer = str_replace("\n","",$buffer);
        $junkboards .= ",'".$buffer."'";
}               
fclose ($junkfile_handle);

/*
   * init mysql connection
   */
$link = mysql_connect($db_host, $db_user, $db_pass) or die("Could not
	connect : 
	" . mysql_error());
mysql_select_db("bbs") or die("Could not select database");

$query = "SELECT * , count( * ) AS sum FROM `".$today_table."` where board not in (".$junkboards.") GROUP BY user_id ORDER BY sum DESC LIMIT 0 , 50 ";
$result = mysql_query($query);

$file_path = "/home/bbs/bbshome/etc/top50users";
$handle = fopen($file_path,"w");
$content = "id\tposts\n";
while ($row = mysql_fetch_assoc($result) ) {
    $content = $content."\n\n[1;41;33m".$row[user_id]."\t".$row[sum]."[m\n";
    $query = "select * ,count(*) as sum from ".$today_table." where user_id like '".$row[user_id]."' and board not in (".$junkboards.") group by board order by sum DESC";
    $result_user = mysql_query($query);
    $i = 0;
    while($row_user = mysql_fetch_assoc($result_user)) {
    	$i++;
    	$content .= $row_user[board]." ".$row_user[sum]."|";
	if($i%5 == 0) {
		$content .= "\n";
		}
	}
}
fwrite($handle,$content);
fclose($handle);



?>

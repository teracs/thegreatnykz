#!/usr/local/bin/php
<?php

if($hour == 0) {
            $count_day = date("Yz");
} else {
            $count_day = date("Yz")+1;
}
$today_table = "posts_daily_".$count_day;
echo $today_table;

$db_host = "202.120.3.4";
$db_user = "report";
$db_pass = "q1w2e3r4";

/*
   * init mysql connection
   */
$link = mysql_connect($db_host, $db_user, $db_pass) or die("Could not
	connect : 
	" . mysql_error());
mysql_select_db("bbs") or die("Could not select database");

$query = "SELECT * , count( * ) AS sum
FROM `".$today_table."` 
GROUP BY user_id
ORDER BY sum DESC 
LIMIT 0 , 50 ";
$result = mysql_query($query);

$file_path = "/home/bbs/bbshome/etc/top50users";
$handle = fopen($file_path,"w");
$content = "id\tposts\n";
while ($row = mysql_fetch_assoc($result) ) {
    $content = $content.$row[user_id]."\t".$row[sum]."\n";
}
fwrite($handle,$content);
fclose($handle);



?>

#!/usr/local/bin/php
<?
/*
* Name:report.php
* Type:script Language:php
* Written by cf@yssy
* Used to replace poststat,supply more detail from db record
* This script runs hourly on every 10min after Hour.
* Created:2005/05/09
*/

include("/home/bbs/bbshome/etc/district_data.inc.php");
?>
<?

function get_recommend_boards($path,$dpath,$num,$sp) {
    if(!($handle = fopen ($path,"r"))) {
	return 0;
    }
    if(!($dhandle = fopen ($dpath,"w"))) {
	return 0;
    }
    $row = 1;
    while ($data = fgetcsv ($handle, 1000, " ")) { 
	if($data[0][0] == '#') {
	    continue;
	}
	   $num = count ($data); 
	   if($num != 2) {
	   	echo "'$data[0]','$data[1]','$data[2]'\n";
	       continue;
	   }
	   
	   $line = sprintf("%s<a
		   href=\"/bbsdoc?board=%s\"
		   target=\"_self\">%s</a>%s",$line,$data[0],$data[1],$sp);
	         $row++; 
    } 
    
    fclose ($handle); 
    fwrite($dhandle,$line);
    fclose ($dhandle);
   
}
function cmp($a, $b) { 
   if ($a['height'] == $b['height']) { 
          return 0; 
	} 
     return ($a['height'] < $b['height']) ? 1 : -1; 
} 
		
function
top($post_sum_array,$junkboards,$district,$filepath,$num,$count_day) {
    
$filehandle = fopen($filepath,"w");
$filehandle_web = fopen($filepath."_web.html","w");
$line = sprintf("%s","          [1;34m-----[37m=====[41m ±¾ÈÕ10´óÈÈÃÅ»°Ìâ [40m=====[34m-----[0m\n\n");
fwrite($filehandle,$line);
$line_web = sprintf("%s","<table width=100% border=0>");
fwrite($filehandle_web,$line_web);
$line = NULL;
$line_web = NULL;
$j = 0;
$i = 0;
/*for($i = 0; $i < $num; $j++) {*/
foreach($post_sum_array as $post_sum_array_row) {
    if($i >= $num) {
	echo $i."\n";
	break;
    }
    	if($post_sum_array_row[reid] == '0') {
	    continue;
	}
	if(in_array($post_sum_array_row[board],$junkboards)) {
		continue;
	}
/*	$query_junk = "select * from junk where time >
	    '".$post_sum_array_row[rawtime]."' and board =
	    '".$post_sum_array_row[board]."' and filename LIKE
	    '".$post_sum_array_row[filename]."'";
	echo $query_junk;
	$result = mysql_query($query_junk) or die("Query failed : " .
		mysql_error());
	if(mysql_num_rows($result) > 0) {
	    echo "PASS!!!!!!!!!!!!!!!!!!";
	    continue;
	}*/
	
	if($district == NULL ||
		in_array($post_sum_array_row[board],$district)) {
	    $cmdline = "/home/bbs/script/onboard ".$post_sum_array_row[board]." ".$post_sum_array_row[reid];
	            $onboard = exec($cmdline);
	            if($onboard==0 or intval($onboard) < $post_sum_array_row[row]/7) continue;
		$today_table = "posts_daily_".$count_day;
	    $query = "SELECT title,user_id  FROM `".$today_table."`  WHERE board =
		'".$post_sum_array_row[board]."' AND `reid` =
		".$post_sum_array_row[reid]." ORDER BY `id` ASC  LIMIT 0 , 1";
	    $post_master_result = mysql_query($query) or die("Query failed :
		    " .mysql_error());
	    if(mysql_num_rows($post_master_result) == 0)
                continue;
	    $post_master_row = mysql_fetch_assoc($post_master_result);
	    
	    if($post_master_row['title'][0]=='R' && $post_master_row['title'][1]=='e' && $post_master_row['title'][2]==':')
	    {
		continue;
	    } else {
		
	    	$post_sum_array_row[post_master] = $post_master_row['user_id'];
		$post_sum_array_row[title] = $post_master_row['title'];
	    }
	if( !isset($limit[$post_sum_array_row[board]] )) {
		$limit[$post_sum_array_row[board]] = 1;
		} else {
			$limit[$post_sum_array_row[board]]++;
		}
	if($limit[$post_sum_array_row[board]] > 3) {
		continue;
		}
	$line = sprintf("%s[1;37mµÚ[1;31m%2d[37mÃû [37mÐÅÇø:[33m%-12s[37m¡¾[32m%s[37m¡¿[36mP:%4d U:%3d I:%3d [35m%+12s\n",$line,$i+1,$post_sum_array_row[board],date("Md H:m",$post_sum_array_row[reid]),$post_sum_array_row[sum],$post_sum_array_row[user_sum],$post_sum_array_row[ip_sum],$post_sum_array_row[post_master]);
	$line_web = sprintf("%s<tr><td width=110>[<a
		href=\"/bbsdoc?board=%s\"
		target=\"_self\">%12s</a>]</td><td><a
		href=\"/bbstcon?board=%s&reid=%d\"
		target=\"_self\">%s
		</a></td><td
		width=110>%s</td></tr>",$line_web,$post_sum_array_row[board],$post_sum_array_row[board],$post_sum_array_row[board],$post_sum_array_row[reid],$post_sum_array_row[title],$post_sum_array_row[post_master]);
	$line = sprintf("%s[37m±êÌâ : [1;44;37m%-60.60s[40m\n",$line,$post_sum_array_row[title]);
	$i++;
	}
}
fwrite($filehandle,$line);
fclose($filehandle);
$line_web = $line_web."</table>";
fwrite($filehandle_web,$line_web);
fclose($filehandle_web);
}

?>
<?
/*
**get the junk boards
*/
$junkboardfile = "/home/bbs/bbshome/etc/junkboards";
$junkfile_handle = fopen($junkboardfile,"r");
$junkboards = array();
while (!feof ($junkfile_handle)) { 
	$buffer = fgets($junkfile_handle, 4096); 
	if(empty($buffer)) {
		continue;
	}
	if($buffer[0] == '#') {
		continue;
	}
	$buffer = str_replace("\n","",$buffer);
	array_push($junkboards, $buffer);
} 
fclose ($junkfile_handle); 
?>
<?
/*
db account
change it to fit your setting
*/
$db_host = "localhost:/var/run/mysqld/mysqld.sock";
$db_user = "root";
$db_pass = "qwer415263";

/*
* init mysql connection
*/
$link = mysql_connect($db_host, $db_user, $db_pass) or die("Could not connect : " . mysql_error());
mysql_select_db("bbs") or die("Could not select database"); 

/*
* if hour == 23,creat table which is used tomorrow.
*/
$hour = date("H");
if($hour == 23) {
	$tomorrow = date("z") + 2;
	$table_name = "posts_daily_".date("Y").$tomorrow;
	$query = "
	CREATE TABLE `bbs`.`".$table_name."` (
	`id` bigint( 64 ) NOT NULL AUTO_INCREMENT ,
	`user_id` varchar( 16 ) NOT NULL default '',
	`ip_addr` varchar( 32 ) NOT NULL default '',
	`time` timestamp( 14 ) NOT NULL ,
	`board` varchar( 32 ) NOT NULL default '',
	`filename` varchar( 128 ) NOT NULL default '',
	`title` varchar( 128 ) NOT NULL default '',
	`content` text NOT NULL ,
	`ext` varchar( 64 ) NOT NULL default 'tel',
	`reid` int( 16 ) NOT NULL ,
	PRIMARY KEY ( `id` ) 
	) TYPE = MYISAM ;
	";
	$result = mysql_query($query) or die("Query failed : " . mysql_error());
}
if($hour == 0) {
	$count_day = date("Yz");
} else {
	$count_day = date("z")+1;
	$count_day = date("Y").$count_day;
}
$today_table = "posts_daily_".$count_day;
/*
*hourly work
*/
#step1: clean the table used next hour *no need
/*
$next_hour = $hour + 1;
if($next_hour == 24) {
	$next_hour = '00';
}
$next_table = "posts".$next_hour;
$query = "TRUNCATE TABLE ".$next_table;
$result = mysql_query($query) or die("Query failed : " . mysql_error());
*/
#analyze 
$previous_hour = $hour - 1;
if($hour == 0) {
	$previous_hour = 23;
	}
$previous_table = sprintf("posts%02d",$previous_hour);
$query = "
	INSERT INTO `bbs`.`".$today_table."` (user_id,ip_addr,time,board,filename,title,content,ext,reid)
	SELECT user_id,ip_addr,time,board,filename,title,content,ext,reid 
	FROM `bbs`.`".$previous_table."` ;
	";
$result = mysql_query($query) or die("Query failed : " . mysql_error());

/* get most active boards last hour*/
$query = "SELECT * , count( * ) AS sum FROM ".$previous_table." GROUP BY
board ORDER BY `sum` DESC LIMIT 0 , 30 ";
$result = mysql_query($query) or die("Query failed : " . mysql_error());
$active_boards =
"/home/bbs/bbshome/httpd/include/aboards.html";
$handle = fopen($active_boards,"w");
$line = "<table width=100% border=0>\n";
while ($row = mysql_fetch_assoc($result) ) {
    if(in_array($row[board],$junkboards)) {
	                continue;
			        }
    $line = sprintf("%s\n<tr><td><a href=\"/bbsdoc?board=%s\"
	    target=\"_self\">%s</a></td></tr>",$line,$row['board'],$row['board']);
}
$line = $line."</table>";
fwrite($handle,$line);
fclose($handle);
$line = NULL;


$result = mysql_query($query) or die("Query failed : " . mysql_error());


$query = "TRUNCATE TABLE ".$previous_table;
$result = mysql_query($query) or die("Query failed : " . mysql_error());

#get today's top10
$query = "select * from ".$today_table." order by board ,reid";
$result = mysql_query($query) or die("Query failed : " . mysql_error());
$today_sum = mysql_num_rows($result);
$cur['sum'] = 0;
$cur['board'] = NULL;
$cur['reid'] = 0;
$cur['post_master'] = NULL;
$post_sum_array = array();
$post_user_array = array();
$post_ip_array = array();
while ($row = mysql_fetch_assoc($result) ) {
    if($row['reid'] == '0') {
	continue;
    }
	if($cur_board != $row['board']) {
		$cur['sum'] = $cur_sum;
		$cur['user_sum'] = $post_user_sum;
		$cur['ip_sum'] = $post_ip_sum;
		$cur['height'] = $post_ip_sum + sqrt($post_user_sum) + sqrt(sqrt($cur_sum));
		array_push($post_sum_array,$cur);
		$cur['board'] = $row['board'];
		$cur['reid'] = $row['reid'];
		$cur['post_master'] = $row['user_id'];
		$cur['filename'] = $row['filename'];
		$cur['title'] = str_replace("Re: ","",$row['title']);
		$cur_board = $row['board'];
		$cur_reid = $row['reid'];
		$cur_sum = 1;
		$post_user_sum = 1;
		$post_ip_sum = 1;
		$post_user_array = array();
		$post_ip_array = array();
		continue;
	}
	if($cur_reid != $row['reid']) {
		$cur['sum'] = $cur_sum;
		$cur['user_sum'] = $post_user_sum;
                $cur['ip_sum'] = $post_ip_sum;
		$cur['height'] = $post_ip_sum + sqrt($post_user_sum) + sqrt(sqrt($cur_sum));
                array_push($post_sum_array,$cur);
		$cur['reid'] = $row['reid'];
		$cur['post_master'] = $row['user_id'];
		$cur['filename'] = $row['filename'];
		$cur['title'] = str_replace("Re: ","",$row['title']);
		$cur_reid = $row['reid'];
		$cur_sum = 1;
		$post_user_sum = 1;
                $post_ip_sum = 1;
		$post_user_array = array();
		$post_ip_array = array();
		continue;
	}
	if(!in_array($row[user_id],$post_user_array)) {
		array_push($post_user_array,$row['user_id']);
		$post_user_sum++;
	}
	if(!in_array($row['ip_addr'],$post_ip_array)) {
		array_push($post_ip_array,$row['ip_addr']);
		$post_ip_sum++;
	}
	
	$cur['time'] = $row['time'][4].$row['time'][5]." ".$row['time'][6].$row['time'][7]." ".$row['time'][8].$row['time'][9].":".$row['time'][10].$row['time'][11];
	$cur['rawtime'] = $row['time'];
	$cur_sum++;
}
usort($post_sum_array,"cmp");

$T10allfilepath = '/home/bbs/bbshome/0Announce/bbslist/newtop100';
top($post_sum_array,$junkboards,NULL,$T10allfilepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/new0top10';
top($post_sum_array,$junkboards,$district_0,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/new1top10';
top($post_sum_array,$junkboards,$district_1,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/new2top10';
top($post_sum_array,$junkboards,$district_2,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/new3top10';
top($post_sum_array,$junkboards,$district_3,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/new4top10';
top($post_sum_array,$junkboards,$district_4,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/new5top10';
top($post_sum_array,$junkboards,$district_5,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/new6top10';
top($post_sum_array,$junkboards,$district_6,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/new7top10';
top($post_sum_array,$junkboards,$district_7,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/new8top10';
top($post_sum_array,$junkboards,$district_8,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/new9top10';
top($post_sum_array,$junkboards,$district_9,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/newatop10';
top($post_sum_array,$junkboards,$district_a,$T10filepath,10,$count_day);

$T10filepath = '/home/bbs/bbshome/0Announce/bbslist/newbtop10';
top($post_sum_array,$junkboards,$district_b,$T10filepath,10,$count_day);

if($hour == 23) {
system("cp -f /home/bbs/bbshome/0Announce/bbslist/newtop100 /home/bbs/bbshome/0Announce/bbslist/newtop100.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/new0top10 /home/bbs/bbshome/0Announce/bbslist/new0top10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/new1top10 /home/bbs/bbshome/0Announce/bbslist/new1top10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/new2top10 /home/bbs/bbshome/0Announce/bbslist/new2top10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/new3top10 /home/bbs/bbshome/0Announce/bbslist/new3top10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/new4top10 /home/bbs/bbshome/0Announce/bbslist/new4top10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/new5top10 /home/bbs/bbshome/0Announce/bbslist/new5top10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/new6top10 /home/bbs/bbshome/0Announce/bbslist/new6top10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/new7top10 /home/bbs/bbshome/0Announce/bbslist/new7top10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/new8top10 /home/bbs/bbshome/0Announce/bbslist/new8top10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/new9top10 /home/bbs/bbshome/0Announce/bbslist/new9top10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/newatop10 /home/bbs/bbshome/0Announce/bbslist/newatop10.last");
system("cp -f /home/bbs/bbshome/0Announce/bbslist/newbtop10 /home/bbs/bbshome/0Announce/bbslist/newbtop10.last");


}

/*
   get recommend board
*/
$rboard_raw =
'/home/bbs/bbshome/0Announce/groups/GROUP_0/sysop/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboard0.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_1/District1/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboard1.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_2/District2/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboard2.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_3/District3/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboard3.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_4/District4/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboard4.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_5/District5/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboard5.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_6/District6/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboard6.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_7/District7/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboard7.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_8/District8/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboard8.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_9/District9/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboard9.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_A/DistrictA/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboarda.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw = 
'/home/bbs/bbshome/0Announce/groups/GROUP_B/DistrictB/recommend_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboardb.html';
get_recommend_boards($rboard_raw,$drboard,5,"|");

$rboard_raw =
'/home/bbs/bbshome/0Announce/groups/GROUP_0/sysop/recommend_boards_all';
$drboard = '/home/bbs/bbshome/httpd/include/rboardall.html';
get_recommend_boards($rboard_raw,$drboard,5,"<br>");

$rboard_raw =
'/home/bbs/bbshome/0Announce/groups/GROUP_0/sysop/new_boards';
$drboard = '/home/bbs/bbshome/httpd/include/rboardnew.html';
get_recommend_boards($rboard_raw,$drboard,5,"<br>");
/*
   get recommend posts
*/
$recommed_posts_file =
"/home/bbs/bbshome/httpd/include/recommend_all.html";

$line = '';
$line = "<table width=100% border=0>";
$handle = fopen($recommed_posts_file, "w");
$query = "SELECT * FROM `recommend` ORDER BY `id` DESC LIMIT 0 , 10";
$result = mysql_query($query) or die("Query failed : " . mysql_error());
while ($row = mysql_fetch_assoc($result) ) {
    $line = sprintf("%s
	    <tr><td width=110>
	    [<a href=\"/bbsdoc?board=%s\"
	    target=\"_self\">%s</a>]
	    </td><td>
	    <a href=\"/bbstcon?board=%s&reid=%d\"
	    target=\"_self\">%s</a>
	    </td><td>
	    %s
	    </td>
	    <td width=80>
	    %s
	    </td></tr>",
	    $line,$row['board'],$row['board'],$row['board'],$row['reid'],
	    $row['title'],date("Md H:m:s",$row['reid']),$row['user_id']);
}
$line = $line."</table>";

fwrite($handle,$line);
fclose($handle);
    
    
/*
   get found
*/
$found_posts_file =
"/home/bbs/bbshome/httpd/include/found.html";

$line = '';
$line = "        <marquee direction=\"up\" scrollAmount=\"1\" height=\"45\" onmouseover=stop() onmouseout=start()>\n";
$handle = fopen($found_posts_file, "w");
$query = "SELECT * FROM `search_found` ORDER BY `id` DESC LIMIT 0 , 15";
$result = mysql_query($query) or die("Query failed : " . mysql_error());
while ($row = mysql_fetch_assoc($result) ) {
    $line = sprintf("%s            <a href=\"/bbstcon,board,Search,reid,%s.html\" target=\"_self\">%s</a><br/>\n",
	    $line,$row['reid'],$row['title']);
}
$line = $line."        </marquee>\n";

fwrite($handle,$line);
fclose($handle);

/*
   get lost
*/
$lost_posts_file =
"/home/bbs/bbshome/httpd/include/lost.html";

$line = '';
$line = "        <marquee direction=\"up\" scrollAmount=\"1\" height=\"45\" onmouseover=stop() onmouseout=start()>\n";
$handle = fopen($lost_posts_file, "w");
$query = "SELECT * FROM `search_lost` ORDER BY `id` DESC LIMIT 0 , 15";
$result = mysql_query($query) or die("Query failed : " . mysql_error());
while ($row = mysql_fetch_assoc($result) ) {
    $line = sprintf("%s            <a href=\"/bbstcon,board,Search,reid,%s.html\" target=\"_self\">%s</a><br/>\n",
	    $line,$row['reid'],$row['title']);
}
$line = $line."        </marquee>\n";

fwrite($handle,$line);
fclose($handle);
    
/*
   get notice
*/
$lost_posts_file =
"/home/bbs/bbshome/httpd/include/notice.html";

$line = '';
$line = "        <marquee direction=\"up\" scrollAmount=\"1\" height=\"45\" onmouseover=stop() onmouseout=start()>\n";
$handle = fopen($lost_posts_file, "w");
$query = "SELECT * FROM `notice` ORDER BY `id` DESC LIMIT 0 , 15";
$result = mysql_query($query) or die("Query failed : " . mysql_error());
while ($row = mysql_fetch_assoc($result) ) {
    $line = sprintf("%s            <a href=\"/bbstcon,board,Notice,reid,%s.html\" target=\"_self\">%s</a><br/>\n",
	    $line,$row['reid'],$row['title']);
}
$line = $line."        </marquee>\n";

fwrite($handle,$line);
fclose($handle);
    
?>


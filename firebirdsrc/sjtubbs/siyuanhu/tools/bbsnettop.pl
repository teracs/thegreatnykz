#!/usr/bin/perl -w
# BBS 每天登陆次数排行榜
# write by alt@2002.04.21

use strict

$maxnum=100;
$date=`date +%Y年%m月%d日`;
chomp($date);
open(BBSNET,"</home/bbs/bbshome/reclog/bbsnet.log") || die "can not open file bbsnet.log";

while($line=<BBSNET>){
	my @array=split(/ +/,$line);
	$connectlist{$array[4]}+=1;
}
close(BBSNET);
$time=`date +%H点%M分`;
	format TITLE =
                [1;34m---[37m===[31m饮水思源 BBS 网络连接次数排行榜[37m===[34m---[m
                         (@<<<<<<<<<<<<<<@<<<<<<<)
	$date,$time
.
	format LINE =
[1;37m 第@#名: @<<<<<<<<<<<< @##### 次  [34m||   [33m第@#名: @<<<<<<<<<<<< @##### 次[m
	$num-1,$host1,$connecttimes1,$num,$host2,$connecttimes2
.

$num=1;
$~="TITLE";
write;
print("\n");
foreach $host(sort by_connecttimes keys(%connectlist)){
	if($num%2==1){
		$host1=$host;
		$connecttimes1=$connectlist{$host};
	}
	else{
		$host2=$host;
		$connecttimes2=$connectlist{$host};
		$~="LINE";
		write;
	}
	if($num==$maxnum){
		last;
	}
        $num+=1;
}

sub by_connecttimes{
	$connectlist{$b} - $connectlist{$a};
}

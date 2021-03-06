#!/usr/bin/perl -w
# BBS 每天登陆次数排行榜
# write by alt@2002.04.21

use strict

$maxnum=26;

$date=`date +%Y年%m月%d日`;
chomp($date);
`grep $date /home/bbs/bbshome/usies > /home/bbs/bbshome/tmp/usies.today`;
open(DAT,"</home/bbs/bbshome/tmp/usies.today") || die "can not open file";

while($line=<DAT>){
	my @array=split(/ /,$line);
	if(($array[2] eq "ENTER")&&($array[3] ne "guest")){
		$loginlist{$array[3]}+=1;
	}
}
close(DAT);
$time=`date +%H点%M分`;

	format TITLE =
[40m        [0;30;47m┎───────────────────────────┒[m
[40m        [0;30;47m┃   〖 饮水思源 BBS 本日登陆次数排行榜 〗              ┃[m
[40m        [0;30;47m┠──┬──────┬───╥──┬──────┬───┨[m
[40m        [0;30;47m┃名次│  〖 ID 〗  │上站数║名次│  〖 ID 〗  │上站数┃[m
[40m        [0;30;47m┠──┼──────┼───╫──┼──────┼───┨[m
.
	format END =
[40m        [0;30;47m┠──┴──────┴───╨──┴──────┴───┨[m
[40m        [0;30;47m┃                     〖 @<<<<<<<<<<<<< @<<<<<<< 〗    ┃[m
	$date,$time
[40m        [0;30;47m┖───────────────────────────┚[m
.
	format LINE =
[40m        [0;30;47m┃ @# │@<<<<<<<<<<<│ @##  ║ @# │@<<<<<<<<<<<│ @##  ┃[m
	$num-1,$id1,$logintime1,$num,$id2,$logintime2
.
print("\n");
$num=1;
$~="TITLE";
write;
foreach $id(sort by_logintimes keys(%loginlist)){
	if($num%2==1){
		$id1=$id;
		$logintime1=$loginlist{$id};
	}
	else{
		$id2=$id;
		$logintime2=$loginlist{$id};
		$~="LINE";
		write;
	}
	if($num==$maxnum){
		last;
	}
        $num+=1;
}
$~="END";
write;

sub by_logintimes{
	$loginlist{$b} - $loginlist{$a};
}

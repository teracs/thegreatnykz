#!/usr/bin/perl -w
# BBS Ã¿ÌìµÇÂ½´ÎÊýÅÅÐÐ°ñ
# write by alt@2002.04.21

use strict

$maxnum=26;

$date=`date +%YÄê%mÔÂ%dÈÕ`;
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
$time=`date +%Hµã%M·Ö`;

	format TITLE =
[40m        [0;30;47m©²©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¶[m
[40m        [0;30;47m©§   ¡¼ ÒûË®Ë¼Ô´ BBS ±¾ÈÕµÇÂ½´ÎÊýÅÅÐÐ°ñ ¡½              ©§[m
[40m        [0;30;47m©Ä©¤©¤©Ð©¤©¤©¤©¤©¤©¤©Ð©¤©¤©¤¨i©¤©¤©Ð©¤©¤©¤©¤©¤©¤©Ð©¤©¤©¤©Ì[m
[40m        [0;30;47m©§Ãû´Î©¦  ¡¼ ID ¡½  ©¦ÉÏÕ¾Êý¨UÃû´Î©¦  ¡¼ ID ¡½  ©¦ÉÏÕ¾Êý©§[m
[40m        [0;30;47m©Ä©¤©¤©à©¤©¤©¤©¤©¤©¤©à©¤©¤©¤¨o©¤©¤©à©¤©¤©¤©¤©¤©¤©à©¤©¤©¤©Ì[m
.
	format END =
[40m        [0;30;47m©Ä©¤©¤©Ø©¤©¤©¤©¤©¤©¤©Ø©¤©¤©¤¨l©¤©¤©Ø©¤©¤©¤©¤©¤©¤©Ø©¤©¤©¤©Ì[m
[40m        [0;30;47m©§                     ¡¼ @<<<<<<<<<<<<< @<<<<<<< ¡½    ©§[m
	$date,$time
[40m        [0;30;47m©º©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¾[m
.
	format LINE =
[40m        [0;30;47m©§ @# ©¦@<<<<<<<<<<<©¦ @##  ¨U @# ©¦@<<<<<<<<<<<©¦ @##  ©§[m
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

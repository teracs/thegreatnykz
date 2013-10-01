#!/bin/sh
# write by alt@sjtu
#########################################################################
#	Usage: bbsbak.sh {boards|0Announce|home|mail|other|log}				#
#备份目录在 $BACKUPHOME/`date +%Y-%m-%d` , 程序保留最近 2 次的备份      #
#可由 crontab 执行                                                      #
#  0 1 * * 1 /home/bbs/local/bin/bbsbak.sh boards> /dev/null 2>&1       #
#                                                                       #
#精华区,版面都是按一个版一个包;home,mail 一个字母一个包                 #
#其余的东西全部打包进bbs.cpio.gz.                                       #
#                                                                       #
# 对 cpio 格式恢复数据运行:                                             #
#$  gzip -d foo.cpio.gz; cpio -ivdum < foo.cpio                         #
#########################################################################

BACKUPHOME=/home/bbs/bbsbak
BBSHOME=/home/bbs/bbshome

bakboards(){
	BACKUPHOME=$BACKUPHOME/boards
	[ ! -d $BACKUPHOME ] && mkdir $BACKUPHOME
	cd $BACKUPHOME
	[ -f .old ] && LAST_BACKUPDIR=$BACKUPHOME/`date -r .old +%Y-%m-%d`

	BACKUPDIR=$BACKUPHOME/`date +%Y-%m-%d`
	[ -d $BACKUPDIR ] && exit
	touch .tmp
	mkdir $BACKUPDIR

	cd $BBSHOME/boards
	for i in *; do
        echo -n "Backup boards/$i... "
		tar -zcf $BACKUPDIR/$i.tar.gz $i
	done

	cd $BACKUPHOME
	[ -f .old ] && rm -rf $LAST_BACKUPDIR
	[ -f .new ] && mv .new .old
	mv .tmp .new
	}

bak0Announce(){
	BACKUPHOME=$BACKUPHOME/0Announce
	[ ! -d $BACKUPHOME ] && mkdir $BACKUPHOME
	cd $BACKUPHOME
	[ -f .old ] && LAST_BACKUPDIR=$BACKUPHOME/`date -r .old +%Y-%m-%d`

	BACKUPDIR=$BACKUPHOME/`date +%Y-%m-%d`
	[ -d $BACKUPDIR ] && exit
	touch .tmp
	mkdir $BACKUPDIR

	cd $BBSHOME/0Announce/groups
	for i in GROUP*; do
		[ ! -d $i ] && continue
		cd $i
		mkdir $BACKUPDIR/$i
		for j in *; do
 	       	echo -n "Backup 0Announce/groups/$i/$j... "
			tar -zcf $BACKUPDIR/$i/$j.tar.gz $j
		done
		cd $BBSHOME/0Announce/groups
	done

	cd $BACKUPHOME
	[ -f .old ] && rm -rf $LAST_BACKUPDIR
	[ -f .new ] && mv .new .old
	mv .tmp .new
	}

bakhome(){
	BACKUPHOME=$BACKUPHOME/home
	[ ! -d $BACKUPHOME ] && mkdir $BACKUPHOME
	cd $BACKUPHOME
	[ -f .old ] && LAST_BACKUPDIR=$BACKUPHOME/`date -r .old +%Y-%m-%d`

	BACKUPDIR=$BACKUPHOME/`date +%Y-%m-%d`
	[ -d $BACKUPDIR ] && exit
	touch .tmp
	mkdir $BACKUPDIR

	cd $BBSHOME/home
	for i in *;do
		echo -n "Backup home/$i... "
		tar -zcf $BACKUPDIR/$i.tar.gz $i
	done

	cd $BACKUPHOME
	[ -f .old ] && rm -rf $LAST_BACKUPDIR
	[ -f .new ] && mv .new .old
	mv .tmp .new
	}

bakmail(){
	BACKUPHOME=$BACKUPHOME/mail
	[ ! -d $BACKUPHOME ] && mkdir $BACKUPHOME
	cd $BACKUPHOME
	[ -f .old ] && LAST_BACKUPDIR=$BACKUPHOME/`date -r .old +%Y-%m-%d`

	BACKUPDIR=$BACKUPHOME/`date +%Y-%m-%d`
	[ -d $BACKUPDIR ] && exit
	touch .tmp
	mkdir $BACKUPDIR

	cd $BBSHOME/mail
	for i in *;do
		echo -n "Backup mail/$i... "
		tar -zcf $BACKUPDIR/$i.tar.gz $i
	done

	cd $BACKUPHOME
	[ -f .old ] && rm -rf $LAST_BACKUPDIR
	[ -f .new ] && mv .new .old
	mv .tmp .new
	}

bakother(){
	BACKUPHOME=$BACKUPHOME/other
	[ ! -d $BACKUPHOME ] && mkdir $BACKUPHOME
	cd $BACKUPHOME
	[ -f .old ] && LAST_BACKUPDIR=$BACKUPHOME/`date -r .old +%Y-%m-%d`

	BACKUPDIR=$BACKUPHOME/`date +%Y-%m-%d`
	[ -d $BACKUPDIR ] && exit
	touch .tmp
	mkdir $BACKUPDIR

	cd $BBSHOME
	echo -n "Backup bbs... "
	find . -path './0Announce/groups/GROUP*' -prune \
		-o -path './boards' -prune \
		-o -path './mail' -prune \
		-o -path './home' -prune \
		-o -path './tmp' -prune \
		-o -print \
		| cpio -ocB | gzip - > $BACKUPDIR/bbs.cpio.gz

	cd $BACKUPHOME
	[ -f .old ] && rm -rf $LAST_BACKUPDIR
	[ -f .new ] && mv .new .old
	mv .tmp .new
	}

baklog(){
	BACKUPHOME=$BACKUPHOME/log
	[ ! -d $BACKUPHOME ] && mkdir $BACKUPHOME

	BACKUPDIR=$BACKUPHOME/`date +%Y-%m-%d`
	[ -d $BACKUPDIR ] && exit
	mkdir $BACKUPDIR

	cd $BBSHOME
	mv usies trace $BACKUPDIR
	cd $BBSHOME/reclog
	mv bbsnet.log pop3d.log mail-log bbsd.log $BACKUPDIR
	mv $BBSHOME/innd/bbslog $BACKUPDIR
	cd /usr/local/apache/logs
	mv access_log error_log $BACKUPDIR
	}

case "$1" in
	boards)
	bakboards
	;;
	0Announce)
	bak0Announce
	;;
	home)
	bakhome
	;;
	mail)
	bakmail
	;;
	other)
	bakother
	;;
	log)
	baklog
	;;
	*)
	echo $"Usage: $0 {boards|0Announce|home|mail|other|log}"
	exit 1
esac

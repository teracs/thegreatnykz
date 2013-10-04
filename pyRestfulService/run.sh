#!/bin/sh

export "PATH=$PATH:`pwd`:`pwd`/servers:`pwd`/apis:`pwd`/parsers:`pwd`/tests"
gcc -o parsers/checkPasswd parsers/checkPasswd.c
arch -i386 python servers/entrance.py

#!/bin/bash

acc_dir='../../acc156win'
addon_dir='/cygdrive/d/Games/DefaultLibrary/Doom/addons'
name=midi-metal-madness
$acc_dir/acc.exe -hh acs/randmus.c acs/randmus.o

if [ "$?" -eq 0 ]; then
	zip -r $name.pk3 . --exclude '.git*'
	rm $addon_dir/$name.pk3
	mv $name.pk3 $addon_dir
fi


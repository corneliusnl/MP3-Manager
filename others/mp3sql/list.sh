#!/bin/sh 

mount /dev/cdrom /cdrom
mp3lister /cdrom $1 >> mp3database
umount /dev/cdrom
eject /dev/cdrom

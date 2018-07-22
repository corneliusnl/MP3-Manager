#!/bin/sh

#
# This scripts reads the playlist and plays the MP3s if the playlist dosen´t exist randomize from
# the big list...
#

#
# This script is made by Daniel Andersson <daniel@sto.sema.se>
#


MPG123=`which mpg123`
FILE="/tmp/playlist"
ALLFILE="/tmp/allmp3files"
OPTIONS=""

#
# Progam start - do not edit below! :)
#

ALLLENGT=`cat $ALLFILE | wc -l `


while [ 0 ]; do
   if [ -s $FILE ]; then 
    {
    MP3=`head -1 $FILE`
    $MPG123 $OPTIONS "$MP3"
    tail +2 $FILE >> $FILE.$$
    mv -f $FILE.$$ $FILE
    chown nobody:nogroup $FILE
    }
 else
   {
   LINE=0
   cat $ALLFILE | while read MP3
     do
     if [ ! -s $FILE ]; then
      {
      random -e $ALLLENGT
      RND=$?
      MP3=`tail +$RND $ALLFILE | head -1`
      $MPG123 "$MP3"
      }
     fi
   done 
  } 
 fi

done

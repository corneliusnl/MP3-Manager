#!/bin/sh 

# 
#     This script is made by Daniel Andersson <daniel@sto.sema.se>
#
#  You may modify and reuse it if you mension my name in the new script
#

# 
# This script will create the needed database on an local MySQL server 
# if you are running an other SQL server it is the folowing lines you have 
# to paste into your SQL parser 
#
# create database mp3;
# create table create table mp3data (id int not null auto_increment, tag \
# char(3), media char(15), filename char(100), artist char(100), album \
# char(100), title char(100), year char(4), comment char (100), genre \
# char(25), filesize int(30), primary key (id)) ;

#
# Create some variables 
#

# password = "password"
mysql=`which mysql`

#
# Create an temporary file for using
#

echo "create database mp3;" > /tmp/create_database.$$
echo "connect mp3;" >> /tmp/create_database.$$
echo "create table mp3data (id int not null auto_increment, tag char(3), media char(15), filename char(100), artist char(100), album char(100), title char(100), year char(4), comment char (100), genre char(25), filesize int(30), primary key (id)) ; " >> /tmp/create_database.$$

#
# Run MySQL to..
#

$mysql -p < /tmp/create_database.$$

#
# Remove temporary files
#

rm -f /tmp/create_database.$$ > /dev/null 2> /dev/null

#
# DONE! :)
#


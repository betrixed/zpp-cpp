#!/bin/bash
. /etc/os-release
if [ $ID == "debian" ]; then
  pvid=`php -r "echo(PHP_MAJOR_VERSION . '.' . PHP_MINOR_VERSION);"`
elif [[ $ID == "arch" || $ID == "endeavouros" ]]; then
#  pvid=""
   pvid=`php -r "echo(PHP_MAJOR_VERSION . PHP_MINOR_VERSION);"`
elif [[ $ID == "archarm" ]]; then
   pvid=""
else
  pvid="Unknown case for $ID"	
fi
echo $pvid
# sudo systemctl restart php${pvid}-fpm

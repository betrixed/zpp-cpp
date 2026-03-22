#!/bin/bash
. /etc/os-release
pconf=`php-config --ini-dir`
pvid=`php -r "echo(PHP_MAJOR_VERSION . '.' . PHP_MINOR_VERSION);"`
echo $pconf
echo $pvid
# sudo systemctl restart php${pvid}-fpm

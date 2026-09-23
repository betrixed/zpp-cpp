#!/bin/bash
# Stop php-fpm services using modules to be replaced
# Disable PHP extension modules that may be not working silently. 
#
. ./pvid.sh
echo "On $ID system"
if [ $ID = "debian" ]; then
IFILE="/etc/php/$pvid/mods-available/wcc.ini"
OFILE="/etc/php/$pvid/mods-available/wcc.off"
else
IFILE="/etc/php/conf.d/wcc.ini"
OFILE="/etc/php/conf.d/wcc.off"
fi
if [ -f "$OFILE" ]; then
	echo "Turn on extensions"
	sudo mv "$OFILE" "$IFILE"
else
	echo "Have $IFILE"
fi
sudo systemctl restart $FPM


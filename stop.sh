#!/bin/bash
# Stop php-fpm services using modules to be replaced
# Disable PHP extension modules that may be not working silently. 
source /etc/os-release
source pvid.sh
echo "os-release: $NAME"
if [[ "$ID" == "debian" ]]; then
  IFILE="/etc/php/$pvid/mods-available/wcc.ini"
  OFILE="/etc/php/$pvid/mods-available/wcc.off"
else
  IFILE="/etc/php/conf.d/wcc.ini"
  OFILE="/etc/php/conf.d/wcc.off"
fi
if [ -f "$IFILE" ]; then
	echo "Turn off extensions $IFILE"
	sudo mv "$IFILE" "$OFILE"
else
	echo "Have $OFILE"
fi
. ./pvid.sh
echo "stop $FPM"
sudo systemctl stop $FPM 


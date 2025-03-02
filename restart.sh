#!/bin/bash
. pvid.sh
echo php${pvid}-fpm 
sudo systemctl restart php${pvid}-fpm

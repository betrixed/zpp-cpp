#!/bin/sh
set -e
sudo make clean
clear

make ./stub/wcc_arginfo.h
make ./stub/xmlread_arginfo.h

#make sqlipart_arginfo.h
#make icache_arginfo.h
#make toml_arginfo.h
#make htmlgem_arginfo.h
#make wcc_config_arginfo.h
#make request_globals_arginfo.h
#make global_response_arginfo.h
#make filter_wcf_arginfo.h
#make ustr8_arginfo.h

sudo make install

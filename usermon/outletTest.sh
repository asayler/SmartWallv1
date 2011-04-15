#!/bin/bash
# Andy Sayler
# Send continour messages to outlet

while [ 1 ]; do
    ./swChnMsg 0x0012 QUERY OUTLET 0x0010 4 0 x 1 x
    ./swChnMsg 0x0012 QUERY OUTLET 0x0022 8 0 x 1 x
    sleep .25
    ./swChnMsg 0x0012 SET OUTLET 0x0010 4 0 1 1 1
    sleep .25
    ./swChnMsg 0x0012 QUERY OUTLET 0x0010 4 0 x 1 x
    ./swChnMsg 0x0012 QUERY OUTLET 0x0022 8 0 x 1 x
    sleep .25
    ./swChnMsg 0x0012 SET OUTLET 0x0010 4 0 0 1 0
    sleep .25
    ./swChnMsg 0x0012 QUERY OUTLET 0x0010 4 0 x
    ./swChnMsg 0x0012 QUERY OUTLET 0x0022 8 0 x
    sleep .25
    ./swChnMsg 0x0012 SET OUTLET 0x0010 4 0 1
    sleep .25
    ./swChnMsg 0x0012 QUERY OUTLET 0x0010 4 1 x
    ./swChnMsg 0x0012 QUERY OUTLET 0x0022 8 1 x
    sleep .25
    ./swChnMsg 0x0012 SET OUTLET 0x0010 4 1 1
    sleep .25
    ./swChnMsg 0x0012 QUERY OUTLET 0x0010 4 0 x
    ./swChnMsg 0x0012 QUERY OUTLET 0x0022 8 0 x
    sleep .25
    ./swChnMsg 0x0012 SET OUTLET 0x0010 4 0 0
    sleep .25
    ./swChnMsg 0x0012 QUERY OUTLET 0x0010 4 1 x
    ./swChnMsg 0x0012 QUERY OUTLET 0x0022 8 1 x
    sleep .25
    ./swChnMsg 0x0012 SET OUTLET 0x0010 4 1 0
    sleep .25
done

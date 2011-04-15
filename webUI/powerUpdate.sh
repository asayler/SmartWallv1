#!/bin/bash
# Andy Sayler
# Do power update on outlet outlet

while [ 1 ]; do
    ./cron.pl
    ./make_graphs.pl
    sleep 5
done

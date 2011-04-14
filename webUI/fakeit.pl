#!/usr/bin/perl

use strict;
use warnings;
use Data::Dumper; #cleanly print array, hash

chdir("/home/laura/senior/code/SmartWallv1/usermon") or die "$!";

chomp(my $month = `date "+%m"`); #01-12
chomp(my $year = `date "+%Y"`); #4 digits (ie 1998)
my $file_name = '../webUI/historic/fake'.'_'.$month.'_'.$year;  
open(FILE_HANDLE,'>',"$file_name") or die "-E- Unable to open $file_name : $!\n";

my@days=('01', '02', '03', '04', '05', '06', '07', '08', '09', 10..30);
my @hours=('01', '02', '03', '04', '05', '06', '07', '08', '09', 10..23);
my @minutes=('01', '02', '03', '04', '05', '06', '07', '08', '09', 10..59);
foreach my $day (@days){
    foreach my $hour (@hours){
	my $power0= int(rand(10))*10;
	my $power1= int(rand(10))*10;
	foreach my $minute (@minutes){
	    my $date_string = $day.".".$hour.".".$minute;
	    print FILE_HANDLE "$date_string $power0 $power1\n";
	}
    }
}
close(FILE_HANDLE);

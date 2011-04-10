#!/usr/bin/perl

use strict;
use warnings;
use Data::Dumper; #cleanly print array, hash
use GD::Graph::lines; #enable line-graph making
#require 'save.pl';

#sw opcodes
my $state = "0x0010";
my $voltage = "0x0020";
my $current = "0x0021";
my $power = "0x0022";
my $freq = "0x0023";
my $phase = "0x0024";
#sw types
my $outlet = "0x8000000000000004";
my $master = "0x8000000000000001";
my $universal = "0x8000000000000000";

chdir("/home/laura/senior/code/SmartWallv1/usermon") or die "$!";

#query for all devices and info
my $outlets_raw = `./swls -raw`; #backtick system call
chomp($outlets_raw);
my @ls_lines = split(/\n/,$outlets_raw); #split on new lines

#fill hash of hashes, %lookup, with device info
my %lookup;
foreach my $row (@ls_lines){
    my @items = split(/ /,$row);
    $items[7] =~ s/ //;
    $lookup{$items[7]}{'swAdr'}=$items[1];
    $lookup{$items[7]}{'ipAdr'}=$items[2];
    $lookup{$items[7]}{'type'}=$items[3];
    $lookup{$items[7]}{'channels'}=$items[4];
    $lookup{$items[7]}{'grpId'}=$items[5];
    $lookup{$items[7]}{'ver'}=$items[6];
}

foreach my $key(sort keys %lookup){
    if($lookup{$key}{'type'} eq $outlet) {
	my $channels = $lookup{$key}{'channels'};
	my $file_name =""; 
	open(FILE_HANDLE, "$file_name") or die "-E- Unable to open $file_name : $!\n";
	my @date_time; #array day.hour.minute
	my @power0; #array of powers for channel 0
	my @power1; #array of powers for channel 1
	if ($channels eq "0x01") {
	}elsif($channels eq "0x02") {
	    while (<FILE_HANDLE>) {
		next if $_ =~ m/^#/;  #skip any line starting with a hash
		chomp; #remove trailing newline
		my $split_string = ' ';  #split on spaces
		my @line_array = split(/$split_string/, $_);  
		push @date_time, $line_array[0];
		push @power0, $line_array[1];
		push @power1, $line_array[2];
	    }
	    
	    my @data = (@date_time,@power0,@power1);
	}else{
	    print "-E-: Invalid number of channels\n";
	    #error or unhandled number of channels
	}	    
	my $my_graph = new GD::Graph::lines( );
	
	$my_graph->set(
	    x_label => 'Time',
	    y_label => 'Power Usage (Watts)',
	    title => 'Monthly Power Usage',
	    y_max_value => 80,
	    y_tick_number => 6,
	    y_label_skip => 2,
	    markers => [ 1, 5 ],
	    
	    transparent => 0,
	    );
	
	$my_graph->set_legend( 'data set 1', 'data set 2' );
	$my_graph->plot(\@data);
	save_chart($my_graph, 'sample42');
	
    }
}

#fill array, @UIDs, with list of UIDs
#my %UIDs;
#foreach my $row (@ls_lines){
#    my @items = split(/ /,$row);
#    $UIDs{$items[7]
#}
#print Dumper(@UIDs); #debug

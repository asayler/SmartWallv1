#!/usr/bin/perl

use strict;
use warnings;
use Data::Dumper; #cleanly print array, hash
use GD::Graph::lines; #enable line-graph making

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

chdir("../webUI/historic") or die "$!";

foreach my $key(sort keys %lookup){
    if($lookup{$key}{'type'} eq $outlet) {
	chomp(my $month = `date "+%m"`); #01-12
	chomp(my $year = `date "+%Y"`);
	my $file_name = $key."_".$month."_".$year; 
	open(FILE_HANDLE, "$file_name") or die "-E- Unable to open $file_name : $!\n";
	my @date_time; #array day.hour.minute
	my @power0; #array of powers for channel 0
	my @power1; #array of powers for channel 1
	my @data; #to be graphed
	my $channels = $lookup{$key}{'channels'};
	if ($channels eq "0x01") {
	    next; #master outlet, no power query necessary
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
	    @data = ([@date_time],[@power0],[@power1]);
	}else{ 	#error or unhandled number of channels
	    print "-E-: Invalid number of channels\n";
	    next;
	}	
	#make the graph from @data
	my $my_graph = GD::Graph::lines->new(400,300);
	$my_graph->set(
	    x_label => 'Date',
	    y_label => 'Power Usage (Watts)',
	    title => 'Monthly Power Usage',
	    y_max_value => 5,
	    y_tick_number => 5,
	    x_tick_number => 10,
	    y_label_skip => 2,
	    markers => [ 1, 5 ],
	    transparent => 0,
	    );
	$my_graph->set_legend( 'Channel 0', 'Channel 1' );
	my $gd = $my_graph->plot(\@data) or die $my_graph->error;
        my $string = $key.".png";

	#save graph in webUI/historic as .png
	open(IMG, ">$string") or die $!;
	binmode IMG;
	print IMG $gd->png;
	close IMG;
    }
}

#create total graph
chomp(my $month = `date "+%m"`); #01-12
chomp(my $year = `date "+%Y"`);
my $file_name = "total"."_".$month."_".$year; 
open(FILE_HANDLE, "$file_name") or die "-E- Unable to open $file_name : $!\n";
my @date_time; #array day.hour.minute
my @power; #array of powers for channel 0
my @data; #to be graphed
while (<FILE_HANDLE>) {
    next if $_ =~ m/^#/;  #skip any line starting with a hash
    chomp; #remove trailing newline
    my $split_string = ' ';  #split on spaces
    my @line_array = split(/$split_string/, $_);  
    push @date_time, $line_array[0];
    push @power, $line_array[1];
}
@data = ([@date_time],[@power]);

#make the graph from @data
my $my_graph = GD::Graph::lines->new(400,300);
$my_graph->set(
	    x_label => 'Date',
	    y_label => 'Power Usage (Watts)',
	    title => 'Power Usage (all SmartWall devices)',
	    y_max_value => 10,
	    y_tick_number => 5,
	    #x_tick_number => 6,
	    y_label_skip => 2,
	    markers => [ 1, 5 ],
	    transparent => 0,
	    );
$my_graph->set_legend( 'Channel 0', 'Channel 1' );
my $gd = $my_graph->plot(\@data) or die $my_graph->error;
my $string = "total.png";

#save graph in webUI/historic as .png
open(IMG, ">$string") or die $!;
binmode IMG;
print IMG $gd->png;
close IMG;

#copy .png files to /var/www
my $success = `cp *.png /var/www`;
#check $success for successful copy


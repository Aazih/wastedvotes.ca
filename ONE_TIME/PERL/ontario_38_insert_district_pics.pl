#!/usr/bin/perl

use DBI;
use strict;
use utf8;
use lib "/home/aamir/ElectionsWorking/ElectionsDB/trunk/ONE_TIME/PERL";

require "login.pl";
my $file_line;

my @file_array;

my $dbh;

if ($ARGV[0] eq "DEV") {
	$dbh = LOGIN_DEV();
}
else{
	$dbh = LOGIN_TEST();
}

my $sth;
my @data;

my $district_english;
my $district_unique;
my $file_name;

my $parliament_type;
my $parliament_id;

my $directory_name = "ontario_38_district/";

$parliament_type = "Ontario";
$sth = $dbh->prepare('SELECT PARLIAMENT_TYPE_ID FROM parliament_type where PARLIAMENT_TYPE=?');
$sth->execute($parliament_type)
        or die "Couldn't execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_id = $data[0];
$sth->finish();

open(DISTRICT_FILE, "../../Data/ontario_38_district_pics.csv");
while ($file_line = <DISTRICT_FILE>)
{
	@file_array = split(',', $file_line);
	$district_english = $file_array[0];
	$file_name = $file_array[1];
	chomp($district_english);
	chomp($file_name);

	$district_english = substr($district_english,1,-1);
	$file_name = $directory_name.$file_name.".gif";
	$sth = $dbh->prepare('SELECT DISTRICT_UNIQUE_NAME FROM districts where DISTRICT_ENGLISH_NAME=? and PARLIAMENT_ID=?');
	$sth->execute($district_english, $parliament_id)
		or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$district_unique = $data[0];
	$sth->finish();

	print "Working with district_english $district_english, district_unique $district_unique for file_name $file_name\n";
	$sth = $dbh->prepare('INSERT INTO pic_resource(ENTITY, ENTITY_ID, PIC_TYPE, PIC_NAME) VALUES (?,?,?,?)');
	$sth = $sth->execute('DISTRICT',$district_unique, 'MAP',$file_name)
		or die "Couldn't execute statement: ". $dbh->errstr;
	print "Done\n";
}

$dbh->disconnect();

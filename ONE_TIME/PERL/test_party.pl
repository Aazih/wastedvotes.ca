#!/usr/bin/perl

###
#Setting Up PERL Settings
###

use DBI;
use strict;
use utf8;
use FindBin;

###
#Including required files
###

use lib "$FindBin::Bin/";
require "login.pl";
require "functions.pl";

###
#Logging into Database
###

my $dbh;

if ($ARGV[0] eq "DEV") {
	$dbh = LOGIN_DEV();
}

###
#Declaring variables
###

my $sth;
my @data;
my $numArgs;

my $file_name;

my $file_line;
my @currentRow;

my $party;
my $party_id;


my $parliament_id;
my $parliament_type;
my $parliament_type_id;
my $province_id;
my $parliament_number;
my $parliament_abbr;

my $count;

###
#Parsing Command Line
###

$numArgs = $#ARGV + 1;

if ($numArgs != 2) {
  print "Database and then Filename of file to be loaded should be the only arguments provided to this script currently providing $numArgs arguments\n";
  exit;
}

$file_name = $ARGV[1];

$file_name = "../../Data/".$file_name;

###
#Parsing election information from file
###

open(INFO_FILE, $file_name);
$parliament_type = <INFO_FILE>;
$parliament_number = <INFO_FILE>;

$parliament_type = trim($parliament_type);
$parliament_number = trim($parliament_number);

###
#Querying information about election from db
###

$sth = $dbh->prepare('SELECT PARLIAMENT_TYPE_ID,PARLIAMENT_ABBR FROM parliament_type where PARLIAMENT_TYPE=?');
$sth->execute($parliament_type)
	or die "Couldn't execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_type_id = $data[0];
$parliament_abbr = $data[1];
$sth->finish();

$parliament_abbr = trim($parliament_abbr);

$sth = $dbh->prepare('SELECT PARLIAMENT_ID FROM parliament where PARLIAMENT_TYPE_ID=? and PARLIAMENT_NUMBER=?');
$sth->execute($parliament_type_id,$parliament_number)
	or die "Could not execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_id = $data[0];
$sth->finish();

print "Starting Line\nparliament_type is $parliament_type and parliament_number is $parliament_number with parliament_abbr $parliament_abbr\n";

###
#Parsing file and inserting districts and contest information
###

$count = 1;

while ($file_line = <INFO_FILE>)
{
	$count++;
#	print "Parsing row $count\n";

	chomp($file_line);
	@currentRow = split(/,/,$file_line);

	$party = $currentRow[5];

	$party = trim($party);

	#Getting PARTY ID
	$sth = $dbh->prepare('SELECT p.PARTY_ID FROM party p where p.PARTY_ENGLISH_NAME=? and PARLIAMENT_TYPE_ID=?');
	$sth->execute($party, $parliament_type_id)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$party_id = $data[0];

	$party_id = trim($party_id);

	if ($party_id =~ /^(\d+\.?\d*|\.\d+)$/) {
		print "FOUND OK $party with id $party_id\n";
	} else {
               	print "ALERT $party not found in database for parliament type $parliament_type_id\n";
	}

}


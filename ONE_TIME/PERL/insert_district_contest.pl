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
elsif ($ARGV[0] eq "TEST"){
	$dbh = LOGIN_TEST();
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

my $province;
my $new_province;
my $district;
my $new_district;
my $district_french;
my $new_district_french;
my $district_unique_name;
my $votes;
my $percent;
my $party;

my $party_id;
my $candidate;
my $candidate_french;
my $won;

my $district_count;
my $district_id;

my $parliament_id;
my $parliament_type;
my $parliament_type_id;
my $province_id;
my $parliament_number;
my $parliament_abbr;

my $count;

###
#Setting up common insert sql statements
###

sub DISTRICT_INSERT {
	$sth = $dbh->prepare('INSERT INTO districts(PARLIAMENT_ID,PROVINCE_ID,DISTRICT_UNIQUE_NAME,DISTRICT_FRENCH_NAME,DISTRICT_ENGLISH_NAME) VALUES (?,?,?,?,?)');
	$sth->execute($parliament_id,$province_id,$district_unique_name,$district,$district) or die "Couldn't execute DISTRICT_INSERT: ".$dbh->errstr;
}

sub CONTEST_INSERT {
	$sth = $dbh->prepare('INSERT INTO contest (DISTRICT_ID, CANDIDATE_NAME, CANDIDATE_PARTY, PROVINCE_ID,VOTES,CONTEST_NUMBER,WON) VALUES (?,?,?,?,?,1,?)');
	$sth->execute($district_id,$candidate,$party_id,$province_id, $votes, $won) or die "Couldn't execute CONTEST_INSERT: ".$dbh->errstr;
}

sub CALC_WINNER {
	$dbh->do('create table contest_result (DISTRICT_ID INT, CANDIDATE_NAME VARCHAR(100))');
	$sth = $dbh->prepare('INSERT INTO contest_result (DISTRICT_ID, CANDIDATE_NAME) SELECT DISTRICT_ID, CANDIDATE_NAME FROM contest o WHERE DISTRICT_ID IN (SELECT DISTRICT_ID from districts where PARLIAMENT_ID = ? AND o.VOTES = (SELECT MAX(votes) FROM contest WHERE DISTRICT_ID = o.DISTRICT_ID))');
	$sth->execute($parliament_id);

	$dbh->do('update contest c SET WON = 1 WHERE (c.DISTRICT_ID, c.CANDIDATE_NAME) IN  (select DISTRICT_ID, CANDIDATE_NAME from contest_result)');

	$dbh->do('drop table contest_result');
}

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

	$new_province = $currentRow[0];
	$new_district = $currentRow[1];
	$new_district_french = $currentRow[2];
	$candidate = $currentRow[3];
	$candidate_french = $currentRow[4];
	$party = $currentRow[5];
	$votes = $currentRow[6];
	$won = 0;

        $new_province = trim($new_province);
	$new_district = trim($new_district);
	$candidate = trim($candidate);
	$party = trim($party);

        if ($new_province ne "") { $province = $new_province; }

        if ($new_district ne "") { $district = $new_district; }

        #GETTING THE PROVINCE ID
        $sth = $dbh->prepare('SELECT PROVINCE_ID FROM province where PROVINCE_ENGLISH_NAME=?');
        $sth->execute($province)
	    or die "Couldn't execute statement: ". $dbh->errstr;
        @data = $sth->fetchrow_array();
        $province_id = $data[0];
        $sth->finish();

	#GETTING THE DISTRICT ID
	$sth = $dbh->prepare('SELECT count(*) FROM districts where DISTRICT_ENGLISH_NAME=? and PARLIAMENT_ID=?');
	$sth->execute($district, $parliament_id)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$district_count = $data[0];

	if ($district_count == 0) {
		$district_unique_name = $parliament_abbr."-".$parliament_number."-".$district;
		print "Created unique_name:\t$district_unique_name\n";
		DISTRICT_INSERT;
		print "District inserted\n";
	}
	elsif ($district_count > 1) {
		print "ERROR: District count is more than one and is $district_count\n";
		exit;
	}
	else {
		print "District already exists\n";
	}

	$sth = $dbh->prepare('SELECT DISTRICT_ID FROM districts where DISTRICT_ENGLISH_NAME=? and PARLIAMENT_ID=?');
	$sth->execute($district, $parliament_id)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$district_id = $data[0];

	#Getting PARTY ID
	$sth = $dbh->prepare('SELECT p.PARTY_ID FROM party p where p.PARTY_ENGLISH_NAME=? and PARLIAMENT_TYPE_ID=?');
	$sth->execute($party, $parliament_type_id)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$party_id = $data[0];

	$party_id = trim($party_id);

	print "Party $party in parliament type $parliament_type_id is of party_id $party_id";
	if ($party_id =~ /^(\d+\.?\d*|\.\d+)$/) {
		print "\n";
	} else {
                print " ALERT\n";
	}

	print "District:$district unique: $district_unique_name Province: $province $province_id Votes:$votes Party: $party $party_id Candidate: $candidate won:$won Parliament_id:$parliament_id\n";

	CONTEST_INSERT;

}

###
#Calculating Winner
###
	CALC_WINNER

print "count is $count, exiting";

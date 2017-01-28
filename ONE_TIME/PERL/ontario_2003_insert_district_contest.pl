#!/usr/bin/perl

use DBI;
use strict;
use utf8;
use lib "/home/aamir/ElectionsWorking/ElectionsDB/trunk/ONE_TIME/PERL";

require "login.pl";

my $dbh;

if ($ARGV[0] eq "DEV") {
	$dbh = LOGIN_DEV();
}
else{
	$dbh = LOGIN_TEST();
}

my $sth;
my @data;

my $file_line;
my @currentRow;

my $district;
my $district_unique_name;
my $plurality;
my $votes;
my $percent;
my $party;
my $party_id;
my $candidate;
my $won;

my $starloc;
my $district_count;
my $district_id;

my $candidate_residence='N/A';
my $candidate_occupation_english='N/A';
my $candidate_occupation_french='N/A';

my $parliament_id;
my $parliament_type_id;
my $province_id;
my $parliament_number;

my $parent_region_id;
my $region;
my $region_id;
my $sub_region;
my $sub_region_id;
my $region_unique_name;
my $school_board;
my $tv;
my $newspaper;
my $region_count;
my $top_region;

my $workingyear;
my $currentyear;

my $count;
my $rdcount;

sub DISTRICT_INSERT {
	$sth = $dbh->prepare('INSERT INTO districts(PARLIAMENT_ID,PROVINCE_ID,DISTRICT_UNIQUE_NAME,DISTRICT_FRENCH_NAME,DISTRICT_ENGLISH_NAME) VALUES (?,?,?,?,?)');
	$sth->execute($parliament_id,$province_id,$district_unique_name,$district,$district) or die "Couldn't execute DISTRICT_INSERT: ".$dbh->errstr;
}

sub CONTEST_INSERT {
	$sth = $dbh->prepare('INSERT INTO contest (DISTRICT_ID, CANDIDATE_NAME, CANDIDATE_PARTY, PROVINCE_ID,VOTES,CONTEST_NUMBER,WON) VALUES (?,?,?,?,?,1,?)');
	$sth->execute($district_id,$candidate,$party_id,$province_id, $votes, $won) or die "Couldn't execute CONTEST_INSERT: ".$dbh->errstr;
}


$sth = $dbh->prepare('SELECT PROVINCE_ID FROM province where PROVINCE_ENGLISH_NAME=?');
$sth->execute('ONTARIO')
	or die "Couldn't execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$province_id = $data[0];
$sth->finish();

$sth = $dbh->prepare('SELECT PARLIAMENT_TYPE_ID FROM parliament_type where PARLIAMENT_TYPE=?');
$sth->execute('Ontario')
	or die "Couldn't execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_type_id = $data[0];
$sth->finish();

open(INFO_FILE, "../../Data/Ontario_81-03_district_region_contest.csv");

print "StartingLine\nProvince_id is $province_id\tparliament_type_id is $parliament_type_id\n";

$workingyear='N/A';
$count = 1;

$file_line = <INFO_FILE>;
while ($file_line = <INFO_FILE>)
{
	$count++;
	print "Parsing row $count\n";

	chomp($file_line);
	@currentRow = split(/\|/,$file_line);

	$currentyear = $currentRow[0];
	$district = $currentRow[1];
	$region = $currentRow[4];
	$sub_region = $currentRow[5];
	$school_board = $currentRow[6];
	$tv = $currentRow[7];
	$newspaper = $currentRow[8];
	$candidate = $currentRow[12];
	$party = $currentRow[13];
	$votes = $currentRow[15];
	$won = $currentRow[16];

	if ($won > 0){
		$won = 1;
	}

	if ($currentyear != '2003') { print "count is $count, currentyear is $currentyear, exiting"; exit; }

	if ($currentyear != $workingyear) {
		$sth = $dbh->prepare('SELECT PARLIAMENT_ID, PARLIAMENT_NUMBER FROM parliament where PARLIAMENT_TYPE_ID=? and YEAR(ELECTION_DATE)=?');
		$sth->execute($parliament_type_id,$currentyear)
			or die "Could not execute statement: ". $dbh->errstr;
		@data = $sth->fetchrow_array();
		$parliament_id = $data[0];
		$parliament_number = $data[1];
		$sth->finish();
	}


	#GETTING THE DISTRICT ID
	$sth = $dbh->prepare('SELECT count(*) FROM districts where DISTRICT_ENGLISH_NAME=? and PARLIAMENT_ID=?');
	$sth->execute($district, $parliament_id)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$district_count = $data[0];

	if ($district_count == 0) {
		$district_unique_name = "ON-38-".$district;
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
	$sth = $dbh->prepare('SELECT p.PARTY_ID FROM party p where p.PARTY_ENGLISH_NAME=? and PARTY_UNIQUE_NAME like \'ON%\'');
	$sth->execute($party)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$party_id = $data[0];

	CONTEST_INSERT;

	print "District:$district unique: $district_unique_name Region:$region Sub-Region:$sub_region Votes:$votes Party: $party $party_id Candidate: $candidate won:$won Year:$currentyear Parliament_id:$parliament_id\n";
}

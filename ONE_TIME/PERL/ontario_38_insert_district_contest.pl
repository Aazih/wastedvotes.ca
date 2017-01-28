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

my $district;
my $district_unique_name;
my $plurality;
my $votes;
my $percent;
my $party_abbr;
my $party;
my $party_id;
my $candidate;
my $won;

my $starloc;
my $district_count;
my $district_id;
my $district_current;

my $candidate_residence='N/A';
my $candidate_occupation_english='N/A';
my $candidate_occupation_french='N/A';

my $parliament_id;
my $province_id;
sub DISTRICT_INSERT {
	$sth = $dbh->prepare('INSERT INTO districts(PARLIAMENT_ID,PROVINCE_ID,DISTRICT_UNIQUE_NAME,DISTRICT_FRENCH_NAME,DISTRICT_ENGLISH_NAME) VALUES (?,?,?,?,?)');
	$sth->execute($parliament_id,$province_id,$district_unique_name,$district_current,$district_current) or die "Couldn't execute DISTRICT_INSERT: ".$dbh->errstr;
}

sub CONTEST_INSERT {
	$sth = $dbh->prepare('INSERT INTO contest (DISTRICT_ID, CANDIDATE_NAME, CANDIDATE_PARTY, CANDIDATE_RESIDENCE, PROVINCE_ID,CANDIDATE_OCCUPATION_ENGLISH,CANDIDATE_OCCUPATION_FRENCH,VOTES,CONTEST_NUMBER,WON) VALUES (?,?,?,?,?,?,?,?,1,?)');
	$sth->execute($district_id,$candidate, $party_id, $candidate_residence, $province_id, $candidate_occupation_english, $candidate_occupation_french, $votes, $won) or die "Couldn't execute CONTEST_INSERT: ".$dbh->errstr;
}
open(INFO_FILE, "../../Data/ontario_38_district_contest_1.csv");

$sth = $dbh->prepare('SELECT PARLIAMENT_TYPE_ID FROM parliament_type where PARLIAMENT_TYPE=?');
$sth->execute('ONTARIO')
	or die "Couldn't execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_id = $data[0];
$sth->finish();

$sth = $dbh->prepare('SELECT PROVINCE_ID FROM province where PROVINCE_ENGLISH_NAME=?');
$sth->execute('ONTARIO')
	or die "Couldn't execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$province_id = $data[0];
$sth->finish();

while ($file_line = <INFO_FILE>)
{
	chomp($file_line);
	if ($file_line eq ',,,,,') {
		next;
	} 
	($district,$plurality,$votes,$percent,$party_abbr,$candidate) = split(',',$file_line);
	$starloc = index($candidate,'*');
	if ($starloc != -1) {
		$candidate = substr($candidate,1);
	}

	if ($district ne '') {
		$district_current = $district;
		$won = 1;
	}
	else {
		$won = 0;
	}
	$sth = $dbh->prepare('SELECT p.PARTY_ID, p.PARTY_ENGLISH_NAME FROM party p, ontario_38_party_trans t where t.PARTY_ABBR=? and p.party_id = t.party_id');
	$sth->execute($party_abbr)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$party_id = $data[0];
	$party = $data[1];

	$sth = $dbh->prepare('SELECT count(*) FROM districts where DISTRICT_ENGLISH_NAME=? and PARLIAMENT_ID=?');
	$sth->execute($district_current, $parliament_id)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$district_count = $data[0];

	if ($district_count == 0) {
		$district_unique_name = "ON-38-".$district_current;
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
	$sth->execute($district_current, $parliament_id)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$district_id = $data[0];
	print "$file_line\n";
	print "District: $district_current\n";
	print "District ID is:\t$district_id\n";
	print "Votes: $votes\n";
	print "Party_abbr: $party_abbr\n";
	print "Party: $party\n";
	print "Party id: $party_id\n";
	print "Candidate: $candidate\n";
	print "Won: $won\n";
	CONTEST_INSERT;
	print "Contest inserted\n";
	print "-----\n";
	
}


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

my $workingyear;
my $currentyear;

my $count;
my $rdcount;
my $dcount=0;

sub DISTRICT_INSERT {
	$sth = $dbh->prepare('INSERT INTO districts(PARLIAMENT_ID,PROVINCE_ID,DISTRICT_UNIQUE_NAME,DISTRICT_FRENCH_NAME,DISTRICT_ENGLISH_NAME) VALUES (?,?,?,?,?)');
	$sth->execute($parliament_id,$province_id,$district_unique_name,$district,$district) or die "Couldn't execute DISTRICT_INSERT: ".$dbh->errstr;
}

sub REGION_INSERT {
	$sth = $dbh->prepare('SELECT count(*) FROM region where REGION_UNIQUE_NAME=? and REGION_LEVEL=1');
	$sth->execute($region_unique_name) or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$rdcount = $data[0];

	if ($rdcount == 0) {
		$sth = $dbh->prepare('INSERT INTO region(PARENT_REGION_ID,REGION_LEVEL,PROVINCE_ID,REGION_UNIQUE_NAME,REGION_FRENCH_NAME,REGION_ENGLISH_NAME) VALUES (?,?,?,?,?,?)');
		$sth->execute('0','1',$province_id,$region_unique_name,$region,$region) or die "Couldn't execute REGION_INSERT: ".$dbh->errstr;
	}
}

sub SUB_REGION_INSERT {
	$sth = $dbh->prepare('SELECT count(*) FROM region where REGION_UNIQUE_NAME=? and REGION_LEVEL!=1');
	$sth->execute($region_unique_name) or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$rdcount = $data[0];

	if ($rdcount == 0) {
		$sth = $dbh->prepare('INSERT INTO region(PARENT_REGION_ID,REGION_LEVEL,PROVINCE_ID,REGION_UNIQUE_NAME,REGION_FRENCH_NAME,REGION_ENGLISH_NAME) VALUES (?,?,?,?,?,?)');
		$sth->execute($parent_region_id,'2',$province_id,$region_unique_name,$sub_region,$sub_region) or die "Couldn't execute REGION_INSERT: ".$dbh->errstr;
	}
}


sub INSERT_REGION_DISTRICT {
	$sth = $dbh->prepare('SELECT count(*) FROM region_districts where REGION_ID=? and DISTRICT_ID=?');
	$sth->execute($region_id, $district_id) or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$rdcount = $data[0];

	if ($rdcount == 0) {
		$sth = $dbh->prepare('INSERT INTO region_districts (REGION_ID, DISTRICT_ID) VALUES (?,?)');
		$sth->execute($region_id,$district_id) or die "Couldn't execute INSERT_REGION_DISTRICT: ".$dbh->errstr;
	}

	$sth = $dbh->prepare('SELECT count(*) FROM region_districts where REGION_ID=? and DISTRICT_ID=?');
	$sth->execute($parent_region_id, $district_id) or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$rdcount = $data[0];

	if ($rdcount == 0) {
		$sth = $dbh->prepare('INSERT INTO region_districts (REGION_ID, DISTRICT_ID) VALUES (?,?)');
		$sth->execute($parent_region_id,$district_id) or die "Couldn't execute INSERT_REGION_DISTRICT: ".$dbh->errstr;
	}
}

sub INSERT_REGION_PARLIAMENT {
        $sth = $dbh->prepare('SELECT count(*) FROM region_parliament where REGION_ID=? and PARLIAMENT_ID=?');
        $sth->execute($region_id, $parliament_id) or die "Couldn't execute statement: ". $dbh->errstr;
        @data = $sth->fetchrow_array();
        $rdcount = $data[0];

        if ($rdcount == 0) {
                $sth = $dbh->prepare('INSERT INTO region_parliament (REGION_ID, PARLIAMENT_ID) VALUES (?,?)');
                $sth->execute($region_id,$parliament_id) or die "Couldn't execute INSERT_REGION_PARLIAMENT: ".$dbh->errstr;
        }
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

open(INFO_FILE, "../../Data/ontario-region-2003.csv");

print "StartingLine\nProvince_id is $province_id\tparliament_type_id is $parliament_type_id\n";

$workingyear='N/A';
$count = 1;

$currentyear = 2003;

$sth = $dbh->prepare('SELECT PARLIAMENT_ID, PARLIAMENT_NUMBER FROM parliament where PARLIAMENT_TYPE_ID=? and YEAR(ELECTION_DATE)=?');
$sth->execute($parliament_type_id,$currentyear)
	or die "Could not execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_id = $data[0];
$parliament_number = $data[1];
$sth->finish();

while ($file_line = <INFO_FILE>)
{
	print "Parsing row $count\n";
	$count++;

	chomp($file_line);
	@currentRow = split(/,/,$file_line);

	$region = $currentRow[0];
	$sub_region = $currentRow[1];
	$district = $currentRow[2];
	print "region is $region\tsub_region is $sub_region\tdistrict is $district\n";
	$currentyear = 2003;

	if ($region ne '') {
		$region_unique_name = "ON-".$region;
		REGION_INSERT;

		print "Inserted region $region\n";

	        $sth = $dbh->prepare('SELECT REGION_ID FROM region where REGION_UNIQUE_NAME=?');
	        $sth->execute($region_unique_name)
	        or die "Couldn't execute statement: ". $dbh->errstr;
	        @data = $sth->fetchrow_array();
		$region_id = $data[0];
		INSERT_REGION_PARLIAMENT;

		print "\tregion_id $region_id\n";

		$parent_region_id = $region_id;
	}

	if ($sub_region ne '') {
		$region_unique_name = "ON-".$sub_region;
		SUB_REGION_INSERT;

		print "Inserted sub region $sub_region\n";

	        $sth = $dbh->prepare('SELECT REGION_ID FROM region where REGION_UNIQUE_NAME=?');
	        $sth->execute($region_unique_name)
	        or die "Couldn't execute statement: ". $dbh->errstr;
	        @data = $sth->fetchrow_array();
		$region_id = $data[0];
		INSERT_REGION_PARLIAMENT;

		print "\tregion_id $region_id\n";
	}

	if ($district ne '') {
		print "Working with district $district\n";

		$sth = $dbh->prepare('SELECT count(*) FROM districts where upper(DISTRICT_ENGLISH_NAME)=? and PARLIAMENT_ID=?');
		$sth->execute($district, $parliament_id)
		or die "Couldn't execute statement: ". $dbh->errstr;
		@data = $sth->fetchrow_array();
		$district_count = $data[0];
	
		if ($district_count == 0) {
			$district_unique_name = "ON-38-".$district;
			print "District does not Exist $district\n";
		}
		elsif ($district_count > 1) {
			print "ERROR: District count is more than one and is $district_count\n";
			exit;
		}
		else {
			print "District $district exists\n";
			$dcount++;
		}

		$sth = $dbh->prepare('SELECT DISTRICT_ID FROM districts where upper(DISTRICT_ENGLISH_NAME)=? and PARLIAMENT_ID=?');
		$sth->execute($district, $parliament_id)
		or die "Couldn't execute statement: ". $dbh->errstr;
		@data = $sth->fetchrow_array();
		$district_id = $data[0];

		INSERT_REGION_DISTRICT;

		print "inserted region_id $region_id with district_id $district_id\n";
	}
	print "dcount is $dcount\n";
}

#!/usr/bin/perl

use DBI;
use strict;
use utf8;
use lib "/home/aamir/ElectionsWorking/ElectionsDB/trunk/ONE_TIME/PERL";

require "login.pl";
my $file_line;

my $parliament_type;
my $parliament_id;
my $parliament_abbr;

my @file_array;
my @party_array;
my $party_english;
my $party_french;
my $party_english_abbr;
my $party_french_abbr;
my $party_unique_name;
my $first = 1;

my $dbh;

if ($ARGV[0] eq "DEV") {
	$dbh = LOGIN_DEV();
}
else{
	$dbh = LOGIN_TEST();
}

my $sth;
my @data;

open(PARTY_FILE, "../../Data/federal_39_party.txt");
while ($file_line = <PARTY_FILE>)
{
	if ($first) {
		$first = 0;
		$parliament_type=$file_line;
		chomp($parliament_type);
		if ($parliament_type eq "Federal") {
			$parliament_abbr = "FD";
			$sth = $dbh->prepare('SELECT PARLIAMENT_TYPE_ID FROM parliament_type where PARLIAMENT_TYPE=?');
			$sth->execute($parliament_type)
				or die "Couldn't execute statement: ". $dbh->errstr;
			@data = $sth->fetchrow_array();
			$parliament_id = $data[0];
			$sth->finish();
			print "Working with parliament_type $parliament_type, parliament_id $parliament_id, and parliament_abbr $parliament_abbr\n";
		}
		else {
			print "$parliament_type Not supported yet, script exiting\n";
			exit;
		}
		next;
	}
	#print $file_line."\n";
	@file_array = split(',', $file_line);
	@party_array = split('/',$file_array[0]);
	$party_english = $party_array[0];
	$party_french = $party_array[1];
	$party_english_abbr = $party_array[2];
	$party_french_abbr = $party_array[3];
	chomp($party_english);
	chomp($party_french);
	chomp($party_english_abbr);
	chomp($party_french_abbr);
	$party_unique_name = $parliament_abbr."-".$party_english;
	print "English Name:\t$party_english\nFrench Name:\t$party_french\nUnique Name:\t$party_unique_name\n";
	$sth = $dbh->prepare('INSERT INTO party(PARLIAMENT_TYPE_ID,PARTY_UNIQUE_NAME,PARTY_ENGLISH_NAME,PARTY_FRENCH_NAME,PARTY_ENGLISH_ABBR,PARTY_FRENCH_ABBR) VALUES (?,?,?,?,?,?)');
	$sth = $sth->execute($parliament_id,$party_unique_name,$party_english,$party_french,$party_english_abbr,$party_french_abbr)
		or die "Couldn't execute statement: ". $dbh->errstr;
	print "Done\n";
}

$dbh->disconnect();

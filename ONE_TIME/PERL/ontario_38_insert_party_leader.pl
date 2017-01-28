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

my $party_name;
my $party_id;


my $candidate_name;
my $leader_number;

my @file_array;
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

$parliament_type = "Ontario";
$parliament_abbr = "ON";
$sth = $dbh->prepare('SELECT PARLIAMENT_ID FROM parliament p, parliament_type pt WHERE p.PARLIAMENT_TYPE_ID = pt.PARLIAMENT_TYPE_ID and pt.PARLIAMENT_TYPE=? AND p.PARLIAMENT_NUMBER=38');
$sth->execute($parliament_type)
	or die "Couldn't execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_id = $data[0];
$sth->finish();

open(LEADER_FILE, "../../Data/ontario_38_party_leader.txt");
while ($file_line = <LEADER_FILE>)
{
	@file_array = split('/', $file_line);
	$party_name = $file_array[0];
	$candidate_name = $file_array[1];
	chomp($party_name);
	chomp($candidate_name);
	$sth = $dbh->prepare('SELECT PARTY_ID FROM party where PARTY_ENGLISH_NAME=?');
	$sth->execute($party_name)
		or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$party_id = $data[0];
	$sth->finish();

	$sth = $dbh->prepare('SELECT COUNT(*) FROM party_leader where PARTY_ID=? AND PARLIAMENT_ID=?');
	$sth->execute($party_id,$parliament_id)
		or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$leader_number = $data[0]+1;
	$sth->finish();
	print "Working with parliament_id $parliament_id, and party_id $party_id for party_name $party_name, candidate_name $candidate_name, leader_number decided is $leader_number\n";
	$sth = $dbh->prepare('INSERT INTO party_leader(PARTY_ID, PARLIAMENT_ID,LEADER_NAME,LEADER_NUMBER) VALUES (?,?,?,?)');
	$sth = $sth->execute($party_id,$parliament_id, $candidate_name,$leader_number)
		or die "Couldn't execute statement: ". $dbh->errstr;
	print "Done\n";
}

$dbh->disconnect();

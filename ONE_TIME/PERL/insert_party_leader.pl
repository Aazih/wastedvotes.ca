#!/usr/bin/perl

use DBI;
use strict;
use utf8;
use FindBin;

###
#Including required files
###

use lib "$FindBin::Bin/";
require "login.pl";

my $file_name;
my $file_line;


my $parliament_type;
my $parliament_type_id;
my $parliament_id;
my $parliament_number;

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
elsif ($ARGV[0] eq "TEST"){
	$dbh = LOGIN_TEST();
}

my $sth;
my @data;

$file_name = $ARGV[1];
open(LEADER_FILE, "../../Data/$file_name");

$parliament_type = <LEADER_FILE>;
$parliament_number = <LEADER_FILE>;

chomp($parliament_type);
chomp($parliament_number);

print "Working with parliament_type $parliament_type and parliament_number $parliament_number\n";

$sth = $dbh->prepare('SELECT PARLIAMENT_TYPE_ID FROM parliament_type where PARLIAMENT_TYPE=?');
$sth->execute($parliament_type)
	or die "Couldn't execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_type_id = $data[0];
$sth->finish();

print "parliament_type_id of parliament $parliament_type is $parliament_type_id\n";

$sth = $dbh->prepare('SELECT PARLIAMENT_ID FROM parliament where PARLIAMENT_TYPE_ID=? and PARLIAMENT_NUMBER=?');
$sth->execute($parliament_type_id,$parliament_number)
	or die "Couldn't execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_id = $data[0];
$sth->finish();

print "the parliament_id of parliament_type_id $parliament_type_id and parliament_number $parliament_number is $parliament_id\n";

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

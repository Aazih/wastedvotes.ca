#!/usr/bin/perl

use DBI;
use strict;
use utf8;
use lib "/home/aamirhussain/webwork/svnrep/PROJECTS/ElectionsDB/trunk/ONE_TIME/PERL";

require "login.pl";
my $file_name;
my $file_line;
my $numArgs;

my $parliament_type;
my $parliament_type_id;
my $parliament_abbr;

my @party_array;
my $party_english;
my $party_french;
my $party_english_abbr;
my $party_french_abbr;
my $party_colour;
my $party_unique_name;
my $first = 1;
my $count;

my $dbh;

if ($ARGV[0] eq "DEV") {
	$dbh = LOGIN_DEV();
}

my $sth;
my @data;

$numArgs = $#ARGV + 1;

if ($numArgs != 2) {
  print "Database and then Filename of file to be loaded should be the only arguments provided to this script currently providing $numArgs arguments\n";
  exit;
}

$file_name = $ARGV[1];
open(PARTY_FILE, "../../Data/$file_name");

$parliament_type = <PARTY_FILE>;

chomp($parliament_type);

$sth = $dbh->prepare('SELECT PARLIAMENT_TYPE_ID,PARLIAMENT_ABBR FROM parliament_type where PARLIAMENT_TYPE=?');
$sth->execute($parliament_type)
	or die "Couldn't execute statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_type_id = $data[0];
$parliament_abbr = $data[1];
$sth->finish();

print "Working with parliament_type $parliament_type, parliament_type_id $parliament_type_id, and parliament_abbr $parliament_abbr\n";
	#print $file_line."\n";
while ($file_line = <PARTY_FILE>)
{
	#print $file_line."\n";
	@party_array = split('/',$file_line);
	$party_english = $party_array[0];
	$party_french = $party_array[1];
	$party_english_abbr = $party_array[2];
	$party_french_abbr = $party_array[3];
        $party_colour = $party_array[4];
	chomp($party_english);
	chomp($party_french);
	chomp($party_english_abbr);
	chomp($party_french_abbr);
	$party_unique_name = $parliament_abbr."-".$party_english;
        $sth = $dbh->prepare('SELECT COUNT(*) FROM party where PARTY_UNIQUE_NAME=?');
        $sth->execute($party_unique_name)
                or die "Couldn't execute statement: ". $dbh->errstr;
        @data = $sth->fetchrow_array();
        $count = @data[0];
        $sth->finish();

	print "English Name:\t$party_english\nFrench Name:\t$party_french\nUnique Name:\t$party_unique_name\n";
        if ($count > 0) {
            print "Already exists and so not inserting\n";
        } else {
            print "Does not exist and so inserting\n";
	$sth = $dbh->prepare('INSERT INTO party(PARLIAMENT_TYPE_ID,PARTY_UNIQUE_NAME,PARTY_ENGLISH_NAME,PARTY_FRENCH_NAME,PARTY_ENGLISH_ABBR,PARTY_FRENCH_ABBR, PARTY_COLOUR) VALUES (?,?,?,?,?,?,?)');
	$sth = $sth->execute($parliament_type_id,$party_unique_name,$party_english,$party_french,$party_english_abbr,$party_french_abbr,$party_colour)
		or die "Couldn't execute statement: ". $dbh->errstr;
        }
	print "Done\n";
}

$dbh->disconnect();

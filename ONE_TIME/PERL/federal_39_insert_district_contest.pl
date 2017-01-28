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

my $first = 1;
my $file_line;
my $field;
my $second_quote = -1;
my $test;
my $word;

my $parliament_id;

my $province_file;
my $prov_english;
my $prov_french;
my $province_id;

my $district;
my $district_english;
my $district_french;
my $district_unique_name;
my $district_id;
my $district_count;

my $candidate_name;
my $party_english;
my $party_french;
my $party_id;

my $candidate_residence;
my $cand_prov_eng;
my $cand_prov_fre;
my $candidate_residence_province_id;
my $candidate_occupation_french;
my $candidate_occupation_english;
my $num_votes;
my $percent;
my $majority;
my $won;

sub GET_WORD {
	my $first_quote;
	$first_quote = index($file_line,'"', $second_quote+1);
	if ($first_quote == -1) {
		return -1;
	}
	$second_quote = index($file_line,'"',$first_quote+1);
	$field = substr($file_line,$first_quote+1,$second_quote-$first_quote-1);
	#print "$first_quote $second_quote\n";
	return $field;
}

sub GET_NUMBER {
	my $current_pos;
	my $current_char;
	my $in_number;
	my $number_start;
	my $number_end;

	$in_number = 0;
	$current_pos = $second_quote+1;
	while ($in_number == 0) {
		if ($current_pos > length($file_line)){
			return -1;
		}
		$current_char = substr($file_line,$current_pos,1);
		if ($current_char =~ m/[0-9]/ or $current_char eq '.'){ 
			$number_start=$current_pos;
			$in_number = 1;
		}
		else {
			$current_pos++;
		}
	}
	while ($in_number == 1) {
		$current_char = substr($file_line,$current_pos,1);
		if ($current_char =~ m/[0-9]/ or $current_char eq '.'){
			$current_pos++;
		}
		else {
			$in_number = 0;
			$number_end=$current_pos;
		}
	}
	$second_quote=$current_pos;
	return substr($file_line,$number_start,$number_end-$number_start);
}

sub PARSE_PROVINCE {
	my $slash_loc;
	$slash_loc = index($province_file,'/');
	if ($slash_loc == -1) {
		$prov_english = $province_file;
		$prov_french = $province_file;
	}
	else {
		$prov_english = substr($province_file,0,$slash_loc);
		$prov_french = substr($province_file,$slash_loc+1);
	}
	$sth = $dbh->prepare('SELECT PROVINCE_ID FROM province where PROVINCE_ENGLISH_NAME=?');
	$sth->execute($prov_english)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$province_id = $data[0];
	return;
}

sub PARSE_NAME_PARTY {
	my $party_loc;
	my $star_loc;
	$sth = $dbh->prepare('SELECT PARTY_ID, PARTY_ENGLISH FROM party_trans_39 where PARTY_FRENCH=?');
	$sth->execute($party_french)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$party_id = $data[0];
	$party_english = $data[1];
	$party_loc = index($word,$party_english,0);
	$candidate_name = substr($word,0,$party_loc-1);
	$star_loc = index($candidate_name, '**',0);
	if ($star_loc != -1) {
		$candidate_name = substr($candidate_name,0,$star_loc-1);
	}
}

sub PARSE_RESIDENCE {
	my $slash_loc;
	my $prov_loc;
	my $comma_loc;

	$slash_loc = index($word,'/',0);
	if ($slash_loc > 0) {
		$cand_prov_fre = substr($word,$slash_loc+1);
	}
	else {
		$comma_loc = index($word,',',0);
		$cand_prov_fre = substr($word,$comma_loc+2);
	}
	$sth = $dbh->prepare('SELECT PROVINCE_ID, PROVINCE_ENGLISH_ABBREVIATION FROM province where PROVINCE_FRENCH_ABBREVIATION=?');
	$sth->execute($cand_prov_fre)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$candidate_residence_province_id = $data[0];
	$cand_prov_eng = $data[1];
	$prov_loc = index($word,$cand_prov_eng,0);
	$candidate_residence = substr($word,0,$prov_loc-2);
}

sub PARSE_OCCUPATION {
	my $slash_loc;

	$slash_loc = rindex($word,'/');
	$candidate_occupation_english = substr($word,0,$slash_loc);
	$candidate_occupation_french = substr($word,$slash_loc+1);
}


sub DISTRICT_INSERT {
	$sth = $dbh->prepare('INSERT INTO districts(PARLIAMENT_ID,PROVINCE_ID,DISTRICT_UNIQUE_NAME,DISTRICT_FRENCH_NAME,DISTRICT_ENGLISH_NAME) VALUES (1,?,?,?,?)');
	$sth->execute($province_id,$district_unique_name,$district_french,$district_english) or die "Couldn't execute DISTRICT_INSERT: ".$dbh->errstr;
}

sub CONTEST_INSERT {
	$sth = $dbh->prepare('INSERT INTO contest (DISTRICT_ID, CANDIDATE_NAME, CANDIDATE_PARTY, CANDIDATE_RESIDENCE, PROVINCE_ID,CANDIDATE_OCCUPATION_ENGLISH,CANDIDATE_OCCUPATION_FRENCH,VOTES,CONTEST_NUMBER,WON) VALUES (?,?,?,?,?,?,?,?,1,?)');
	$sth->execute($district_id,$candidate_name, $party_id, $candidate_residence, $candidate_residence_province_id,$candidate_occupation_english, $candidate_occupation_french, $num_votes, $won) or die "Couldn't execute CONTEST_INSERT: ".$dbh->errstr;
}
	
my $count =0;

$sth = $dbh->prepare('select parliament_id from parliament where parliament_type_id = (select parliament_type_id from parliament_type where parliament_type = ?) and parliament_number = ?');
$sth->execute('Federal',39)
or die "Couldn't exeucte statement: ". $dbh->errstr;
@data = $sth->fetchrow_array();
$parliament_id= $data[0];

print "working with parliament_id $parliament_id\n";

open(INFO_FILE, "../../Data/federal_39_district_contest.csv");
while ($file_line = <INFO_FILE>)
{
	$second_quote=-1;
	if ($first) {
			$first = 0;
			next;
	}
	$province_file = GET_WORD;
	PARSE_PROVINCE;
	$district = GET_WORD;
	($district_english,$district_french) = split('/',$district);
	if ($district_french eq '') { $district_french=$district_english; }
	$word = GET_WORD;
	$party_french = GET_WORD;
	PARSE_NAME_PARTY;
	$word = GET_WORD;
	PARSE_RESIDENCE;
	$word = GET_WORD;
	PARSE_OCCUPATION;
	$num_votes = GET_NUMBER;
	$percent = GET_NUMBER;
	$majority = GET_NUMBER;
	
	print "Province:\t$province_file\n";
	print "Province_English:\t$prov_english\n";
	print "Province_French:\t$prov_french\n";
	print "Province_ID:\t$province_id\n";
	print "District:\t$district\n";
	print "District English:\t$district_english\n";
	print "District French:\t$district_french\n";
	print "Candidate Name:\t$candidate_name\n";
	print "Party French:\t$party_french\n";
	print "Party English:\t$party_english\n";
	print "Party ID:\t$party_id\n";
	print "Candidate Residence:\t$candidate_residence\n";
	print "Residence French Abbr:\t$cand_prov_fre\n";
	print "Residence English Abbr:\t$cand_prov_eng\n";
	print "Canadidate Residence Province:\t$candidate_residence_province_id\n";
	print "Candidate Occupation English:\t$candidate_occupation_english\n";
	print "Candidate Occupation French:\t$candidate_occupation_french\n";
	print "Number of votes received:\t$num_votes\n";
	print "Extra votes received:\t$majority\n";
	$sth = $dbh->prepare('SELECT count(*) FROM districts where DISTRICT_ENGLISH_NAME=? and PARLIAMENT_ID=?');
	$sth->execute($district_english,$parliament_id)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$district_count = $data[0];
	$count++;
	print "District count in table is:\t$district_count\n";
	if ($district_count == 0) {
		$district_unique_name = "FD-39-".$district_english;
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
	$sth = $dbh->prepare('SELECT DISTRICT_ID FROM districts where DISTRICT_ENGLISH_NAME=? AND PARLIAMENT_ID=1');
	$sth->execute($district_english)
	or die "Couldn't execute statement: ". $dbh->errstr;
	@data = $sth->fetchrow_array();
	$district_id = $data[0];
	print "District ID is:\t$district_id\n";
	if ($majority > 0) {
		$won = 1
	}
	else {
		$won = 0
	}
	CONTEST_INSERT;
	print "Contest inserted\n";
	print "*****************************\n";
}


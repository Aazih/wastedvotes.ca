#!/usr/bin/perl

use DBI;
use strict;
use utf8;

sub LOGIN_DEV {
	my %config;

	$config{'dbServer'} = "localhost";
	$config{'dbUser'} = "root";
	$config{'dbPass'} = "mysql";
	$config{'dbName'} = "elections_dev";

	$config{'dataSource'} = "DBI:mysql:$config{'dbName'}:$config{'dbServer'}";

	my $dbh = DBI->connect($config{'dataSource'},$config{'dbUser'},$config{'dbPass'}) or
		die "Can't connect to $config{'dataSource'}\n<br>$DBI::errstr";
	return $dbh;
}

sub LOGIN_TEST {
	my %config;

	$config{'dbServer'} = "localhost";
	$config{'dbUser'} = "root";
	$config{'dbPass'} = "mysql";
	$config{'dbName'} = "elections_test";

	$config{'dataSource'} = "DBI:mysql:$config{'dbName'}:$config{'dbServer'}";

	my $dbh = DBI->connect($config{'dataSource'},$config{'dbUser'},$config{'dbPass'}) or
		die "Can't connect to $config{'dataSource'}\n<br>$DBI::errstr";
	return $dbh;
}
1;

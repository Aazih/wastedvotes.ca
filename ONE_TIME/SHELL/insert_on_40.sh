#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
echo "Rerunning ontario party insert to create new parties"
../PERL/insert_party.pl $DB_PERL ontario_party.txt
echo "Running sql to create parliament"
mysql $DB -u root -p$PASS < ../SQL/on_40_parliament.sql
echo "Running perl to create district_contest"
../PERL/insert_district_contest.pl $DB_PERL on_40_district_contest.csv
echo "Running perl to create party leader"
../PERL/insert_party_leader.pl $DB_PERL on_40_party_leader.txt

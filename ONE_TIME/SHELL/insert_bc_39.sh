#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
mysql $DB -u root -p$PASS < ../SQL/bc_39_parliament.sql
../PERL/insert_district_contest.pl $DB_PERL bc_39_district_contest.csv
../PERL/insert_party_leader.pl $DB_PERL bc_39_party_leader.txt

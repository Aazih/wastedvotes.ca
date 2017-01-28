#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
mysql $DB -u root -p$PASS < ../SQL/misc_42.sql
mysql $DB -u root -p$PASS < ../SQL/fd_42_parliament.sql
../PERL/insert_party.pl $DB_PERL federal_party.txt
../PERL/insert_district_contest.pl $DB_PERL fd_42_district_contest.csv
../PERL/insert_party_leader.pl $DB_PERL fd_42_party_leader.txt

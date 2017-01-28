#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
mysql $DB -u root -p$PASS < ../SQL/federal_40_parliament.sql
../PERL/federal_40_insert_district_contest.pl $DB_PERL
mysql $DB -u root -p$PASS < ../SQL/federal_40_calc_winner.sql
../PERL/insert_party_leader.pl $DB_PERL federal_40_party_leader.txt

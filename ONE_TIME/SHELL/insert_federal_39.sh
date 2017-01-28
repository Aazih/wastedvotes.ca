#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
mysql $DB -u root -p$PASS < ../SQL/federal_39_party_trans.sql
mysql $DB -u root -p$PASS < ../SQL/federal_39_parliament.sql
../PERL/federal_39_insert_district_contest.pl $DB_PERL
../PERL/insert_party_leader.pl $DB_PERL federal_39_party_leader.txt
mysql $DB -u root -p$PASS < ../SQL/federal_39_party_logo_insert.sql

#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
mysql $DB -u root -p$PASS < ../SQL/novascotia_parliament_type.sql
mysql $DB -u root -p$PASS < ../SQL/novascotia_party.sql
mysql $DB -u root -p$PASS < ../SQL/ns_2009_parliament.sql
../PERL/insert_district_contest.pl $DB_PERL novascotia_61_district_contest.csv
../PERL/insert_party_leader.pl $DB_PERL novascotia_61_party_leader.txt

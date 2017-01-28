#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
mysql $DB -u root -p$PASS < ../SQL/bc_parliament_type.sql
../PERL/bc_2005_insert_party.pl $DB_PERL
mysql $DB -u root -p$PASS < ../SQL/bc_2005_parliament.sql
../PERL/bc_2005_insert_district_contest.pl $DB_PERL
../PERL/bc_2005_insert_party_leader.pl $DB_PERL

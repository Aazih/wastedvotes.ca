#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
mysql $DB -u root -p$PASS < ../SQL/ontario_38_parliament_type.sql
mysql $DB -u root -p$PASS < ../SQL/ontario_38_party.sql
mysql $DB -u root -p$PASS < ../SQL/ontario_38_party_trans.sql
mysql $DB -u root -p$PASS < ../SQL/ontario_38_parliament.sql
../PERL/ontario_2003_insert_district_contest.pl $DB_PERL
../PERL/ontario_2003_insert_region.pl $DB_PERL
../PERL/ontario_38_insert_party_leader.pl $DB_PERL
mysql $DB -u root -p$PASS < ../SQL/ontario_38_party_logo_insert.sql

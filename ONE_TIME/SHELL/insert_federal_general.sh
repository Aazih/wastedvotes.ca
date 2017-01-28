#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
mysql $DB -u root -p$PASS < ../SQL/federal_parliament_type.sql
../PERL/insert_party.pl $DB_PERL federal_party.txt

#!/bin/bash

DB=$1
PASS=Disiselection
mysql $DB -u root -p$PASS < ../../SCHEMA/province.sql
mysql $DB -u root -p$PASS < ../../SCHEMA/parliament_type.sql
mysql $DB -u root -p$PASS < ../../SCHEMA/party.sql
mysql $DB -u root -p$PASS < ../../SCHEMA/parliament.sql
mysql $DB -u root -p$PASS < ../../SCHEMA/party_leader.sql
mysql $DB -u root -p$PASS < ../../SCHEMA/districts.sql
mysql $DB -u root -p$PASS < ../../SCHEMA/region.sql
mysql $DB -u root -p$PASS < ../../SCHEMA/contest.sql
mysql $DB -u root -p$PASS < ../../SCHEMA/party_trans_39.sql
mysql $DB -u root -p$PASS < ../../SCHEMA/ontario_38_party_trans.sql
mysql $DB -u root -p$PASS < ../../SCHEMA/pic_resource.sql
mysql $DB -u root -p$PASS < ../SQL/prov_insert.sql
mysql $DB -u root -p$PASS <../SQL/region_flag_insert.sql

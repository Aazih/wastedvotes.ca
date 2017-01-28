#!/bin/bash

DB=$1
PASS=Disiselection
mysql $DB -u root -p$PASS < ../SQL/drop_all.sql

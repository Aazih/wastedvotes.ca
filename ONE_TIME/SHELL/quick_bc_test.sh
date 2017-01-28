#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
./drop_all.sh $1
./create_schema.sh $1
./insert_bc_2005.sh $1 $2
./insert_bc_39.sh $1 $2
./final_inserts.sh $1 $2

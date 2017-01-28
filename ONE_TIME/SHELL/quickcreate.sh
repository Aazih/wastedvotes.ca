#!/bin/bash

DB=$1
PASS=Disiselection
DB_PERL=$2
./drop_all.sh $1
./create_schema.sh $1
./insert_federal_general.sh $1 $2
./insert_federal_39.sh $1 $2
./insert_ontario_general.sh $1 $2
./insert_ontario_81-03.sh $1 $2
./insert_bc_2005.sh $1 $2
./insert_federal_40.sh $1 $2
./insert_ns_61.sh $1 $2
./insert_bc_39.sh $1 $2
./insert_on_39.sh $1 $2
./insert_on_40.sh $1 $2
./insert_ns_62.sh $1 $2
./insert_on_41.sh $1 $2
./final_inserts.sh $1 $2

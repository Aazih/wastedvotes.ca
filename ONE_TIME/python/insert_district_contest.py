import sys
import pymysql
import wavdefs

ins_district_sql = "INSERT INTO districts(PARLIAMENT_ID,PROVINCE_ID,DISTRICT_UNIQUE_NAME,DISTRICT_FRENCH_NAME,DISTRICT_ENGLISH_NAME) VALUES ('%s','%s','%s','%s','%s')"
ins_contest_sql =  "INSERT INTO contest (DISTRICT_ID, CANDIDATE_NAME, CANDIDATE_PARTY, PROVINCE_ID,VOTES,CONTEST_NUMBER) VALUES ('%s','%s',%s,%s,%s,1)"

ins_district_count = 0
ins_contest_count = 0

print("Executing insert_district_contest.py Python script. Please provide full path to election result file as argument")
print("***")
conn = wavdefs.logindev()

if len(sys.argv) != 2:
    print("Must provide file name of election parites to load with full path")
    sys.exit(2)

print("Retrieving parliament type details")
print("***")
resultfile = open(sys.argv[1], encoding="utf-8")

parliament_type = resultfile.readline()

parliament_type = parliament_type.strip() 

cur = conn.cursor()

sql = "SELECT PARLIAMENT_TYPE_ID,PARLIAMENT_ABBR FROM parliament_type where PARLIAMENT_TYPE='%s'"

cur.execute(sql % parliament_type)

result = cur.fetchone()

parliament_type_id = result[0]
parliament_abbr = result[1]

print("Working with parliament type %s and parliament_type_id %d and parliament_abbr %s" % (parliament_type, parliament_type_id, parliament_abbr))
print("***")

print("Retrieving parliament details")
print("***")

parliament_number = resultfile.readline()
parliament_number = parliament_number.strip()

sql = "SELECT PARLIAMENT_ID FROM parliament where PARLIAMENT_TYPE_ID=%s and PARLIAMENT_NUMBER=%s"
cur.execute(sql % (parliament_type_id,parliament_number))

result = cur.fetchone()

parliament_id = result[0]

print("Working with parliament_id", parliament_id)


print("Now processing party details")
loopcount = 0

for resultline in resultfile:
    loopcount = loopcount + 1
    print(resultline.strip())
    splitline = resultline.split(',')
    new_province = splitline[0].strip()
    new_district = splitline[1].strip()
    new_district_french = splitline[2].strip()
    candidate = splitline[3].strip()
    candidate_french = splitline[4].strip()
    party = splitline[5].strip()
    votes = splitline[6].strip()

    if new_province != '' : province=new_province
    if new_district != '' : district=new_district

    sql = "SELECT PROVINCE_ID FROM province where PROVINCE_ENGLISH_NAME='%s'"
    cur.execute(sql % province)
    result = cur.fetchone()
    province_id = result[0]

    sql = "SELECT count(*) FROM districts where DISTRICT_ENGLISH_NAME='%s' and PARLIAMENT_ID='%s'"
    cur.execute(sql % (district, parliament_id))
    result = cur.fetchone()
    districtExist = result[0]
    
    if districtExist == 0 :
        district_unique_name = parliament_abbr+"-"+parliament_number+"-"+district
        print ("Created unique_name: " + district_unique_name)
        cur.execute(ins_district_sql % (parliament_id, province_id, district_unique_name, district, district))
        ins_district_count = ins_district_count+1
    elif districtExist > 1 :
        print ("ERROR! more than one district found of this name which should never happen")
    else :
        print ("District already exists")
    
    sql = "SELECT DISTRICT_ID FROM districts where DISTRICT_ENGLISH_NAME='%s' and PARLIAMENT_ID=%s"
    cur.execute(sql % (district, parliament_id))
    result = cur.fetchone()
    district_id = result[0]

    sql = "SELECT p.PARTY_ID FROM party p where p.PARTY_ENGLISH_NAME='%s' and PARLIAMENT_TYPE_ID=%s"
    cur.execute(sql % (party, parliament_type_id))
    result = cur.fetchone()
    party_id = result[0]
    print("District %s %s Province %s %s votes %s party %s %s candidate %s parliament_id %s" % (district, district_id, province, province_id, str(votes), party, party_id, candidate, parliament_id))

    #Now check if this district plus candidate name pair already exists. If so skip otherwise insert.
    sql = "SELECT count(*) FROM contest where DISTRICT_ID='%s' and CANDIDATE_NAME='%s'"
    cur.execute(sql % (district_id, candidate))
    result = cur.fetchone()
    contestExist = result[0]
    if contestExist == 0 :
        print("Inserting contest result")
        cur.execute(ins_contest_sql % (district_id, candidate, party_id, province_id, votes))
        ins_contest_count = ins_contest_count+1
    elif contestExist > 1 :
        print ("ERROR! more than one contest found in this district for candidate which should never happen")
    else :
        print ("Result for this district and contest already exists, skipping insert")
    print("")

#Calculate Winner
print("Calculating contest winner")
cur.execute("create table contest_result (DISTRICT_ID INT, CANDIDATE_NAME VARCHAR(100))")
cur.execute("INSERT INTO contest_result (DISTRICT_ID, CANDIDATE_NAME) SELECT DISTRICT_ID, CANDIDATE_NAME FROM contest o WHERE DISTRICT_ID IN (SELECT DISTRICT_ID from districts where PARLIAMENT_ID = '%s' AND o.VOTES = (SELECT MAX(votes) FROM contest WHERE DISTRICT_ID = o.DISTRICT_ID))" % parliament_id)
cur.execute("update contest c SET WON = 1 WHERE (c.DISTRICT_ID, c.CANDIDATE_NAME) IN  (select DISTRICT_ID, CANDIDATE_NAME from contest_result)")
cur.execute("drop table contest_result")

#TODO: Put in Calculate constest vote percent for null

print ("%d contest lines in result file" % loopcount)

cur.close()
conn.close()

print ("Insterted %d districts" % ins_district_count)
print ("Insterted %d contests" % ins_contest_count)
print("Script processing ended")
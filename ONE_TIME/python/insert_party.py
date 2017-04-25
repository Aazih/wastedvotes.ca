import sys
import pymysql
import wavdefs

print("Executing insert_party.py Python script. Please provide full path to election party file as argument")
print("***")
conn = wavdefs.logindev()

if len(sys.argv) != 2:
    print("Must provide file name of election parites to load with full path")
    sys.exit(2)

print("Retrieving parliament details")
print("***")
partyfile = open(sys.argv[1])

parliament_type = partyfile.readline()

parliament_type = parliament_type.strip() 

cur = conn.cursor()

sql = "SELECT PARLIAMENT_TYPE_ID,PARLIAMENT_ABBR FROM parliament_type where PARLIAMENT_TYPE='%s'"

cur.execute(sql % parliament_type)

result = cur.fetchone()

parliament_type_id = result[0]
parliament_abbr = result[1]

print("Working with parliament type %s and parliament_type_id %d and parliament_abbr %s" % (parliament_type, parliament_type_id, parliament_abbr))
print("***")
print("Now processing party details")
for partyline in partyfile:
    splitline = partyline.split('/')
    #print(splitline)
    party_english_name = splitline[0].strip()
    party_french_name = splitline[1].strip()
    party_english_abbr = splitline[2].strip()
    party_french_abbr = splitline[3].strip()
    #party_colour = splitline[4].strip()
    party_color = ''

    party_unique_name = parliament_abbr+'-'+party_english_name
    print ("Processing unique %s, english %s, french %s" % (party_unique_name,party_english_name,party_french_name))

    sql = "SELECT COUNT(*) FROM party where PARTY_UNIQUE_NAME='%s'"
    cur.execute(sql % party_unique_name)
    partyexist = cur.fetchone()
    
    if partyexist[0] > 0 :
        print("Party already exists")
    else :
        print("Party does not exist need to insert")
        sql = "INSERT INTO party(PARLIAMENT_TYPE_ID,PARTY_UNIQUE_NAME,PARTY_ENGLISH_NAME,PARTY_FRENCH_NAME,PARTY_ENGLISH_ABBR,PARTY_FRENCH_ABBR, PARTY_COLOUR) VALUES (%s,%s,%s,%s,%s,%s,%s)"
        cur.execute(sql,  (parliament_type_id, party_unique_name, party_english_name, party_french_name, party_english_abbr, party_french_abbr, party_color) )
        result = cur.fetchone()
        print(result)


cur.close()
conn.close()
print("Script processing ended")
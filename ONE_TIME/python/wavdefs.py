import pymysql

def logindev ():
    "This function just logs in to the elections_dev database"
    conn = pymysql.connect(host='localhost', port=3306, user='root', passwd='mysql', db='elections_dev', use_unicode='True',charset='utf8')
    return conn


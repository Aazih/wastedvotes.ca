CREATE TABLE districts(
DISTRICT_ID INT NOT NULL AUTO_INCREMENT,
PRIMARY KEY (DISTRICT_ID),
PARLIAMENT_ID INT NOT NULL,
FOREIGN KEY (PARLIAMENT_ID)
REFERENCES parliament(PARLIAMENT_ID)
ON UPDATE NO ACTION ON DELETE NO ACTION,
PROVINCE_ID INT NOT NULL,
FOREIGN KEY (PROVINCE_ID)
REFERENCES province(PROVINCE_ID)
ON UPDATE NO ACTION ON DELETE NO ACTION,
DISTRICT_UNIQUE_NAME VARCHAR(100) NOT NULL,
DISTRICT_FRENCH_NAME VARCHAR(100) NOT NULL,
DISTRICT_ENGLISH_NAME VARCHAR(100) NOT NULL);

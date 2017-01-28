insert into party(PARLIAMENT_TYPE_ID,PARTY_UNIQUE_NAME,PARTY_ENGLISH_NAME,PARTY_FRENCH_NAME,PARTY_ENGLISH_ABBR,PARTY_FRENCH_ABBR,PARTY_COLOUR)
values ((select parliament_type_id from parliament_type where parliament_type='Nova Scotia'),
'NS-Nova Scotia Liberal Party','Nova Scotia Liberal Party','Nova Scotia Liberal Party','Liberal','Liberal','F08080');

insert into party(PARLIAMENT_TYPE_ID,PARTY_UNIQUE_NAME,PARTY_ENGLISH_NAME,PARTY_FRENCH_NAME,PARTY_ENGLISH_ABBR,PARTY_FRENCH_ABBR,PARTY_COLOUR)
values ((select parliament_type_id from parliament_type where parliament_type='Nova Scotia'),
'NS-Nova Scotia New Democratic Party','Nova Scotia New Democratic Party','Nova Scotia New Democratic Party','NDP','NDP','F4A460');

insert into party(PARLIAMENT_TYPE_ID,PARTY_UNIQUE_NAME,PARTY_ENGLISH_NAME,PARTY_FRENCH_NAME,PARTY_ENGLISH_ABBR,PARTY_FRENCH_ABBR,PARTY_COLOUR)
values ((select parliament_type_id from parliament_type where parliament_type='Nova Scotia'),
'NS-Progressive Conservative Association of Nova Scotia','Progressive Conservative Association of Nova Scotia','Progressive Conservaitve Association of Nova Scotia','PC','PC','6495ED');

insert into party(PARLIAMENT_TYPE_ID,PARTY_UNIQUE_NAME,PARTY_ENGLISH_NAME,PARTY_FRENCH_NAME,PARTY_ENGLISH_ABBR,PARTY_FRENCH_ABBR,PARTY_COLOUR)
values ((select parliament_type_id from parliament_type where parliament_type='Nova Scotia'),
'NS-Green Party of Nova Scotia','Green Party of Nova Scotia','Green Party of Nova Scotia','GP','GP','9ACD32');

insert into party(PARLIAMENT_TYPE_ID,PARTY_UNIQUE_NAME,PARTY_ENGLISH_NAME,PARTY_FRENCH_NAME,PARTY_ENGLISH_ABBR,PARTY_FRENCH_ABBR,PARTY_COLOUR)
values ((select parliament_type_id from parliament_type where parliament_type='Nova Scotia'),
'NS-Independent', 'Independent','Independent','Ind','Ind','736F6E');

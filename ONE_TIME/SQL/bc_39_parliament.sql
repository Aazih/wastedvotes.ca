insert into parliament(PARLIAMENT_TYPE_ID,GOVERNING_PARTY,PARLIAMENT_NUMBER,START_DATE,ELECTION_DATE)
values ((select parliament_type_id from parliament_type where parliament_type='British Columbia'),
(select party_id from party where party_unique_name='BC-British Columbia Liberal Party'),
39,'20090825','20090512');

insert into parliament(PARLIAMENT_TYPE_ID,GOVERNING_PARTY,PARLIAMENT_NUMBER,START_DATE,ELECTION_DATE)
values ((select parliament_type_id from parliament_type where parliament_type='Ontario'),
(select party_id from party where party_unique_name='ON-Ontario Liberal Party'),
41,'20140612','20140612');

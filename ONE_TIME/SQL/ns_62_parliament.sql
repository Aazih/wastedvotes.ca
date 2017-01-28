insert into parliament(PARLIAMENT_TYPE_ID,GOVERNING_PARTY,PARLIAMENT_NUMBER,START_DATE,ELECTION_DATE)
values ((select parliament_type_id from parliament_type where parliament_type='Nova Scotia'),
(select party_id from party where party_unique_name='NS-Nova Scotia Liberal Party'),
62,'20131024','20131008');

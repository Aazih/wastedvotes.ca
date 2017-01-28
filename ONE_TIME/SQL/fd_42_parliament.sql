insert into parliament(PARLIAMENT_TYPE_ID,GOVERNING_PARTY,PARLIAMENT_NUMBER,START_DATE,ELECTION_DATE)
values ((select parliament_type_id from parliament_type where parliament_type='Federal'),
(select party_id from party where party_unique_name='FD-Liberal Party of Canada'),
42,'20151203','20151019');

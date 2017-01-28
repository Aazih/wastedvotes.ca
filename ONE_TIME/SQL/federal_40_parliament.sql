insert into parliament(PARLIAMENT_TYPE_ID,GOVERNING_PARTY,PARLIAMENT_NUMBER,ELECTION_DATE,START_DATE)
values ((select parliament_type_id from parliament_type where parliament_type='Federal'),
(select party_id from party where party_unique_name='FD-Conservative Party of Canada'),
40,'20081014','20081118');

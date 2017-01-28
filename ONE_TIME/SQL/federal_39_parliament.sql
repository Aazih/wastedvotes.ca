insert into parliament(PARLIAMENT_TYPE_ID,GOVERNING_PARTY,PARLIAMENT_NUMBER,START_DATE,ELECTION_DATE,END_DATE)
values ((select parliament_type_id from parliament_type where parliament_type='Federal'),
(select party_id from party where party_unique_name='FD-Conservative Party of Canada'),
39,'20060403','20060123','20080907');

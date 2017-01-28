<?php
$total_query = <<<TOTAL
select max(TOTAL) TOTAL, max(WASTE) WASTE from
(select sum(votes) TOTAL, 0 WASTE
from contest
where 1=1 CONDITION
UNION
select 0 TOTAL, sum(votes) WASTE
from contest
where won=FALSE CONDITION) AS S
TOTAL;

$total_seat_query = <<<TOTAL_SEAT
select count(*) TOTAL_SEAT
from districts
where 1=1 CONDITION
TOTAL_SEAT;


$twaste_query = <<<TWASTE
select p.PARLIAMENT_NUMBER, YEAR(p.ELECTION_DATE) ELECTION_YEAR, pt.PARLIAMENT_TYPE, d.DISTRICT_ID, d.DISTRICT_ENGLISH_NAME, d.DISTRICT_FRENCH_NAME, pr.PROVINCE_ENGLISH_ABBREVIATION, pr.PROVINCE_FRENCH_ABBREVIATION, 100-c.vote_percent WASTED
from contest c, districts d, parliament p, parliament_type pt, province pr 
where c.won=TRUE 
and d.DISTRICT_ID=c.DISTRICT_ID 
and d.PARLIAMENT_ID=p.PARLIAMENT_ID 
and p.PARLIAMENT_TYPE_ID=pt.PARLIAMENT_TYPE_ID 
and d.PROVINCE_ID=pr.PROVINCE_ID
and c.vote_percent < 50 CONDITION
order by vote_percent asc
TWASTE;
?>

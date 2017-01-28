<?php
require_once ("login.inc");
require_once ("global.inc");
require_once ("function.inc");
class Party {
	var $party_id;
	var $party_english;
	var $party_french;
	var $party_unique;
	var $parliament_type_id;
	var $parliament_id;

	var $loop_max;

	var $num_candidates;
	var $num_seats;

	var $stat_query;
	var $district_query;
	
	var $dbcnx;
	//--------------
	// CONSTRUCTOR
	function Party($in_name, $parliament_number) {
		$this->loop_max = 5;
		$this->party_unique = $in_name;
		$this->dbcnx = LoginDev();
		$sqlquery = <<<QUERY
select PARTY_ID, PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME, p.PARLIAMENT_TYPE_ID, PARLIAMENT_ID
from party p, parliament t
where party_unique_name='$this->party_unique'
and p.parliament_type_id=t.parliament_type_id
and t.parliament_number=$parliament_number
QUERY;
		$result = mysqli_query($dbcnx, $sqlquery) or die(mysqli_error($dbcnx));
		if (!result) {
			exit('<p> Unable to execute Constructor Query');
		}
		$row = mysqli_fetch_row($result);
		if ($row == FALSE) {
			exit();
			echo "Information not available for Constructor.<BR>";
		}
		$this->party_id = $row[0];
		$this->party_english = $row[1];
		$this->party_french = $row[2];
		$this->parliament_type_id = $row[3];
		$this->parliament_id = $row[4];
		mysqli_free_result($result);

		$sqlquery = <<<QUERY
select count(*) RAN 
from contest c, districts d 
where c.district_id = d.district_id 
and d.parliament_id=$this->parliament_id
and c.candidate_party=$this->party_id 
QUERY;

		$result = mysqli_query($dbcnx, $sqlquery) or die(mysqli_error($dbcnx));
		if (!result) {
			exit('<p> Unable to execute Constructor RAN Query');
		}
		$row = mysqli_fetch_row($result);
		if ($row == FALSE) {
			exit();
			echo "Information not available for Constructor.<BR>";
		}
		$this->num_candidates = $row[0];
		mysqli_free_result($result);

		$sqlquery = <<<QUERY
select count(*) RAN 
from contest c, districts d 
where c.district_id = d.district_id 
and d.parliament_id=$this->parliament_id
and c.candidate_party= $this->party_id
and WON=TRUE
QUERY;

		$result = mysqli_query($dbcnx, $sqlquery) or die(mysqli_error($dbcnx));
		if (!result) {
			exit('<p> Unable to execute Constructor RAN Query');
		}
		$row = mysqli_fetch_row($result);
		if ($row == FALSE) {
			exit();
			echo "Information not available for Constructor.<BR>";
		}
		$this->num_seats = $row[0];
		mysqli_free_result($result);

		$this->stat_query = <<<STAT
select T.TOTAL, T.COUNTED, T.WASTED, T.WASTED/T.TOTAL*100 WASTED_PERCENT FROM
(select sum(S.TOTAL) TOTAL, sum(S.COUNTED) COUNTED, sum(S.WASTED) WASTED from
(select sum(c.votes) TOTAL, 0 COUNTED, 0 WASTED
from contest c, party p, districts d
where p.party_id = c.candidate_party
and c.district_id=d.district_id
and d.parliament_id = $this->parliament_id
and p.party_id=$this->party_id
UNION
select 0 TOTAL, sum(c.votes) COUNTED, 0 WASTED
from contest c, party p, districts d
where p.party_id = c.candidate_party
and c.district_id=d.district_id
and won=TRUE
and d.parliament_id = $this->parliament_id
and p.party_id=$this->party_id
UNION
select 0 TOTAL, 0 COUNTED, sum(c.votes) WASTED
from contest c, party p, districts d
where p.party_id = c.candidate_party
and c.district_id=d.district_id
and won=FALSE
and d.parliament_id = $this->parliament_id
and p.party_id=$this->party_id) AS S
) AS T
STAT;

		$this->district_query = <<<DISTRICT
select D.DISTRICT_UNIQUE_NAME, D.DISTRICT_ENGLISH_NAME, D.DISTRICT_FRENCH_NAME, D.DISTRICT_ID, 
C.CANDIDATE_NAME, C.VOTES, C.VOTE_PERCENT,
P.PROVINCE_ENGLISH_NAME, P.PROVINCE_FRENCH_NAME
FROM districts D, contest C, province P
where D.district_id = C.district_id
and D.province_id = P.province_id
and C.won = FALSE
and D.parliament_id = $this->parliament_id
and C.candidate_party = $this->party_id
ORDER BY VOTE_PERCENT DESC
DISTRICT;
	}
	//---------------
	// PUBLIC METHOD
	function ToString() {
		echo "part_id is $this->party_id<BR>\n";
		echo "party_english_name is $this->party_english_name<BR>\n";
		echo "party_french_name is $this->party_french_name<BR>\n";
		echo "parliament_type_id is $this->parliament_type_id<BR>\n";
		echo "parliament_id is $this->parliament_id<BR>\n";
	}
	function SplashInfo() {
$leader_query = <<<LEADQUERY
select LEADER_NAME
from party p, party_leader l
where p.PARTY_ID=$this->party_id
and l.PARLIAMENT_ID = $this->parliament_id
and l.PARTY_ID=p.PARTY_ID
and l.LEADER_NUMBER=1
LEADQUERY;

		$result = mysqli_query($dbcnx, $leader_query) or die(mysqli_error($dbcnx));
		if (!$result) {
			exit('<p> Unable to execute SplashInfo leader_query');
		}

		while ($row = mysqli_fetch_array($result)) {
			$party_leader= $row['LEADER_NAME'];
		}
		mysqli_free_result($result);

$picture_query = <<<PICQUERY
select PIC_NAME
from pic_resource 
where ENTITY='PARTY'
and ENTITY_ID='$this->party_unique'
and PIC_TYPE='LOGO'
PICQUERY;
		$result = mysqli_query($dbcnx, $picture_query) or die(mysqli_error($dbcnx));
		if (!$result){
			exit('<p> Unable to execute SplashInfo picture_query');
		}

		while ($row = mysqli_fetch_array($result)) {
			$logo= $row['PIC_NAME'];
		}
		mysqli_free_result($result);
		echo "<p>Party Leader (at time of election) $party_leader<p>";
		if ($logo != '') {
			echo DisplayPicture($logo);
		}
	}
	function GeneralInfo(){
		$result = mysqli_query($dbcnx, $this->stat_query) or die (mysqli_error($dbcnx));
		if (!result){
			exit('<p> Unable to execute GeneralInfo stat_query');
		}

		while ($row = mysqli_fetch_array($result)) {
			$total = $row['TOTAL'];
			$counted = $row['COUNTED'];
			$wasted = $row['WASTED'];
		}
		mysqli_free_result($result);
		if ($this->num_seats != 0) { 
			$votes_per_seat = $total/$this->num_seats; 
			$votes_per_seat = round($votes_per_seat,0);
		}
		$wasted_percent=PercentConversion($wasted/$total*100, 'ENGLISH');
		$total=number_format($total);
		$wasted=number_format($wasted);
		echo "<p>This party ran $this->num_candidates candidates and won $this->num_seats seats";
		echo "<p>$total Canadians voted for $this->party_english. $wasted of those votes were wasted. $wasted_percent.</p>";
		if ($this->num_seats != 0) {
			echo "<p>It took around $votes_per_seat votes for this party to elect one representative</p>";
		}
		$this->GenWorstDistricts();
	}
	function GenWorstDistricts(){
		$result = mysqli_query($dbcnx, $this->district_query) or die (mysqli_error($dbcnx));
		if (!result){
			exit('<p> Unable to execute GeneralInfo district_query');
		}
		echo "<p>Districts where the most voters for this party had their votes wasted</p>";
		echo "\n<TABLE BORDER=1><TR><TH>District Name</TH><TH>Candidate</TH><TH>Number of Votes</TH><TH>Percent of Support</TH></TR>";
		$loop_count = 0;
		while ($row = mysqli_fetch_array($result) AND $loop_count < $this->loop_max) {
			$district_english =$row['DISTRICT_ENGLISH_NAME'];
			$province_english =$row['PROVINCE_ENGLISH_NAME'];
			$district_french  =$row['DISTRICT_FRENCH_NAME'];
			$district_french  =$row['PROVINCE_FRENCH_NAME'];
			$candidate_name   =$row['CANDIDATE_NAME'];
			$candidate_votes  =$row['VOTES'];
			$candidate_percent=$row['VOTE_PERCENT'];
			$district_name = $district_english.', '.$province_english;
			$candidate_percent= PercentConversion($candidate_percent,'ROUND');
			echo "\n<TR><TD>$district_name</TD><TD>$candidate_name</TD><TD>$candidate_votes</TD><TD>$candidate_percent</TD></TR>";
			$loop_count++;
		}
		mysqli_free_result($result);
		echo "\n</TABLE>";
	} 
} // Class Party

class Party_FEDERAL extends Party{
	function Party_FEDERAL($in_name, $parliament_number) {
		parent::Party($in_name, $parliament_number);
	}
} //End Party_FEDERAL
?>

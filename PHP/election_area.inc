<?php
require_once ("login.inc");
require_once ("function.inc");
require_once ("table.inc");
require_once ("drupal_form.inc");
require_once ("sql_functions.inc");
require_once ("election_object.inc");
function EA_Factory($parliament_type, $parliament_number, $area_id, $area_type) {
	if ($area_type=='TOP') {
		$return_EA = new EA_Top($parliament_type,$parliament_number);
	}
	elseif ($area_type=='PROVINCE') {
		$return_EA = new EA_Province($parliament_type,$parliament_number, $area_id);
	}
	elseif($area_type=='DISTRICT') {
		$return_EA = new EA_District($parliament_type, $parliament_number, $area_id);
	}
	elseif($area_type=='REGION'){
		$return_EA = new EA_Region($parliament_type,$parliament_number, $area_id);
	}
	else {
		echo "Invalid area_type: $area_type";
	}
	return($return_EA);
}
class ElectionArea extends ElectionObject{
	var $stat_query;
	var $tvote_query;
	var $count_party_query;
	var $total_seats;
	var $total_votes;
	var $loop_count;
	var $party_english_array = array();
	var $party_english_abbr_array = array();
	var $party_french_array = array();
	var $party_seat_array = array();
	var $party_vote_array = array();
	var $election_id_array = array();
	var $election_number_array = array();
	var $election_year_array = array();
	var $in_govt;
	var $num_to_display;
	//--------------
	// CONSTRUCTOR
	function ElectionArea($in_type,$in_number) {
		parent::ElectionObject($in_type,$in_number);

		$this->stat_query = <<<STAT
select T.PARTY PARTY, T.PARTY_ABBR PARTY_ABBR, T.PARTY_COLOUR, PARTY_COLOUR, T.TOTAL TOTAL, T.COUNTED COUNTED, T.WASTED WASTED, T.WASTED/T.TOTAL*100 WASTED_PERCENT FROM
(select S.PARTY, S.PARTY_ABBR, S.PARTY_COLOUR, sum(S.TOTAL) TOTAL, sum(S.COUNTED) COUNTED, sum(S.WASTED) WASTED from
(select p.party_english_name PARTY, p.party_english_abbr PARTY_ABBR, p.party_colour PARTY_COLOUR, sum(c.votes) TOTAL, 0 COUNTED, 0 WASTED
from contest c, party p, districts d
where p.party_id = c.candidate_party 
and c.district_id = d.district_id CONDITION
group by p.party_english_name
UNION
select p.party_english_name PARTY, p.party_english_abbr PARTY_ABBR, p.party_colour PARTY_COLOUR, 0 TOTAL, sum(c.votes) COUNTED, 0 WASTED
from contest c, party p, districts d
where p.party_id = c.candidate_party
and c.district_id = d.district_id
and won=TRUE CONDITION
group by p.party_english_name
UNION
select p.party_english_name PARTY, p.party_english_abbr PARTY_ABBR, p.party_colour PARTY_COLOUR, 0 TOTAL, 0 COUNTED, sum(c.votes) WASTED
from contest c, party p, districts d
where p.party_id = c.candidate_party
and c.district_id = d.district_id
and won=FALSE CONDITION
group by p.party_english_name) AS S
group by S.PARTY) AS T ORDER BY TOTAL DESC
STAT;

		$this->tvote_query = <<<TOTAL
select sum(VOTES)
from contest c, districts d 
where c.DISTRICT_ID=d.DISTRICT_ID 
and d.PARLIAMENT_ID=$this->parliament_id CONDITION
TOTAL;
		$this->count_party_query = <<<COUNT
select count(*) 
from 	(select p.PARTY_ENGLISH_NAME name, sum(c.VOTES)/TOTALVOTE prop 
	 from contest c, districts d, party p 
	 where c.DISTRICT_ID = d.DISTRICT_ID 
	 and d.PARLIAMENT_ID=$this->parliament_id CONDITION 
	 and c.CANDIDATE_PARTY=p.PARTY_ID 
	 group by name having prop >= 0.03
	) as T;
COUNT;

		$this->num_to_display= $this->GetNumDisplay();
	}
	//---------------
	// PUBLIC METHOD
	function GenVoteSummary() {
		$this->result = mysqli_query($this->dbcnx, $this->stat_query) or die(mysqli_error($this->dbcnx));
		if (!$this->result) {
			exit('<p> Unable to execute EA Query');
		}
	}
	function GetVoteSummary() {
		$full_total = $full_counted = $full_wasted = $full_waste_percent = $loop_count = $other_waste_vote = 0;
		$party_waste_name = array();
		$party_waste_abbr = array();
		$party_waste_vote = array();
		$party_waste_percent = array();
		$this->GenVoteSummary();
		$num_to_display=$this->num_to_display;

		while ($row = mysqli_fetch_array($this->result)) {
			$party_name= $row['PARTY'];
			$party_abbr= $row['PARTY_ABBR'];
			$party_colour= $row['PARTY_COLOUR'];
			$total_vote= $row['TOTAL'];
			$counted_vote = $row['COUNTED'];
			$wasted_vote = $row['WASTED'];
			$waste_percent = $row['WASTED_PERCENT'];

			$full_total+=$total_vote;
			$full_counted+=$counted_vote;
			$full_wasted+=$wasted_vote;
			if ($loop_count < $num_to_display && $party_name !='Independent') {
				$party_waste_name[]=$party_name;
				$party_waste_abbr[]=$party_abbr;
				$party_waste_colour[]=$party_colour;
				$party_waste_vote[]=$wasted_vote;
				$party_waste_percent[]=$waste_percent;
				$loop_count++;
			}
			else {
				$loop_count++;
				$other_waste_vote+=$wasted_vote;
			}
		}

		$party_waste_name[]='Other';
		$party_waste_abbr[]='Other';
		$party_waste_colour[]='000000';
		$party_waste_vote[]=$other_waste_vote;

		$waste_percent= PercentConversion($full_wasted/$full_total*100, 'ENGLISH');
		$english_total = number_format($full_total);
		$english_waste = number_format($full_wasted);

		echo "\n<H3>Wasted Votes</H3>";
		$graph_html = "<IMG class='graph' id='total_waste' SRC='../PHP/ea_total_waste_graph.php?total=".$full_total."&waste=".$full_wasted."'></IMG>";
		printf($graph_html);
		echo "<p class='caption'>Voters cast $english_total votes. $english_waste of them were wasted</p>";
		$party_breakdown='';
		for ($count = 0; $count < count($party_waste_name); $count++){
			if ($party_waste_vote[$count] > 0) {
				$waste_percent= PercentConversion($party_waste_vote[$count]/$full_total*100, 'ENGLISH');
				$english_waste = number_format($party_waste_vote[$count]);
				$party_breakdown.="<p class='caption'>$english_waste votes for $party_waste_name[$count] wasted</p>";
			}
		}
		mysqli_free_result($this->result);

		$csv_vote = implode(",",$party_waste_vote);
		$csv_pname = implode(",",$party_waste_abbr);
		$csv_pcolour = implode(",",$party_waste_colour);
		$graph_html = "<IMG class='graph' id='waste' SRC='../PHP/ea_waste_graph.php?vote=".$csv_vote."&pname=".$csv_pname."&pcolour=".$csv_pcolour."'></IMG>";
		printf($graph_html);
		printf($party_breakdown);

		if ($this->province_id == '') {
			$province_waste = "ALL";
		}
		else {
			$province_waste = $this->province_id;
		}

		if ($this->region_id == '') {
			$region_waste = "ALL";
		}
		else {
			$region_waste = $this->region_id;
		}

		$array_waste_table = CreateTopWasteTable($this->dbcnx, "$this->parliament_id",$province_waste,$region_waste);
		$topFiveTable = $array_waste_table[0];
		$overFiftyTable = $array_waste_table[1];
		$totalOverFifty = $array_waste_table[2];
		$overFiftyPercent = PercentConversion($totalOverFifty/$this->total_seats*100, "ENGLISH");
		if ($totalOverFifty > 0) {
			echo "\n<H3>Most Wasted Votes</H3>";
			echo "<p>Most of the time in our current system more than half of the votes cast in a district are wasted. The following table shows the worst of these.</p>\n";
			echo "\n<p class='caption label'> <a href='javascript:showFive();'>Top Five Most Wasted Votes</a> | <a href='javascript:showFifty();'>All Over Half Wasted</a></p>";
			echo $topFiveTable;
			echo $overFiftyTable;
			echo "<p class='caption'><em>More than half of the votes cast in $totalOverFifty districts were wasted. $overFiftyPercent of all districts</em><p>";
		}

	} //End GetVoteSummary
	function PrintQuery() {
		echo "<BR> $this->stat_query";
	}
	function StatsExecute($govstat_query) {
		$this->total_seats = 0;
		$this->total_votes = 0;
		$this->loop_count = 0;
		$this->in_govt = 0;
		$result = mysqli_query($this->dbcnx, $govstat_query) or die(mysqli_error($this->dbcnx));
		if (!$result) {
			exit('<p> Unable to execute TOP Statsinfo Query');
		}
		while ($row = mysqli_fetch_array($result)) {
			$party_english= $row['PARTY_ENGLISH_NAME'];
			$party_french= $row['PARTY_FRENCH_NAME'];
			$party_english_abbr = $row['PARTY_ENGLISH_ABBR'];

			$number_seats= $row['WON'];
			$number_votes= $row['VOTES'];
			$this->party_english_array[] = $party_english;
			$this->party_english_abbr_array[] = $party_english_abbr;
			$this->party_french_array[] = $party_french;
			$this->party_seat_array[] = $number_seats;
			$this->party_vote_array[] = $number_votes;
			$this->loop_count++;
			$this->total_seats+=$number_seats;
			if ($number_seats > 0) { $this->in_govt=$this->loop_count+1; }
			$this->total_votes+=$number_votes;
		}
		mysqli_free_result($result);
	}
	function DisplayStats() {
		echo "\n<H3>Seats won Vs. Popular support</H3>";
		#echo "\n<TABLE BORDER=1><TR><TH>Party</TH><TH>Number of seats</TH><TH>Popular Support</TH></TR>";
		$statsTable = new Table("statsTable", array("Party","Number of seats","Popular Support"));
		$statsTableFull = new Table("statsTableFull", array("Party","Number of seats","Popular Support"));
		$statsTableFull->Hide();
		
		$num_to_display = $this->num_to_display;


		$other_support = 0;
		$party_displayed = 0;
		for ($i = 0 ; $i < count($this->party_english_array); $i++) {
			$seat_percentage = $this->party_seat_array[$i]/$this->total_seats * 100;
			$popular_support = $this->party_vote_array[$i]/$this->total_votes * 100;
			$party_english = $this->party_english_array[$i];
			$party_french = $this->party_french_array[$i];
			$party_seats = $this->party_seat_array[$i];

			#$ind_support = $this->party_vote_array[$i];

			$popular_support_disp = PercentConversion($popular_support,'ROUND');
			$seat_percentage_disp = PercentConversion($seat_percentage,'ROUND');
			if ($party_english == 'Independent' || $party_english == 'No Affiliation') { $popular_support_disp= 'N/A'; } 

			if ($party_displayed < $num_to_display){
				#echo "\n<TR><TD>$party_english</TD><TD>$party_seats ($seat_percentage)</TD><TD>$popular_support</TD></TR>";
				//Set up Graph
				$english_array[$i] = $this->party_english_array[$i];
				$english_abbr_array[$i] = $this->party_english_abbr_array[$i];
				$french_array[$i] = $this->party_french_array[$i];
				$support_array[$i] = round($popular_support,2);
				$seat_percent_array[$i] = round($seat_percentage,2);
				$statsTable->Add(array($party_english,"$party_seats|$party_seats ($seat_percentage_disp)","$popular_support|$popular_support_disp"));
				$statsTableFull->Add(array($party_english,"$party_seats|$party_seats ($seat_percentage_disp)","$popular_support|$popular_support_disp"));
			}
			else {
				$statsTableFull->Add(array($party_english,"$party_seats|$party_seats ($seat_percentage_disp)","$popular_support|$popular_support_disp"));
				$other_support+=$this->party_vote_array[$i];
			}
			if ($party_english != 'Independent') { 
				$party_displayed++;
			}
		}
		$other_support = $other_support/$this->total_votes * 100;
		$print_other_support = PercentConversion($other_support,'ROUND');
		
		#echo"\n<TR><TD>Other</TD><TD>0 (0%)</TD><TD>$print_other_support</TD></TR>";

		//Adding Other row if required to graph and table
		if ($other_support > 0) { 
			$statsTable->Add(array("Other", "0|0 (0%)","$other_support|$print_other_support")); 
			$last=count($support_array);
			$support_array[$last] = $other_support;
			$seat_percent_array[$last] = 0;
			$english_array[$last] = 'Other';
			$english_abbr_array[$last] = 'Other';
		}
		echo "\n<p class='caption label'> <a href='javascript:showNormal();'>Summary Results</a> | <a href='javascript:showFull();'>Full Results</a></p>";
		echo $statsTable->Create();
		echo $statsTableFull->Create();
		
		echo "\n<p>Total number of seats: $this->total_seats </p>";


		$csv_support = implode(",",$support_array);
		$csv_seat = implode(",",$seat_percent_array);
		$csv_pname = implode(",",$english_abbr_array);
		$graph_html = "<IMG class='graph' id='discrepancy' SRC='../PHP/ea_discrepancy_graph.php?support=".$csv_support."&seat=".$csv_seat."&pname=".$csv_pname."'></IMG>";

		printf($graph_html);
	} //end DisplayStats
	function StatsInfo() {}
	function CreateDropDown(){
	}
	function CreateDistrictDropDown(){
		if ($this->parliament_type=='Federal') {
			$province_name = $this->province_english_name;
		} else { 
			$province_name = $this->parliament_type;
		}
		$district_drop_query = <<<DISTRICTDROP
select DISTRICT_ID, DISTRICT_UNIQUE_NAME, DISTRICT_FRENCH_NAME, DISTRICT_ENGLISH_NAME 
FROM districts  
where parliament_id = $this->parliament_id
and province_id = (select PROVINCE_ID from province where PROVINCE_ENGLISH_NAME='$province_name')
ORDER BY DISTRICT_ENGLISH_NAME
DISTRICTDROP;
		$result = mysqli_query($this->dbcnx, $district_drop_query) or die(mysqli_error($this->dbcnx));
		if (!$result) {
			exit('<p> Unable to execute CDDD district_drop_query');
		}
		$election_array = array();
		while ($row = mysqli_fetch_array($result)) {
			$form_key = $this->parliament_type."/".$this->parliament_number."/".$row['DISTRICT_ID'];
			$form_value = $row['DISTRICT_ENGLISH_NAME'];
			$election_array[$form_key]= $form_value;
		}
		$form = drupal_get_form('create_drupal_district_form',$election_array);
		echo "<div id='districtdropbox'>$form</div>";
	}//end CreateDistrictDropDown
	function CreateRegionDropDown($in_level, $in_parent){
		if ($this->parliament_type=='Federal') {
			$province_name = $this->province_english_name;
		} else { 
			$province_name = $this->parliament_type;
		}
		$region_drop_query = <<<REGIONDROP
select REGION_ID, REGION_UNIQUE_NAME, REGION_FRENCH_NAME, REGION_ENGLISH_NAME 
FROM region 
where region_id in (SELECT region_id from region_parliament where parliament_id = $this->parliament_id)
and province_id = (select PROVINCE_ID from province where PROVINCE_ENGLISH_NAME='$province_name')
and region_level = $in_level
and parent_region_id = $in_parent
ORDER BY REGION_ENGLISH_NAME
REGIONDROP;
		$result = mysqli_query($this->dbcnx, $region_drop_query) or die(mysqli_error($this->dbcnx));
		if (!$result) {
			exit('<p> Unable to execute RDQ region_drop_query');
		}
		$election_array = array();
		$region_count = 0;
		while ($row = mysqli_fetch_array($result)) {
			$form_key = $this->parliament_type."/".$this->parliament_number."/".$row['REGION_ID'];
			$form_value = $row['REGION_ENGLISH_NAME'];
			$election_array[$form_key]= $form_value;
			$region_count++;
		}
		if ($region_count > 0) {
			$form = drupal_get_form('create_drupal_region_form',$election_array,$in_level);
			echo "<div id='regiondropbox'>$form</div>";
		}
	}//end CreateRegionDropDown
	function CreateProvinceDropDown(){
	}//end CreateProvinceDropDown
	function HasRegion(){
$region_count_query = <<<REGCOUNT
select COUNT(*)
from region r, region_parliament rp
where rp.PARLIAMENT_ID=$this->parliament_id CONDITION
REGCOUNT;
		if ($this->parliament_type == 'Federal') {
			$region_count_query = str_replace("CONDITION","and r.PROVINCE_ID = $this->province_id",$region_count_query);
		}
		else {
			$region_count_query = str_replace ("CONDITION", "",$region_count_query);
		}
		$result = mysqli_query($this->dbcnx, $region_count_query) or die(mysqli_error($this->dbcnx)); 
		if (!$result) {
			exit('<p> Unable to execute RegionCount query');
		}
		$row = mysqli_fetch_array($result);
		if ($row[0] > 0){
			$toReturn = TRUE;
		}
		else {
			$toReturn = FALSE;
		}
		return $toReturn;
	} //End HasRegion
	function GetRegionList(){}
} // End class ElectionArea

class EA_Top extends ElectionArea{
	function EA_Top($in_type,$in_number) {
		parent::ElectionArea($in_type,$in_number);
		$this->stat_query = str_replace("CONDITION","and d.parliament_id = $this->parliament_id",$this->stat_query);
	}
	function StatsInfo() {
		$govstat_query = <<<GOVSQUERY
select PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME, PARTY_ENGLISH_ABBR, MAX(WON) WON, MAX(VOTES) VOTES FROM
(select count(*) WON, 0 VOTES, PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME, PARTY_ENGLISH_ABBR
from contest C, party P, districts D
where C.CANDIDATE_PARTY=P.PARTY_ID 
and C.DISTRICT_ID = D.DISTRICT_ID
and D.PARLIAMENT_ID=$this->parliament_id
and WON=TRUE GROUP by candidate_party 
UNION 
SELECT 0 WON, SUM(VOTES) VOTES, PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME, PARTY_ENGLISH_ABBR
from contest C, party P, districts D
where C.CANDIDATE_PARTY=P.PARTY_ID 
and C.DISTRICT_ID = D.DISTRICT_ID
and D.PARLIAMENT_ID=$this->parliament_id
GROUP by candidate_party) I
GROUP BY PARTY_ENGLISH_NAME
order by WON DESC, VOTES DESC;
GOVSQUERY;
		$this->StatsExecute($govstat_query);
		$this->DisplayStats();
	
	}
	function GetNumDisplay(){
		$this->tvote_query = str_replace("CONDITION",'',$this->tvote_query);
		$this->count_party_query = str_replace("CONDITION",'',$this->count_party_query);
		$result = mysqli_query($this->dbcnx, $this->tvote_query) or die(mysqli_error($this->dbcnx));
		if (!result) {
			exit('<p> Unable to execute tvote Query</p>');
		}
		$row = mysqli_fetch_row($result);

		if ($row == FALSE) {
			echo "tvote Query empty.<BR>";
			exit();
		}

		$tvote = $row[0];
		$this->count_party_query = str_replace("TOTALVOTE",$tvote,$this->count_party_query);

		$result = mysqli_query($this->dbcnx, $this->count_party_query) or die(mysqli_error($this->dbcnx));
		if (!result) {
			exit('<p> Unable to execute count Query</p>');
		}
		$row = mysqli_fetch_row($result);

		if ($row == FALSE) {
			echo "tvote Query empty.<BR>";
			exit();
		}
		
		return $row[0];
	}
	function CreateDropDown(){
		if ($this->HasMultipleElections()){
			$this->CreateElectionDropDown();
		}
		if ($this->parliament_type=='Federal'){
			$this->CreateProvinceDropDown();
		} else {
			if ($this->HasRegion()){
				$this->CreateRegionDropDown(1,0);
			}
			$this->CreateDistrictDropDown();
		}
	}

	function CreateProvinceDropDown(){
		$prov_drop_query = <<<PROVDROP
select PROVINCE_ID, PROVINCE_ENGLISH_NAME, PROVINCE_FRENCH_NAME 
from province
ORDER BY PROVINCE_ENGLISH_NAME;
PROVDROP;
		
		$result = mysqli_query($this->dbcnx, $prov_drop_query) or die(mysqli_error($this->dbcnx));
		if (!$result) {
			exit('<p> Unable to execute ProvinceDropDown query');
		}
		$election_array = array();
		while ($row = mysqli_fetch_array($result)) {
			$form_key = $this->parliament_type."/".$this->parliament_number."/".$row['PROVINCE_ID'];
			$form_value = $row['PROVINCE_ENGLISH_NAME'];
			$election_array[$form_key]= $form_value;
		}
		$form = drupal_get_form('create_drupal_province_form',$election_array);
		echo "<div id='provincedropbox'>$form</div>";
	}

} //End Class EA_Top

class EA_Province extends ElectionArea{
	var $province_id;
	var $province_english_name;
	var $province_french_name;
	function EA_Province($in_type,$in_number,$in_province) {
		$this->province_id = $in_province;
		parent::ElectionArea($in_type,$in_number);
		$this->stat_query = str_replace("CONDITION","and d.parliament_id = $this->parliament_id and c.province_id = $in_province",$this->stat_query);

$provinfo_query = <<<PROVINFO
select PROVINCE_ENGLISH_NAME, PROVINCE_FRENCH_NAME
from province
where province_id=$this->province_id
PROVINFO;
		$result = mysqli_query($this->dbcnx, $provinfo_query) or die(mysqli_error($this->dbcnx));
		while ($row = mysqli_fetch_array($result)) {
			$this->province_english_name= $row['PROVINCE_ENGLISH_NAME'];
			$this->province_french_name= $row['PROVINCE_FRENCH_NAME'];
		}
	}

	function SplashInfo() {
$picture_query = <<<PICQUERY
select PIC_NAME
from pic_resource 
where ENTITY='REGION'
and ENTITY_ID='$this->province_english_name'
and PIC_TYPE='FLAG'
PICQUERY;
		$result = mysqli_query($this->dbcnx, $picture_query) or die(mysqli_error($this->dbcnx));
		while ($row = mysqli_fetch_array($result)) {
			$flag_pic= $row['PIC_NAME'];
		}
		if (!$result){
			exit('<p> Unable to execute province SplashInfo picture_query');
		}

		while ($row = mysqli_fetch_array($result)) {
			$flag_pic= $row['PIC_NAME'];
		}
		mysqli_free_result($result);
		echo "<IMG class='flag' id='province-flag' SRC='../IMAGES/$flag_pic'></IMG>";
		echo "<p>$this->province_english_name</p>";
	}

	function StatsInfo() {
		$govstat_query = <<<GOVSQUERY
select PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME, PARTY_ENGLISH_ABBR, MAX(WON) WON, MAX(VOTES) VOTES FROM
(select count(*) WON, 0 VOTES, PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME, PARTY_ENGLISH_ABBR
from contest C, party P, districts D 
where C.CANDIDATE_PARTY=P.PARTY_ID 
and D.DISTRICT_ID = C.DISTRICT_ID
and D.PROVINCE_ID=$this->province_id
and D.PARLIAMENT_ID=$this->parliament_id
and WON=TRUE GROUP by candidate_party 
UNION 
SELECT 0 WON, SUM(VOTES) VOTES, PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME, PARTY_ENGLISH_ABBR
from contest C, party P, districts D
where C.CANDIDATE_PARTY=P.PARTY_ID 
and D.DISTRICT_ID = C.DISTRICT_ID
and D.PROVINCE_ID=$this->province_id
and D.PARLIAMENT_ID=$this->parliament_id
GROUP by candidate_party) I
GROUP BY PARTY_ENGLISH_NAME
order by WON DESC, VOTES DESC;
GOVSQUERY;
		$this->StatsExecute($govstat_query);
		$this->DisplayStats();
	}
	function GetProvinceEnglishName(){
		return $this->province_english_name;
	}
	function GetProvinceId(){
		return $this->province_id;
	}
	function CreateDropDown(){
		#$this->CreateElectionDropDown();
		if ($this->HasRegion()){
			$this->CreateRegionDropDown(1,0);
		}
		$this->CreateDistrictDropDown();
	}
	function GetNumDisplay(){
		$this->tvote_query = str_replace("CONDITION","and d.PROVINCE_ID = $this->province_id",$this->tvote_query);
		$this->count_party_query = str_replace("CONDITION","and d.PROVINCE_ID =$this->province_id",$this->count_party_query);
		$result = mysqli_query($this->dbcnx, $this->tvote_query) or die(mysqli_error($this->dbcnx));
		if (!result) {
			exit('<p> Unable to execute tvote Query</p>');
		}
		$row = mysqli_fetch_row($result);

		if ($row == FALSE) {
			echo "tvote Query empty.<BR>";
			exit();
		}

		$tvote = $row[0];
		$this->count_party_query = str_replace("TOTALVOTE",$tvote,$this->count_party_query);

		$result = mysqli_query($this->dbcnx, $this->count_party_query) or die(mysqli_error($this->dbcnx));
		if (!result) {
			exit('<p> Unable to execute count Query</p>');
		}
		$row = mysqli_fetch_row($result);

		if ($row == FALSE) {
			echo "tvote Query empty.<BR>";
			exit();
		}
		mysqli_free_result($result);
	
		return $row[0];
	}
} //End Class EA_Province

class EA_Region extends ElectionArea{
	var $region_id;
	var $parent_region_id;
	var $region_level;
	var $region_english_name;
	var $region_french_name;
	var $province_english_name;
	var $province_french_name;
	function EA_Region($in_type,$in_number,$in_region) {
		$this->region_id = $in_region;
		parent::ElectionArea($in_type,$in_number);
		$this->stat_query = str_replace("CONDITION","and d.parliament_id = $this->parliament_id and c.district_id in (select district_id from region_districts where region_id=$this->region_id) ",$this->stat_query);

$regioninfo_query = <<<REGIONINFO
select REGION_ENGLISH_NAME, REGION_FRENCH_NAME, PROVINCE_ENGLISH_NAME, PROVINCE_FRENCH_NAME, PARENT_REGION_ID,REGION_LEVEL
from region r, province p
where region_id=$this->region_id
and r.province_id = p.province_id
REGIONINFO;
		$result = mysqli_query($this->dbcnx, $regioninfo_query) or die(mysqli_error($this->dbcnx));
		while ($row = mysqli_fetch_array($result)) {
			$this->region_english_name= $row['REGION_ENGLISH_NAME'];
			$this->region_french_name= $row['REGION_FRENCH_NAME'];
			$this->province_english_name= $row['PROVINCE_ENGLISH_NAME'];
			$this->province_french_name= $row['PROVINCE_FRENCH_NAME'];
			$this->parent_region_id=$row['PARENT_REGION_ID'];
			$this->region_level=$row['REGION_LEVEL'];
		}
	}
	function GetRegionList(){
$exampledistrict_query = <<<EDISTRICT
select DISTRICT_ID
from region_districts 
where region_id=$this->region_id LIMIT 1;
EDISTRICT;
		$result = mysqli_query($this->dbcnx, $exampledistrict_query) or die(mysqli_error($this->dbcnx));
		while ($row = mysqli_fetch_array($result)) {
			$example_district=$row['DISTRICT_ID'];
		}
$fullregionlist_query = <<<FRLIST
select REGION_ID, REGION_ENGLISH_NAME, REGION_FRENCH_NAME, REGION_LEVEL
from region 
where REGION_ID in (select REGION_ID from region_districts where DISTRICT_ID=$example_district)
order by REGION_LEVEL
FRLIST;
		$result = mysqli_query($this->dbcnx, $fullregionlist_query) or die(mysqli_error($this->dbcnx));
		while ($row = mysqli_fetch_array($result)) {
			$list_regionid=$row['REGION_ID'];
			$list_regionename=$row['REGION_ENGLISH_NAME'];
			$list_regionfname=$row['REGION_FRENCH_NAME'];
			$list_regionlevel=$row['REGION_LEVEL'];
			if ($list_regionlevel <= $this->region_level){
				$toReturn[]=array($list_regionename,$list_regionid);
			}

		}
		return $toReturn;
	}//End GetRegionList
	function CreateDistrictDropDown(){
		$district_drop_query = <<<DISTRICTDROP
select DISTRICT_ID, DISTRICT_UNIQUE_NAME, DISTRICT_FRENCH_NAME, DISTRICT_ENGLISH_NAME 
FROM districts  
where parliament_id = $this->parliament_id
and district_id IN (select DISTRICT_ID from region_districts where REGION_ID=$this->region_id)
ORDER BY DISTRICT_ENGLISH_NAME
DISTRICTDROP;
		$result = mysqli_query($this->dbcnx, $district_drop_query) or die(mysqli_error($this->dbcnx));
		if (!$result) {
			exit('<p> Unable to execute CDDD district_drop_query');
		}
		$election_array = array();
		while ($row = mysqli_fetch_array($result)) {
			$form_key = $this->parliament_type."/".$this->parliament_number."/".$row['DISTRICT_ID'];
			$form_value = $row['DISTRICT_ENGLISH_NAME'];
			$election_array[$form_key]= $form_value;
		}
		$form = drupal_get_form('create_drupal_district_form',$election_array);
		echo "<div id='districtdropbox'>$form</div>";
	} //End CreateDistrictDropDown EA_Region

	function SplashInfo() {
		echo "<p>$this->region_english_name - $this->province_english_name</p>";
	}

	function StatsInfo() {
		$govstat_query = <<<GOVSQUERY
select PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME, PARTY_ENGLISH_ABBR, MAX(WON) WON, MAX(VOTES) VOTES FROM
(select count(*) WON, 0 VOTES, PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME, PARTY_ENGLISH_ABBR
from contest C, party P, districts D 
where C.CANDIDATE_PARTY=P.PARTY_ID 
and D.DISTRICT_ID = C.DISTRICT_ID
and D.DISTRICT_ID in (select DISTRICT_ID from region_districts where REGION_ID=$this->region_id)
and D.PARLIAMENT_ID=$this->parliament_id
and WON=TRUE GROUP by candidate_party 
UNION 
SELECT 0 WON, SUM(VOTES) VOTES, PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME, PARTY_ENGLISH_ABBR
from contest C, party P, districts D
where C.CANDIDATE_PARTY=P.PARTY_ID 
and D.DISTRICT_ID = C.DISTRICT_ID
and D.DISTRICT_ID in (select DISTRICT_ID from region_districts where REGION_ID=$this->region_id)
and D.PARLIAMENT_ID=$this->parliament_id
GROUP by candidate_party) I
GROUP BY PARTY_ENGLISH_NAME
order by WON DESC, VOTES DESC;
GOVSQUERY;
		$this->StatsExecute($govstat_query);
		$this->DisplayStats();
	}
	function GetRegionEnglishName(){
		return $this->region_english_name;
	}
	function GetRegionId(){
		return $this->region_id;
	}
	function CreateDropDown(){
		$this->CreateRegionDropDown($this->region_level+1, $this->region_id);
		$this->CreateDistrictDropDown();
	}
	function GetNumDisplay(){
		$this->tvote_query = str_replace("CONDITION","and d.DISTRICT_ID IN (select district_id from region_districts where region_id=$this->region_id)",$this->tvote_query);
		$this->count_party_query = str_replace("CONDITION","and d.DISTRICT_ID IN (select district_id from region_districts where region_id=$this->region_id)",$this->count_party_query);
		$result = mysqli_query($this->dbcnx, $this->tvote_query) or die(mysqli_error($this->dbcnx));
		if (!result) {
			exit('<p> Unable to execute tvote Query</p>');
		}
		$row = mysqli_fetch_row($result);

		if ($row == FALSE) {
			echo "tvote Query empty.<BR>";
			exit();
		}

		$tvote = $row[0];
		$this->count_party_query = str_replace("TOTALVOTE",$tvote,$this->count_party_query);

		$result = mysqli_query($this->dbcnx, $this->count_party_query) or die(mysqli_error($this->dbcnx));
		if (!result) {
			exit('<p> Unable to execute count Query</p>');
		}
		$row = mysqli_fetch_row($result);

		if ($row == FALSE) {
			echo "tvote Query empty.<BR>";
			exit();
		}
		mysqli_free_result($result);
	
		return $row[0];
	}
} //End Class EA_Region

class EA_District extends ElectionArea{
	var $district_id;
	var $district_unique_name;
	var $district_french_name;
	var $district_english_name;
	var $province_id;
	var $province_english_name;
	var $province_french_name;
	var $candidate_name_array = array();
	function EA_District($in_type,$in_number,$in_district) {
		$this->district_id = $in_district;
		parent::ElectionArea($in_type,$in_number);
		$this->stat_query = str_replace("CONDITION","and d.parliament_id = $this->parliament_id and c.district_id = $in_district",$this->stat_query);

		$district_infoquery= <<<DISTRICT
select DISTRICT_UNIQUE_NAME, DISTRICT_FRENCH_NAME, DISTRICT_ENGLISH_NAME,
	PROVINCE_ENGLISH_NAME, PROVINCE_FRENCH_NAME, P.PROVINCE_ID
from districts D, province P 
where D.district_id=$this->district_id 
and P.province_id=D.province_id;
DISTRICT;
		$result = mysqli_query($this->dbcnx, $district_infoquery) or die(mysqli_error($this->dbcnx));
		if (!$result){
			exit('<p> Unable to execute District SplashInfo district_infoquery');
		}
		$row = mysqli_fetch_array($result);
		$this->district_unique_name= $row['DISTRICT_UNIQUE_NAME'];
		$this->district_french_name= $row['DISTRICT_FRENCH_NAME'];
		$this->district_english_name= $row['DISTRICT_ENGLISH_NAME'];
		$this->province_english_name= $row['PROVINCE_ENGLISH_NAME'];
		$this->province_french_name= $row['PROVINCE_FRENCH_NAME'];
		$this->province_id = $row['PROVINCE_ID'];
		$this->num_to_display = $this->GetNumDisplay();
		mysqli_free_result($result);
	}
	function GetNumDisplay(){
		$this->tvote_query = str_replace("CONDITION","and d.DISTRICT_ID=$this->district_id",$this->tvote_query);

		$this->count_party_query = str_replace("CONDITION","and d.DISTRICT_ID=$this->district_id",$this->count_party_query);
		$result = mysqli_query($this->dbcnx, $this->tvote_query) or die(mysqli_error($this->dbcnx));
		if (!result) {
			exit('<p> Unable to execute tvote Query</p>');
		}
		$row = mysqli_fetch_row($result);

		if ($row == FALSE) {
			echo "tvote Query empty.<BR>";
			exit();
		}

		$tvote = $row[0];
		mysqli_free_result($result);
		$this->count_party_query = str_replace("TOTALVOTE",$tvote,$this->count_party_query);

		$result = mysqli_query($this->dbcnx, $this->count_party_query) or die(mysqli_error($this->dbcnx));
		if (!result) {
			exit('<p> Unable to execute count Query</p>');
		}
		$row = mysqli_fetch_row($result);

		if ($row == FALSE) {
			echo "tvote Query empty.<BR>";
			exit();
		}
		mysqli_free_result($result);
	
		return $row[0];
	}

	function GetVoteSummary() {
		$full_total = $full_counted = $full_wasted = $full_waste_percent = $loop_count = 0;
		$party_waste_name = array();
		$party_waste_vote = array();
		$party_waste_percent = array();
		$other_vote = 0;
		$this->GenVoteSummary();
		while ($row = mysqli_fetch_array($this->result)) {
			$party_name= $row['PARTY'];
			$party_abbr= $row['PARTY_ABBR'];
			$party_colour=$row['PARTY_COLOUR'];

			$total_vote= $row['TOTAL'];
			$counted_vote = $row['COUNTED'];
			$wasted_vote = $row['WASTED'];
			$waste_percent = $row['WASTED_PERCENT'];

			$full_total+=$total_vote;
			$full_counted+=$counted_vote;
			$full_wasted+=$wasted_vote;
			if ($loop_count < $this->num_to_display) {
				$party_waste_name[]=$party_name;
				$party_waste_abbr[]=$party_abbr;
				$party_waste_colour[]=$party_colour;
				$party_waste_vote[]=$wasted_vote;
				$party_waste_percent[]=$waste_percent;
			}
			else {
				$other_vote+=$wasted_vote;
			}
			$loop_count++;
		}

		$party_waste_name[]='Other';
		$party_waste_abbr[]='Other';
		$party_waste_vote[]=$other_vote;
		$party_waste_colour[]='000000';

		$english_total= number_format($full_total);
		$english_waste= number_format($full_wasted);
		$graph_html = "<IMG class='graph' id='total_waste' SRC='../PHP/ea_total_waste_graph.php?total=".$full_total."&waste=".$full_wasted."'></IMG>";
		printf($graph_html);
		echo "<p class='caption'>Voters cast $english_total valid votes.</p>";
		$csv_vote = implode(",",$party_waste_vote);
		$csv_pname = implode(",",$party_waste_abbr);
		$csv_pcolour = implode(",",$party_waste_colour);
		$graph_html = "<IMG class='graph' id='waste' SRC='../PHP/ea_waste_graph.php?vote=".$csv_vote."&pname=".$csv_pname."&pcolour=".$csv_pcolour."'></IMG>";
		printf($graph_html);
		for ($count = 0; $count < sizeof($party_waste_name); $count++){
			if ($count != 0 && $party_waste_vote[$count]!=0) {
				$english_waste = number_format($party_waste_vote[$count]);
				echo "<p class='caption'>$english_waste votes for $party_waste_name[$count] wasted</p>";
			}
		}
		mysqli_free_result($this->result);
	}
	function GetDistrictEnglishName() {
		return $this->district_english_name;
	}
	function GetProvinceEnglishName(){
		return $this->province_english_name;
	}
	function GetProvinceId(){
		return $this->province_id;
	}
	function SplashInfo() {
		echo "<p>$this->district_english_name - $this->province_english_name</p>";
$picture_query = <<<PICQUERY
select PIC_NAME
from pic_resource 
where ENTITY='DISTRICT'
and ENTITY_ID='$district_unique_name'
and PIC_TYPE='MAP'
PICQUERY;
		$result = mysqli_query($this->dbcnx, $picture_query) or die(mysqli_error($this->dbcnx));
		$map_pic='';
		while ($row = mysqli_fetch_array($result)) {
			$map_pic= $row['PIC_NAME'];
		}
		if (!$result){
			exit('<p> Unable to execute province SplashInfo picture_query');
		}
		mysqli_free_result($result);
		if ($map_pic != '') {
			echo "<IMG class='map' id='district-map' SRC='../IMAGES/$map_pic'></IMG>";
		}
		#echo "<p>Member of Parliament.</p>";
	}
	function StatsInfo() {
		$govstat_query = <<<GOVSQUERY
SELECT VOTES, CANDIDATE_NAME, PARTY_ENGLISH_NAME, PARTY_FRENCH_NAME
from contest C, party P, districts D
where C.CANDIDATE_PARTY=P.PARTY_ID 
and C.DISTRICT_ID = D.DISTRICT_ID
and C.DISTRICT_ID=$this->district_id
and D.PARLIAMENT_ID=$this->parliament_id
ORDER BY WON DESC, VOTES DESC;
GOVSQUERY;
		$this->StatsExecute($govstat_query);
		$this->DisplayStats();
	}
	function StatsExecute($govstat_query) {
		$this->total_votes = 0;
		$this->loop_count = 0;
		$result = mysqli_query($this->dbcnx, $govstat_query) or die(mysqli_error($this->dbcnx));
		if (!$result) {
			exit('<p> Unable to execute TOP Statsinfo Query');
		}
		while ($row = mysqli_fetch_array($result)) {
			$party_english= $row['PARTY_ENGLISH_NAME'];
			$party_french= $row['PARTY_FRENCH_NAME'];
			$candidate_name = $row['CANDIDATE_NAME'];
			$number_votes= $row['VOTES'];
			$this->party_english_array[] = $party_english;
			$this->party_french_array[] = $party_french;
			$this->candidate_name_array[] = $candidate_name;
			$this->party_vote_array[] = $number_votes;
			$this->loop_count++;
			$this->total_votes+=$number_votes;
		}
		mysqli_free_result($result);
	}
	function DisplayStats() {
		#echo "\n<TABLE BORDER=1><TR><TH>Candidate</TH><TH>Number of Votes</TH><TH>Elected</TH></TR>";

		$statsTable = new Table("statsTable", array("Candidate","Number of votes","Elected"));
		for ($i = 0 ; $i < $this->loop_count; $i++) {
			$number_votes = $this->party_vote_array[$i];
			$party_english = $this->party_english_array[$i];
			$party_french = $this->party_french_array[$i];
			$candidate_name = $this->candidate_name_array[$i];
			if ($i==0) { $elected='X'; } else {$elected =' ';}
			#echo "\n<TR><TD>$candidate_name ($party_english)</TD><TD>$number_votes</TD><TD>$elected</TR>";
			$statsTable->Add(array("$candidate_name ($party_english)",$number_votes,$elected));
		}
		#echo "\n</TABLE>";
		echo $statsTable->Create();
	}
	function CreateDropDown(){
		#$this->CreateElectionDropDown();
	}
	function GetRegionList(){
$fullregionlist_query = <<<FRLIST
select REGION_ID, REGION_ENGLISH_NAME, REGION_FRENCH_NAME, REGION_LEVEL
from region 
where REGION_ID in (select REGION_ID from region_districts where DISTRICT_ID=$this->district_id)
order by REGION_LEVEL
FRLIST;
		$result = mysqli_query($this->dbcnx, $fullregionlist_query) or die(mysqli_error($this->dbcnx));
		while ($row = mysqli_fetch_array($result)) {
			$list_regionid=$row['REGION_ID'];
			$list_regionename=$row['REGION_ENGLISH_NAME'];
			$list_regionfname=$row['REGION_FRENCH_NAME'];
			$list_regionlevel=$row['REGION_LEVEL'];
			$toReturn[]=array($list_regionename,$list_regionid);
		}
		return $toReturn;
	}//End GetRegionList
} //End Class EA_District
?>

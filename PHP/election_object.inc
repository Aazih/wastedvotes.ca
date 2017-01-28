<?php
require_once ("login.inc");
require_once ("function.inc");
require_once ("drupal_form.inc");
require_once ("sql_functions.inc");
class ElectionObject {
	var $parliament_type;
	var $parliament_type_id;
	var $parliament_number;
	var $parliament_id;
	var $dbcnx;
	var $election_drop_query;
	var $election_year;
	//--------------
	// CONSTRUCTOR
	function ElectionObject($in_type,$in_number) {
		$this->parliament_type = $in_type;
		$this->parliament_number = $in_number;
		$this->dbcnx = LoginProd();

		$this->election_drop_query = <<<ELECTION
select PARLIAMENT_ID, PARLIAMENT_NUMBER, PARLIAMENT_TYPE_ID, YEAR(ELECTION_DATE) ELECTION_YEAR
from parliament 
where parliament_type_id=	(select PARLIAMENT_TYPE_ID
				from parliament_type 
				where parliament_type='$this->parliament_type'
				)
order by PARLIAMENT_NUMBER DESC
ELECTION;
		$result = mysqli_query($this->dbcnx, $this->election_drop_query) or die(mysqli_error($this->dbcnx));
		if (!result) {
			exit('<p> Unable to execute Constructor election_drop_query');
		}
		while ($row = mysqli_fetch_array($result)) {
			$this->election_id_array[]= $row['PARLIAMENT_ID'];
			$this->election_number_array[]= $row['PARLIAMENT_NUMBER'];
			$this->election_year_array[]= $row['ELECTION_YEAR'];
			$this->parliament_type_id= $row['PARLIAMENT_TYPE_ID'];
		}
		mysqli_free_result($result);
		if ($this->parliament_number=='LATEST') {
			//$this->parliament_number = $this->election_number_array[count($this->election_number_array)-1];
			$this->parliament_number = $this->election_number_array[0];
		}
			
		$idquery = <<<ID
select PARLIAMENT_ID, YEAR(ELECTION_DATE) ELECTION_YEAR 
from parliament 
where PARLIAMENT_TYPE_ID = $this->parliament_type_id
and PARLIAMENT_NUMBER = $this->parliament_number
ID;

		$result = mysqli_query($this->dbcnx, $idquery) or die(mysqli_error($this->dbcnx));
		if (!result) {
			exit('<p> Unable to execute Constructor ID Query</p>');
		}
		$row = mysqli_fetch_row($result);

		if ($row == FALSE) {
			echo "ID Information not available for Constructor.<BR>";
			exit();
		}
		$this->parliament_id = $row[0];
		$this->election_year = $row[1];
		mysqli_free_result($result);

		drupal_add_js("JAVASCRIPT/table_hide.js");
		drupal_add_js("JAVASCRIPT/sorttable.js");

	}
	//---------------
	// PUBLIC METHOD
	function ToString() {
		echo "parliament_type is $this->parliament_type<BR>\n";
		echo "parliament_number is $this->parliament_number<BR>\n";
		echo "parliament_id was queried to be $this->parliament_id<BR>\n";
	}
	function GetParliamentNumber() {
		return $this->parliament_number;
	}
	function GetNumDisplay(){}
	function SplashInfo() {}
	function StatsInfo() {}
	function ReturnTitle() {
		return(CreateTitle($this->election_year, $this->parliament_type));
	}
	function CreateElectionDropDown(){
		$election_array = array();
		for ($i=0; $i < count($this->election_id_array); $i++){
			$form_key = $this->parliament_type."/".$this->election_number_array[$i];
			$form_value = CreateCrumb($this->election_year_array[$i],$this->parliament_type,$this->election_number_array[$i]);
			$election_array[$form_key]= $form_value;
		}
		$form = drupal_get_form('create_drupal_election_form',$election_array);
		echo "<div id='electiondropbox'>$form</div>";
	}

	function GeneralInfo() {
		$region_name= $this->parliament_type;
		if ($region_name=='Federal') { $region_name='Canada';}
$govparty_query = <<<GOVPQUERY
select PARTY_ENGLISH_NAME,PARTY_FRENCH_NAME, LEADER_NAME
from party p, party_leader l
where p.PARTY_ID=(select GOVERNING_PARTY from parliament where PARLIAMENT_ID=$this->parliament_id)
and p.PARTY_ID=l.PARTY_ID
and l.PARLIAMENT_ID=$this->parliament_id
and l.LEADER_NUMBER=1;
GOVPQUERY;

		$result = mysqli_query($this->dbcnx, $govparty_query) or die(mysqli_error($this->dbcnx));
		if (!$result) {
			exit('<p> Unable to execute GeneralInfo govparty_query');
		}

		while ($row = mysqli_fetch_array($result)) {
			$party_english= $row['PARTY_ENGLISH_NAME'];
			$party_french= $row['PARTY_FRENCH_NAME'];
			$party_leader= $row['LEADER_NAME'];
		}
		mysqli_free_result($result);

$picture_query = <<<PICQUERY
select PIC_NAME
from pic_resource 
where ENTITY='REGION'
and ENTITY_ID='$region_name'
and PIC_TYPE='FLAG'
PICQUERY;
		$result = mysqli_query($this->dbcnx, $picture_query) or die(mysqli_error($this->dbcnx));
		if (!$result){
			exit('<p> Unable to execute GeneralInfo picture_query');
		}

		while ($row = mysqli_fetch_array($result)) {
			$flag_pic= $row['PIC_NAME'];
		}
		mysqli_free_result($result);
		echo "<IMG class='flag' id='top-flag' SRC='../IMAGES/$flag_pic'></IMG>";
		if ($this->parliament_type=='Federal') { $leader_type_english='PRIME MINISTER';}
		else { $leader_type_english='PREMIER';}
		$govdate_query = <<<GOVDATEQUERY
select DATE_FORMAT(START_DATE, '%D %M %Y') START_DATE, DATE_FORMAT(END_DATE, '%D %M %Y') END_DATE,
	DATE_FORMAT(ELECTION_DATE, '%D %M %Y') ELECTION_DATE
from parliament
where PARLIAMENT_ID=$this->parliament_id;
GOVDATEQUERY;

		$result = mysqli_query($this->dbcnx, $govdate_query) or die(mysqli_error($this->dbcnx));
		if (!$result) {
			exit('<p> Unable to execute GeneralInfo govdate_query');
		}
		while ($row = mysqli_fetch_array($result)) {
			$start_date = $row['START_DATE'];
			$end_date = $row['END_DATE'];
			$election_date = $row['ELECTION_DATE'];
			if ($start_date == '') { $start_date = 'Not Available'; }
			if ($end_date == '') { $end_date = 'Present'; }
			if ($election_date == '') { $election_date = 'Not Available'; }
		}
		mysqli_free_result($result);
		echo "<p>$leader_type_english $party_leader - $party_english</p>";
		echo "<p>".CreateParliamentTitle($this->parliament_number, $this->parliament_type)."</p>";
		if ($start_date != 'Not Available') {
			echo "<p>Duration $start_date - $end_date</p>";
		}
		echo "<p>Election Held on $election_date</p>";
		#echo "<p>Picture of TOP area.</p>";
	} //End GeneralInfo
	function HasMultipleElections(){
$election_count_query = <<<ELECOUNT
select COUNT(*)
from parliament
where PARLIAMENT_TYPE_ID=$this->parliament_type_id
ELECOUNT;
		$result = mysqli_query($this->dbcnx, $election_count_query) or die(mysqli_error($this->dbcnx));
		if (!$result) {
			exit('<p> Unable to execute RegionCount query');
		}
		$row = mysqli_fetch_array($result);
		if ($row[0] > 1){
			$toReturn = TRUE;
		}
		else {
			$toReturn = FALSE;
		}
		return $toReturn;
	} //End HasMultipleElections
	function ReturnTopCrumb(){
		$top_crumb = CreateCrumb($this->election_year,$this->parliament_type,$this->parliament_number);
		return $top_crumb;
	}//End ReturnTopCrumb
} // End class ElectionObject
?>

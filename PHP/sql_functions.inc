<?php
include_once "table.inc";
include_once "function.inc";

function GetTotalQuery() {
	include "sql.inc";
	$return_query = str_replace("CONDITION", "", $total_query); 
	return($return_query);
}

function GetTotalInfo($dbcnx) {
	$total_query = GetTotalQuery();

	$result = mysqli_query($dbcnx, $total_query) or die(mysqli_error());

	if (!result) {
	        exit('<p> Unable to execute Total Query</p>');
	}

	$row = mysqli_fetch_row($result);

	if ($row == FALSE) {
	        echo "No information available for GetTotalInfo.<BR>";
	        exit();
	}

	return(array($row[0],$row[1]));
}

function GetTotalSeatQuery() {
	include "sql.inc";
	$return_query = str_replace("CONDITION", "", $total_seat_query); 
	return($return_query);
}

function GetTotalSeats($dbcnx) {
	$seat_query = GetTotalSeatQuery();

	$result = mysqli_query($dbcnx, $seat_query) or die(mysqli_error());

	if (!result) {
	        exit('<p> Unable to execute Total Seats Query</p>');
	}

	$row = mysqli_fetch_row($result);

	if ($row == FALSE) {
	        echo "No information available for GetTotalSeats<BR>";
	        exit();
	}

	return($row[0]);
}

function GetTopWasteQuery($in_parliament,$in_province,$in_region) {
	include "sql.inc";
	
	$condition='';

	if ($in_parliament!="ALL") {
		$condition.= " and d.PARLIAMENT_ID=$in_parliament ";
	}
	if ($in_province!="ALL") {
		$condition.= " and d.PROVINCE_ID=$in_province ";

	}
	if ($in_region!="ALL") {
		$condition.= " and d.DISTRICT_ID IN (SELECT DISTRICT_ID from region_districts where REGION_ID=$in_region) ";
	}
	$return_query = str_replace("CONDITION", $condition, $twaste_query);
	return($return_query);
}

function CreateTopWasteTable($dbcnx, $in_parliament, $in_province, $in_region) {
	include "global.inc";

	$twaste_query = GetTopWasteQuery($in_parliament,$in_province,$in_region);

	$result = mysqli_query($dbcnx, $twaste_query) or die(mysqli_error());
        if (!result) {
                exit('<p> Unable to execute Top Waste Query</p>');
        }

	if ($in_parliament=="ALL"){
		$header_array = array("Election","District","Percent Votes Wasted");
	}
	else {
		$header_array = array("District", "Percent Votes Wasted");
	}

	$topFiveTable = new Table("topFiveTable",$header_array );
	$overFiftyTable = new Table("overFiftyTable", $header_array);
	$overFiftyTable->Hide();

	$display_count==0;
	while ($row = mysqli_fetch_array($result)) {
		$display_count++;

		$parliament_number= $row['PARLIAMENT_NUMBER'];
		$parliament_type = $row['PARLIAMENT_TYPE'];
		$election_year = $row['ELECTION_YEAR'];

		$district_id= $row['DISTRICT_ID'];
		$district_french= $row['DISTRICT_FRENCH_NAME'];
		$district_english= $row['DISTRICT_ENGLISH_NAME'];

		$province_french = $row['PROVINCE_FRENCH_ABBREVIATION'];
		$province_english = $row['PROVINCE_ENGLISH_ABBREVIATION'];
		
		$wasted_percent = $row['WASTED'];

		$election_title = CreateTitle($election_year, $parliament_type);
		

	
		$wasted_disp = PercentConversion($wasted_percent, "ROUND");

		$election_disp = "<A HREF='?q=node/$ELECTION_AREA_NODE/$parliament_type/$parliament_number/0/TOP'>$election_title</A>";
		if ($in_province == "ALL") {
			$district_disp = $district_english.'-'.$province_english;
		}
		else {
			$district_disp = $district_english;
		}
		$district_disp = "<A HREF='?q=node/$ELECTION_AREA_NODE/$parliament_type/$parliament_number/$district_id/DISTRICT'>$district_disp</A>";



		if ($in_parliament == "ALL") {
			$row_array = array($election_disp,$district_disp, "<em>$wasted_percent</em>|$wasted_disp");
		}
		else {
			$row_array = array($district_disp, "<em>$wasted_percent</em>|$wasted_disp");
		}

		if ($display_count < 6) {
			$topFiveTable->Add($row_array);
		}
		$overFiftyTable->Add($row_array);	
	}
	mysqli_free_result($result);
	
	$return_table = array($topFiveTable->Create(),$overFiftyTable->Create(),"$display_count");
	return $return_table;
}
?>

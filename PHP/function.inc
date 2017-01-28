<?php
function CreateTitle($in_year, $in_type) {
	$parliament_title = $in_year." Election for";
	if ($in_type=='Federal') { $parliament_title.= " the Federal Government of Canada";}
	else {$parliament_title.= " the Government of ".$in_type; }
	return($parliament_title);
} //end CreateTitle

function CreateCrumb($in_year, $in_type, $in_number) {
	$crumb = $in_year." ".$in_type." (".ordinalize($in_number).")";
	return($crumb);
} //end CreateCrumb

function CreateParliamentTitle($in_number, $in_type) {
	$parliament_title = ordinalize($in_number)." Parliament";
	if ($in_type=='Federal') { $parliament_title.= " of the Federal Government of Canada";}
	else {$parliament_title.= " of the Government of ".$in_type; }
	return($parliament_title);
} //end CreateTitle

function PercentConversion($percent_value, $option) {
	if ($option=='ROUND') {
		$return_value = round($percent_value, 2);
	}
	if ($option=='ENGLISH') {
		$full_round = round($percent_value,0);
		$diff = $full_round - $percent_value;
		#echo "<BR>full_round is $full_round<BR>actual_value is $percent_value<BR>diff is $diff<BR>";
		if ($percent_value < 1) { $return_value = "Less than 1"; }
		else if ($diff > 0) { $return_value = "Almost $full_round"; }
		else if ($diff == 0) { $return_value = "$full_round"; }
		else if ($diff > -0.1) { $return_value = "Around $full_round"; }
		else {$return_value = "More than $full_round"; } 
	}
	$return_value .= " %";
	return($return_value);
}
function DisplayPicture($file_name) {
	include "global.inc";
	$return_value = "<IMG SRC='$IMAGE_DIR$file_name'></IMG>";
	return($return_value);
}
function ordinalize($num) {
       if (!is_numeric($num))
               return $num;

       if ($num % 100 >= 11 and $num % 100 <= 13)
               return $num."th";
       elseif ( $num % 10 == 1 )
               return $num."st";
       elseif ( $num % 10 == 2 )
               return $num."nd";
       elseif ( $num % 10 == 3 )
               return $num."rd";
       else // $num % 10 == 0, 4-9
               return $num."th";
}
?>

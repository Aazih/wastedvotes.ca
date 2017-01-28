<?php
	require_once("login.inc");
	$dbcnx = LoginDev();
	$result = mysqli_query($dbcnx, 'SELECT party_french_name from party');
	if (!result) {
		exit('<p> Unable to execute query</p>');
	}
	$count = 0;	
	while ($row = mysqli_fetch_array($result)) {
		print ''.$row['party_french_name'].'<BR>';
		$count++;
	}
		
?>

<?php
	function LoginProd(){
		$dbcnx = mysqli_connect('localhost', 'root','mysql');
		if (!$dbcnx) {
			echo '<p> Unable to connect to the prod database server</p>';
			exit();
		}
		if (!mysqli_select_db($dbcnx, 'elections_prod')) {
			exit('<p> Unable to select prod database</p>');
		}
		return $dbcnx;
	}
	function LoginDev(){
		$dbcnx = mysqli_connect('127.0.0.1', 'root','mysql');
		if (!$dbcnx) {
			echo '<p> Unable to connect to the database server</p>';
			exit();
		}
		if (!mysqli_select_db($dbcnx, 'elections_dev')) {
			exit('<p> Unable to select database</p>');
		}
		return $dbcnx;
	}
	function LoginTest(){
		$dbcnx = mysqli_connect('127.0.0.1', 'root','mysql');
		if (!$dbcnx) {
			echo '<p> Unable to connect to the database server</p>';
			exit();
		}
		if (!mysqli_select_db($dbcnx, 'elections_test')) {
			exit('<p> Unable to select database</p>');
		}
		return $dbcnx;
	}
		
?>

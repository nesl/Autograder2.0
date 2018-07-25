<?php
	$fGrade = $_POST['fGrade'];
	$l_name = $_POST['l_name'];
	$f_name = $_POST['f_name'];
	$attempt = 1;
	$fileName = $l_name . '_' . $f_name . '_submission' . $attempt . '.txt';
	while(file_exists($fileName)){
		$attempt++;
		$fileName = $l_name . '_' . $f_name . '_submission' . $attempt . '.txt';
	}
	$myfile = fopen($fileName,'a+');
	fwrite($myfile, $fGrade);
	fclose($myfile);
?>
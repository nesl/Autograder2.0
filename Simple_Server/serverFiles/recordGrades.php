<?php
	$submission = $_POST['submission'];
	$myfile = fopen('Submission.txt','a+');
	fwrite($myfile, $text1);
	fclose($myfile);
?>
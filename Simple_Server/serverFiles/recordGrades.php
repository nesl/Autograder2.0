<?php
	$text1 = $_POST['text1'];
	$myfile = fopen('Submission.txt','a+');
	fwrite($myfile, $text1);
	fclose($myfile);
?>
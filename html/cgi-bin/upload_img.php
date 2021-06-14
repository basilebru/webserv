#!/usr/bin/php-cgi
<?php
$uploaddir = '../upload/';
$uploadfile = $uploaddir . basename($_FILES['file']['name']);

if (move_uploaded_file($_FILES['file']['tmp_name'], $uploadfile)) {
    $message = "Le fichier est valide, et a été uploadé avec succès.\n";
} else {
    $message = "L'upload a échoué\n";
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Document</title>
</head>
<body>
	<p><strong><?= $message . ": " . $_FILES['file']['name'] ?></strong></p>
	<p>Il est de type: <strong><?= $_FILES['file']['type'] ?></strong></p>
	<p>Et a une taille de: <strong><?= $_FILES['file']['size'] ?></strong></p>	
</body>
</html>
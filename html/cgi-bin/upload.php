<?php
$uploaddir = $_SERVER['DOCUMENT_ROOT'] . "/" . getenv('UPLOAD_DIR');
$uploadfile = $uploaddir . "/" . basename($_FILES['file']['name']);

if (!is_dir($uploaddir))
    mkdir($uploaddir, 0775);

if (move_uploaded_file($_FILES['file']['tmp_name'], $uploadfile)) {
    $message = "Le fichier est valide, et a été uploadé avec succès.\n";
} else {
    $message = "L'upload a échoué\n";
}
$fileError = $_FILES["file"]["error"];
switch($fileError) {
    case UPLOAD_ERR_INI_SIZE:
        $error = "Exceeds max size in php.ini";
        break;
    case UPLOAD_ERR_PARTIAL:
        $error = "Exceeds max size in html form";
        break;
    case UPLOAD_ERR_NO_FILE:
        $error = "No file was uploaded";
        break;
    case UPLOAD_ERR_NO_TMP_DIR:
        $error = "No /tmp dir to write to";
        break;
    case UPLOAD_ERR_CANT_WRITE:
        $error = "Error writing to disk";
        break;
    default:
        $error = "All went successfully !";
        break;
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
	<p>Error: <strong><?= $error ?></strong></p>	
</body>
</html>
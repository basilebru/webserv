<?php
    if (!empty($_POST))
    {
        $name = $_POST['name'];
        $surname = $_POST['surname'];
    }
    else if (!empty($_GET))
    {
        $name = $_GET['name'];
        $surname = $_GET['surname'];
    }
    else
    {
        $name = "No value entered";
        $surname = "No value entered";
    }
?>
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>POST Result</title>
</head>
<body>
    <h2>Vous avez rentré:</h2>
	<p><strong>Name: <?= $name ?></strong></p>
    <p><strong>Surname: <?= $surname ?></strong></p>
</body>
</html>
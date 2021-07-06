<?php
	$env = getenv();
?>
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>WEBSERV CGI</title>
</head>
<body>
<h1>Webserv CGI environnement variables</h1>
<?php
	foreach ($env as $key => $value) {
		echo "<p><strong>$key =></strong> \"$value\"</p>\n";
	}
?>
<p><strong>© BB & JN :-)</strong></p>
</body>
</html>
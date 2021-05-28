#!/usr/bin/php

<?php
	$env = getenv();
	echo("HTTP/1.1 200 OK\n");
	echo("Content-Type: text/html\n");
	echo("Connection: close\n\n");
	// phpinfo();
?>
<html lang="fr">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>WEBSERV CGI</title>
</head>	
<body>
<h1>Webserv CGI environnement variables</h1>
<?php
	foreach ($env as $key => $value) {
		echo "<p>$key => $value</p>\n";
	}
?>
</body>
</html>

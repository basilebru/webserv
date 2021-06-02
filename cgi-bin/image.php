#!/usr/bin/php
<?php
$image = file_get_contents("/home/julien/Cursus42/webserv/html/images/favicon.ico");
echo("Accept-Ranges: bytes\r\n");
echo("Content-Length: 1150\r\n");
echo("Content-Type: image/vnd.microsoft.icon\r\n");
echo("Connection: keep-alive\r\n\r\n");
echo($image);
?>
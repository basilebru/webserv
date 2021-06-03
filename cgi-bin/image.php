#!/usr/bin/php
<?php
$image = file_get_contents("./html/images/favicon_96.png");
echo("Accept-Ranges: bytes\r\n");
echo("Content-Length: 7936\r\n");
echo("Content-Type: image/png\r\n");
echo("Connection: keep-alive\r\n\r\n");
echo($image);
?>
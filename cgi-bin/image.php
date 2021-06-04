#!/usr/bin/php
<?php
$image = file_get_contents("./html/images/favicon_96.png");
echo("Accept-Ranges: bytes\n");
echo("Content-Length: 7936\n");
echo("Content-Type: image/png\n");
echo("Connection: keep-alive\n\n");
echo($image);
?>
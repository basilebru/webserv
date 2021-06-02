#!/usr/bin/php
<?php
$image = file_get_contents("/home/julien/Cursus42/webserv/html/images/favicon.ico");
// echo(base64_encode("Accept-Ranges: bytes\r\n"));
// echo(base64_encode("Content-Length: 1150\r\n"));
// echo(base64_encode("Content-Type: image/vnd.microsoft.icon\r\n"));
// echo(base64_encode("Connection: keep-alive\r\n\r\n"));
echo(base64_encode($image));
?>
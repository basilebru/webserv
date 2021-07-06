<?php
$image = file_get_contents("../images/42.png");
header("Content-Type: image/png");
echo($image);
?>
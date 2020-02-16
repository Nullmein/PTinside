<?php
$fp = fopen('stdin.txt', 'w');
fwrite($fp, $_POST["command"]);
fclose($fp);
echo "OK";
?>
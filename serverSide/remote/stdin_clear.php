<?php
$fp = fopen('stdin.txt', 'w');
fwrite($fp, "");
fclose($fp);
echo "OK";
?>
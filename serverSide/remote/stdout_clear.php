<?php
$fp = fopen('stdout.txt', 'w');
fwrite($fp, "");
fclose($fp);
echo "OK";
?>
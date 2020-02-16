<?php
$fp = fopen('stdout.txt', 'a');
fwrite($fp, $_POST["output"]."\n");
fclose($fp);
echo "OK";
?>
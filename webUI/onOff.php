<html>
<body>

<?php
//print which outlet is being turned on or off 
//if word "on" or "off" is found in input
$status = $_POST["onOff"];
$number = $_POST["outletNum"];
$pattern = "/^on|off$/i";
if (preg_match($pattern,$status)) echo "Turning <b>$status</b> outlet number <b>$number</b>";
else echo "Did not receive on or off command";

?>

</body>
</html>

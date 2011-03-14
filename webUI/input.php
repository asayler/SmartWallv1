<html>
<body>

All active outlets:  <br />
<?php

//get outlet list from Andy's compiled swls.c program
$b = shell_exec("cd /home/laura/senior/code/SmartWallv1/usermon && ./swls 2>&1");

//extract 16digit hex values
$UID_format = "/(0x\d{16})/";
preg_match_all($UID_format, $b, $matches,PREG_PATTERN_ORDER);
//print_r($matches);

//extract UIDs
$counter = 0; 
foreach($matches[1] as $value) { 
   if($counter%2 != 0){
   $UIDs[] = $value;
   }
   $counter++;
}
//print_r($UIDs);
?>

<!-- Print all UIDs in a selection form-->
<form action="<?php echo $PHP_SELF; ?>" method="get">
  <select name="outlets" size="<?php echo count($UIDs); ?>">
    <?php
       foreach($UIDs as $value) {
       echo "<option value=$value>$value</option>";
       }
    ?>
  </select>
  <input type="submit" value="Get Status" name="submit">
</form>

<?php
   if (isset($_GET['submit'])){
     $outlet = $_GET["outlets"];
     echo "Outlet ".$outlet." is on/off";
   }
?>

<!---
<form action="onOff_test.php" method="post">
Outlet: <input type="text" name="outletNum" />
On/Off: <input type="text" name="onOff" />
<input type="submit" />
</form>
-->

</body>
</html>

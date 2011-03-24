<html>
<body>

All active outlets:  <br />
<?php

//get outlet list from Andy's compiled swls.c program

//change directory for relative path purposes
chdir('/home/laura/senior/code/SmartWallv1/usermon');
$raw = shell_exec("./swls -raw 2>&1");

//initialize array $lookup
//$lookup will hold hash of UID->array(other data)
$lookup = array(); 

$lines = explode("\n", $raw); //break up on newline characters
foreach($lines as $line) {
   $items = explode(" ", $line); //break up on spaces
   $lookup[$items[6]] = array('swAdr' => $items[1], 'ipAdr' => $items[2], 'type' => $items[3], 'ch' => $items[4], 'ver' => $items[5]); 
}   

foreach($lookup as $key => $value) {
   $UIDs[] = $key;
}
//print_r($UIDs); //debug

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

</body>
</html>

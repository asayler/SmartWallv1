<?php
  //   error_reporting(0); //suppress error reports. uncomment when problems
?>

<html>
<head>
<title>Set Timers</title>
<link rel="stylesheet" type="text/css" href="style.css"
</head>

<body>
<div id="containter">

<?php include("header.inc"); ?>

<div id="content">

<?php
//<SW opcode> (i.e. what you're querying for or setting)
$state = "0x0010";
$power = "0x0022";
?>

<h4> All active outlets: </h4>  

<?php //get outlet list from Andy's compiled swls.c program

//change directory for relative path purposes
chdir('/home/laura/senior/code/SmartWallv1/usermon');
$raw = shell_exec("./swls -raw 2>&1");
$raw = trim($raw, "\n");

//initialize array $lookup
//$lookup will hold hash of UID->array(other data)
$lookup = array(); 

$lines = explode("\n", $raw); //break up on newline characters
foreach($lines as $line) {
   $items = explode(" ", $line); //break up on spaces
   $lookup[$items[7]] = array('swAdr' => $items[1], 'ipAdr' => $items[2], 'type' => $items[3], 'channels' => $items[4], 'grpId' => $items[5],'ver' => $items[6]); 
}   

foreach($lookup as $key => $value) {
   $UIDs[] = $key;
}
//print_r($UIDs); //debug

?>
<?php
//populate $aliases hash from aliases.txt 
chdir('/home/laura/senior/code/SmartWallv1/webUI');
$handle = fopen("./aliases.txt","r") or exit("Unable to open alias file.");
while(!feof($handle)) {
   $file_line = fgets($handle);
   $file_bits = explode(' ', $file_line);
   $the_UID = trim($file_bits[0]);
   $the_alias = trim($file_bits[1]);
   $UID_alias[$the_UID] = $the_alias;
   $alias_UID[$the_alias] = $the_UID;
}
fclose($handle);

foreach($UIDs as $value) {
   //check if this UID has an alias
   if(array_key_exists($value, $UID_alias)){
      $aliased_UIDs[] = $UID_alias[$value];
   } else {
      $aliased_UIDs[] = $value;
   }
}
chdir('/var/www/');
?>

<!-- Print all UIDs in a selection form-->
<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="get">
  <select name="outlets" size="<?php echo count($UIDs); ?>">
    <?php
    foreach($aliased_UIDs as $value) {
       echo "<option value=$value>$value</option>";
    }

    ?>
  </select>
  <input type="submit" value="View Energy Usage" name="usage">
</form>


  <?php
  //Notice button press of submit, display outlet info
  if (isset($_GET['usage'])){
    if($_GET["outlets"] != NULL) { //check that an outlet has been selected
      $outlet = $_GET["outlets"]; //selected outlet
      chdir('/home/laura/senior/code/SmartWallv1/usermon');
      
      //convert possibly aliased UID into normal UID
      if(array_key_exists($outlet, $alias_UID)){ //check if this is an alias
	$outlet = $alias_UID[$outlet]; //replace $outlet with UID
      }
      
     //query for device state: call format
     // ./swChnMsg <SW Dest Address> <SW Msg Type> <SW Tgt Type> 
     //        <SW Opcode> <Chn Arg Size (bytes)> <Chn#> <Chn Arg>
     
      $swAdr = $lookup[$outlet]['swAdr']; //shell_exec can't handle
      //NOTE: only outlet 0x0011 working right now! Others cause hang.
      $temp = shell_exec("./swChnMsg $swAdr QUERY OUTLET $state 1 0 x 2>&1"); //temp until power query ready
      echo $temp; //temp until power query ready
      //uncomment when power query for swChnMsg is ready
      //$energy = shell_exec("./swChnMsg $swAdr QUERY OUTLET $power 1 0 x 2>&1");
      //echo $energy;
   } else {
      echo "First, select an outlet.\n";
   }
}
?>

</div> <!-- end content -->

<?php include("navigation.inc"); ?>
<?php include("footer.inc"); ?>

</div> <!-- end container -->

</body>
</html>
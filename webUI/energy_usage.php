<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>Energy Usage</title>
<link rel="stylesheet" type="text/css" href="style.css"
</head>

<body>
<div id="" class="container_12">    
   <div id="header" class="grid_12">
   <?php include("header.inc"); ?>
</div>
</div>

<div id="content" class="container_12">
<div id="navigation" class="grid_2">
   <?php include("navigation.inc"); ?>
</div>

<div id="table" class="grid_5">
<?php
//sw opcodes (i.e. what you're querying for or setting)
$state = "0x0010";
$voltage = "0x0020";
$current = "0x0021";
$power = "0x0022";
$freq = "0x0023";
$phase = "0x0024";
//sw types
$outlet = "0x8000000000000004";
$master = "0x8000000000000001";
$universal = "0x8000000000000000";
?>

<h3> &nbsp&nbsp Select a device: </h3>  

<?php //get outlet list from Andy's compiled swls.c program

//change directory for relative path purposes
chdir('/home/vermilion/SmartWallv1/usermon');
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
chdir('../webUI');
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
  <select name="outlets" size="<?php echo count($UIDs)-1; ?>">
  <?php
  foreach($aliased_UIDs as $value) {
  if(array_key_exists($value, $alias_UID)){
    $proper_UID = $alias_UID[$value];
  }else{
    $proper_UID = $value;
  }
  $ttype = $lookup[$proper_UID]['type'];
  $temp = strcmp($ttype,$master);
  if($temp != 0){ #if not master outlet
    echo "<option value=$value>$value</option>";
  }
}
?>
</select>
<input type="submit" value="View Energy Usage" name="usage">
  </form>
  </div>

<div id="picture" class="grid_5">

  <?php
  //Notice button press of usage, display relevant graph
  if (isset($_GET['usage'])){
    if($_GET["outlets"] != NULL) { //check that an outlet has been selected
      $outlet = $_GET["outlets"]; //selected outlet
      
      //convert possibly aliased UID into normal UID
      if(array_key_exists($outlet, $alias_UID)){ //check if this is an alias
	$outlet = $alias_UID[$outlet]; //replace $outlet with UID
      }
      
      $string = $outlet.'.png';
            echo "<IMG SRC=\"$string\">"; //display graph
      //echo "<IMG SRC=\"fake.png\">"; #temp for screen shot
   } else {
      echo "First, select an outlet.\n";
   }
}
?> 
</div>
</div> <!-- end content -->

<div id="" class="container_12">
  <div id="footer" class="grid_12">
  <?php include("footer.inc"); ?>
</div>
</div>

</body>
</html>

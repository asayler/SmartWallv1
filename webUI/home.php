<?php
   error_reporting(0); //suppress error reports. uncomment when problems
?>

<html>
<head>
<title>SmartWall Home</title>
<link rel="stylesheet" type="text/css" href="style.css" />
    
</head>
    
<body>

<?php include("header.inc"); ?>

<div id="content">

<?php //get outlet list from Andy's compiled swls.c program

//change directory for relative path purposes
chdir('/home/laura/senior/code/SmartWallv1/usermon');
$raw = shell_exec("./swls -raw 2>&1");
$raw = trim($raw, "\n"); //trim trailing new line character

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
//chdir('/var/www/');
?>

<!-- Print all UIDs (aliased as appropriate) and their on/off status-->
<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="get">
  <?php
  chdir('/home/laura/senior/code/SmartWallv1/usermon');
echo "<table class=\"pretty\" border=\"1\">";
echo "<th>Outlet</th><th>Status</th>";
foreach($aliased_UIDs as $value) {
  //    	$swAdr = $lookup[$outlet]['swAdr']; //shell_exec can't convert
  //	$swAdr = "0x0000000000000011"; //debug tool until all outlets available
  $swAdr = "0x0011";
  $query = shell_exec("./swChnMsg $swAdr QUERY OUTLET 0x0010 1 0 x 2>&1");
  $query = chop($query);
  if(preg_match('/1$/',$query)) { //pick which button is marked
    $on = "checked";
    $off = "";
  } else if(preg_match('/0$/',$query)){
    $on = "";
    $off = "checked";
  }
  echo "<tr><td>&nbsp $value &nbsp</td>";
  echo "<td> <input type=\"radio\" name=$value value=\"On\" $on>On";
  echo "<input type=\"radio\" name=$value value=\"Off\" $off>Off &nbsp</td></tr>";
  
}
echo "</tr></table>";
?>
&nbsp&nbsp
<input type="submit" value="Apply Changes" name="apply">
  </form>
  
  <IMG SRC="0x0000000000000010.gif" ALT="Some Text">  



  <?php
  //Notice button press of apply, update outlets
  chdir('/home/laura/senior/code/SmartWallv1/usermon');
if (isset($_GET['apply'])){
  foreach($aliased_UIDs as $aUID){
    $on_off = $_GET[$aUID]; //selected outlet

    //convert possibly aliased UID into normal UID
    if(array_key_exists($aUID, $alias_UID)){ //check if this is an alias
      $uaUID = $alias_UID[$aUID]; 
    } 
    if(preg_match("/On/", $on_off)) {
	//    $swAdr = $lookup[$uaUID]['swAdr']; //shell_exec can't handle
	$swAdr = "0x0011"; //temp while other outlets aren't simulated
	$temp = shell_exec("./swChnMsg $swAdr SET OUTLET 0x0010 1 0 1 2>&1");
    } elseif(preg_match("/Off/",$on_off)) {
	//    $swAdr = $lookup[$uaUID]['swAdr']; //shell_exec can't handle
	$swAdr = "0x0011"; //temp while other outlets aren't simulated
	$temp = shell_exec("./swChnMsg $swAdr SET OUTLET 0x0010 1 0 0 2>&1");
    }
  }
  header('location:http://localhost/home.php'); //refresh to display changed data
 }

?>

</div> <!-- end content -->

<?php include("navigation.inc"); ?>
<?php include("footer.inc"); ?>

</body>
</html>
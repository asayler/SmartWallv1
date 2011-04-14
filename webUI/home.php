<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<title>SmartWall Home</title>
<link rel="stylesheet" type="text/css" href="style.css" />
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

<?php 
//get outlet list from Andy's compiled swls.c program
chdir('/home/vermilion/SmartWallv1/usermon'); //relative path purposes
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
//chdir('/var/www/');
?>

<!-- Print all UIDs (aliased as appropriate) and their on/off status-->

<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="get">
  <?php
  chdir('../usermon');
echo "<table class=\"pretty\" border=\"1\">";
echo "<tr><th>Outlet</th><th>Status</th></tr>";
foreach($aliased_UIDs as $value) {
  if($lookup[$value]['channels'] == "0x02"){
    $swAdr = $lookup[$value]['swAdr'];
    $query_string = "./swChnMsg $swAdr QUERY OUTLET $state 1 0 x 1 x"; 
    //echo "query is: ".$query_string."<br />";
    $query = shell_exec($query_string);
    //echo "response is: ".$query."<br />";
    //$query = rtrim($query, "\n");
    if(preg_match("/$swAdr REPORT OUTLET $state 0 (0|1) 1 (0|1)/",$query,$matches)) { 
      //echo "matches[1] is: ".$matches[1]."<br />";
      //echo "matches[2] is: ".$matches[2]."<br />";
      if ($matches[1] == 1) {
	//echo "[1] matched 1 <br />";
	$on_top = "checked";
	$off_top = "";
      } elseif ($matches[1] == 0) {
	//echo "[1] matched 0 <br />";
	$on_top = "";
	$off_top = "checked";
      }
      if ($matches[2] == 1){
	//echo "[2] matched 1 <br />";
	$on_bottom = "checked";
	$off_bottom = "";
      } elseif ($matches[2] == 0){
	//echo "[2] matched 0 <br />";
	$on_bottom = "";
	$off_bottom = "checked"; 
      }
    }
    $top_name = "top_".$value;
    $bottom_name = "bot_".$value;
    //echo "top name is: ".$top_name."<br />";
    //echo "bottom name is: ".$bottom_name."<br />";
    echo "<tr><td>&nbsp $value &nbsp</td>";
    echo "<td>";
    echo "<input type=\"radio\" name=$top_name value=\"On\" $on_top>On"; //top on
    echo "<input type=\"radio\" name=$top_name value=\"Off\" $off_top>Off"; //top
    echo "&nbsp<br />&nbsp";
    echo "<input type=\"radio\" name=$bottom_name value=\"On\" $on_bottom>On"; //bottom on
    echo "<input type=\"radio\" name=$bottom_name value=\"Off\" $off_bottom>Off";
    echo "&nbsp</td></tr>"; //bottom off
  }
}
echo "</table>";
?>
<input type="submit" class="button" value="Apply Changes" name="apply">
  </form>

  <?php
  //Notice button press of apply, update outlets
  chdir('../usermon');
if (isset($_GET['apply'])){
  foreach($aliased_UIDs as $aUID){
    if($lookup[$aUID]['channels'] == "0x02"){
      $top_string = "top_".$aUID;
      $bot_string = "bot_".$aUID;
      //echo "top string is: ".$top_string."<br />";
      //echo "bottom string is: ".$bot_string."<br />";
      //print_r($_GET);
      $on_off_top = $_GET[$top_string]; //selected outlet
      //echo "top to be turned: $on_off_top <br />";
      $on_off_bottom = $_GET[$bot_string];
      //echo "bottom to be turned: $on_off_bottom <br />";

      //convert possibly aliased UID into normal UID
      if(array_key_exists($aUID, $alias_UID)){ //check if this is an alias
	$uaUID = $alias_UID[$aUID]; 
      } else{
	$uaUID = $aUID; 
      }

      if(preg_match("/On/", $on_off_top)) {
	$top = '1';
      }elseif (preg_match("/Off/", $on_off_top)){
	$top = '0';
      }
      if(preg_match("/On/", $on_off_bottom)) { 
	$bottom = '1';
      }elseif (preg_match("/Off/", $on_off_bottom)){
	$bottom = '0';
      }
      $swAdr = $lookup[$uaUID]['swAdr']; 
      $set_string = "./swChnMsg $swAdr SET OUTLET $state 1 0 $top 1 $bottom 2>&1";
      //echo $set_string."\n";
      $temp = shell_exec($set_string); 
    }
  }
  header('location:http://localhost/home.php'); //refresh to display changed data
 }

?>
</div>

<div id="graph" class="grid_5">
  <IMG SRC="total.png" ALT="Some Text"> 
  <!-- <IMG SRC="fake2.png" ALT="Some Text"> -->
</div>
</div>

                

<div id="" class="container_12">
<div id="footer" class="grid_12">
  <?php include("footer.inc"); ?>
</div>
</div>
            
</body>
</html>

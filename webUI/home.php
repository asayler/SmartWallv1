<?php
   error_reporting(0); //suppress error reports. uncomment when problems
?>

<html>
<head>
<title>SmartWall Home</title>
<link rel="stylesheet" type="text/css" href="style.css" />

</head>

<body>
<div id="containter">

<div id="header">
<h1>SmartWall Home Control Pannel</h1>
</div> <!-- end header -->

<div id="content">

<?php //get outlet list from Andy's compiled swls.c program

//change directory for relative path purposes
chdir('/home/laura/senior/code/SmartWallv1/usermon');
$raw = shell_exec("./swls -raw 2>&1");

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
$handle = fopen("./aliases.txt","r") or exit("Unable to open file.");
while(!feof($handle)) {
   $file_line = fgets($handle);
   $file_bits = explode(' ', $file_line);
   $aliases[$file_bits[0]] = $file_bits[1];
}
fclose($handle);

foreach($UIDs as $value) {
   //check if this UID has an alias
   if(array_key_exists($value, $aliases)){
      $aliased_UIDs[] = $aliases[$value];
   } else {
      $aliased_UIDs[] = $value;
   }
}
//chdir('/var/www/');
?>

<!-- Print all UIDs in a selection form-->
<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="get">
    <?php
    chdir('/home/laura/senior/code/SmartWallv1/usermon');
    echo "<table class=\"pretty\" border=\"1\">";
    echo "<th>Status</th><th>Outlet</th>";
    foreach($aliased_UIDs as $value) {
//    	$swAdr = $lookup[$outlet]['swAdr']; //shell_exec can't convert
	$swAdr = "0x0000000000000011"; //debug tool until all outlets available
   	$query = shell_exec("./swChnMsg $swAdr QUERY OUTLET 0x0010 1 0 x 2>&1");
	$query = chop($query);
	if(preg_match('/1$/',$query)) { //pick which button is marked
	   $on = "checked";
	   $off = "";
	} else if(preg_match('/0$/',$query)){
	   $on = "";
	   $off = "checked";
	}
    	echo "<tr><td> <input type=\"radio\" name=$value value=\"On\" $on>On";
        echo "<input type=\"radio\" name=$value value=\"Off\" $off>Off &nbsp</td>";
	echo "<td>&nbsp $value &nbsp</td></tr>";
    }
    echo "</tr></table>";
    ?>
  <input type="submit" value="Get Status" name="submit">
</form>


<?php
//Notice button press of submit, display outlet info
if (isset($_GET['submit'])){
   if($_GET["outlets"] != NULL) { //check that an outlet has been selected
   $outlet = $_GET["outlets"]; //selected outlet
   chdir('/home/laura/senior/code/SmartWallv1/usermon');

   //convert possibly aliased UID into normal UID

   //query for device state: call format
   // ./swChnMsg <SW Dest Address> <SW Msg Type> <SW Tgt Type> 
   //        <SW Opcode> <Chn Arg Size (bytes)> <Chn#> <Chn Arg> ...
   echo "./swChnMsg ".$lookup[$outlet]['swAdr']." QUERY OUTLET 0x0010 1 0x01 x\n"; //debug  
   echo "<br />";       
   $swAdr = $lookup[$outlet]['swAdr']; //shell_exec can't handle
   //NOTE: only outlet 0x0011 working right now! Others cause hang.
   $temp = shell_exec("./swChnMsg $swAdr QUERY OUTLET 0x0010 1 0x01 x 2>&1");
   //IMPORTANT: when swChnMsg fails, it does so silently and hangs
   //for now, kill it with:
   // >> sudo killall swChnMsg
   // check for hung programs with:
   // >> ps aux | grep swChnMsg
   echo $temp."\n";
   } else {
      echo "First, select an outlet.\n";
   }
}
?>

</div> <!-- end content -->

<div id="navigation">
     <ul>
     <li><a href="http://localhost/home.php">Home</a></li>
     <li><a href="http://localhost/timers.php">Timers</a></li>
     <li><a href="#">Trends</a></li>
     <li><a href="#">Rename</a></li>
     </ul>
</div>  <!-- end navigation -->

<div id="footer">
     Footer
</div> <!-- end footer -->

</div> <!-- end container -->

</body>
</html>

<?php
   error_reporting(0); //suppress error reports. uncomment when problems
?>

<html>
<head>
<title>SmartWall Home</title>
<style type="text/css">
	body{
		margin: 0;
             	padding: 0;
		background: #ccc;
	}
	#container{
		width: 960px;
		margin: 0 auto;
	}		
	#header{
		padding: 5px;
		background: #bbb;
	}
	#navigation{
		float: left;
		width: 150px;
		background: #eee;
	}
	#content{
		float: right;
		width: 810px;
		background: #fff;
	}
	#navigation a:link,a:visited{
		display: block;
		padding: 4px;
		text-decoration: none;
	}
	#navigation a:hover,a:active{
		    background-color: #bbf
	}
	#footer{
		clear: both;
		background: #bbb;
		padding: 10px;
	}
	ul{
		margin: 0;
		padding: 0;
		list-style-type: none;
		width: 150px;
	}
</style>
</head>

<body>
<div id="containter">

<div id="header">
<h1>SmartWall Homepage</h1>
</div> <!-- end header -->

<div id="content">
<h4> All active outlets: </h4>  

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
     <li><a href="#">Home</a></li>
     <li><a href="#">Next</a></li>
     </ul>
</div>  <!-- end navigation -->

<div id="footer">
     Footer
</div> <!-- end footer -->

</div> <!-- end container -->

</body>
</html>

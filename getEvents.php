<?php
include("simple_html_dom.php");

if($argc != 2){
  exit(0);
}

// Create DOM from URL or file
$html = file_get_html('http://polling.bbc.co.uk/sport/shared/football/oppm/live-text-commentary/' . $argv[1]);

$html->find('#live-text',0)->innertext = '<link type="text/css" rel="stylesheet" href="http://static.bbci.co.uk/sport/2.7.249/desktop/styles/core-compiled.css">' . $html->find('#live-text',0);

//$fname = $argv[1] . ".html";
//$f = fopen($fname,"w");
//fwrite($f,$html->find('#live-text-commentary-wrapper',0));
//fclose($f);
//print shell_exec('links -dump ' . $fname);
//shell_exec('rm ' . $fname);

print $html->find('#live-text',0);
print $html->find('#more-live-text',0);
?>

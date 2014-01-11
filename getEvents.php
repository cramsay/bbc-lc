<?php
include("simple_html_dom.php");

if($argc != 2){
  exit(0);
}

// Create DOM from URL or file
$html = file_get_html('http://polling.bbc.co.uk/sport/shared/football/oppm/live-text-commentary/' . $argv[1]);

//Remove introductary "Live Text Commentary" h2
$html->find('#live-text-introduction',0)->innertext = "";

// Add in tags for ncurses formatting
// Event Highlighting
foreach($html->find('p[class=event]') as $e){
  $e->innertext = 'EVENT_START' .  $e . 'EVENT_END<br/>';
}

$fname = $argv[1] . ".html";
$f = fopen($fname,"w");
fwrite($f,$html->find('#live-text-commentary-wrapper',0));
fclose($f);
print shell_exec('links -dump ' . $fname);
shell_exec('rm ' . $fname);
?>

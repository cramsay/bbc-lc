<?php
include("simple_html_dom.php");

// Create DOM from URL or file
$html = file_get_html('http://polling.bbc.co.uk/sport/shared/football/oppm/live-text-commentary/' . $argv[1]);

/*foreach($html->find('tr') as $row){
  if($row->class=='fixture'||$row->class=='report'){
        if(sizeof($row->find('td'))>1){
                $score = $row->find('td[class=match-score]',0);
                $results[] = $score->find('span[class=team-home]',0)->plaintext;
                $results[] = $score->find('span[class=team-away]',0)->plaintext;
                $results[] = end(split('-',$row->id));
        }
  }
}*/

foreach($html->find('p[class=event]') as $e){
  $e->innertext = 'EVENT_START' .  $e . 'EVENT_END<br/>';
}

$fname = $argv[1] . ".html";
$f = fopen($fname,"w");
fwrite($f,$html->find('#live-text',0));
fclose($f);
print shell_exec('links -dump ' . $fname);
shell_exec('rm ' . $fname);
?>

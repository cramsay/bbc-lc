<?php 
include("simple_html_dom.php");

// Create DOM from URL or file
$html = file_get_html('http://www.bbc.co.uk/sport/shared/football/live-scores/matches/118996176/today');

$results=array();

// Find all fixtures
foreach($html->find('tr') as $row){
  if($row->class=='fixture'||$row->class=='report'||$row->class=='live'){
        if(sizeof($row->find('td'))>1){
                $score = $row->find('td[class=match-score]',0);
                $results[] = $score->find('span[class=team-home]',0)->plaintext;
                $results[] = $score->find('span[class=team-away]',0)->plaintext;
                $results[] = end(split('-',$row->id));
        }
  }
}

//Print the fixtures out - prepended by the number of fixtures
echo (sizeof($results)/3) ."\n";
foreach($results as $l)
  echo $l . "\n";

?>

#!/bin/bash

#Get raw live commentary page
wget --no-verbose -O lc.html  http://polling.bbc.co.uk/sport/shared/football/oppm/live-text-commentary/$1 &> /dev/null

#Process the raw html
#Remove extra time tags (makes splitting events much easier)
sed -i 's/+[0-9]\+:[0-9]\{2\}//g' lc.html
#Add in hr tags after each event
sed -i 's/[0-9]\+:[0-9]\{2\}/&<hr\/>/g' lc.html

#Get the formated text (not html) from links dump
links -width 80 -dump lc.html

#Clear up
rm lc.html

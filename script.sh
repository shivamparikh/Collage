#!/bin/bash
# My first script

echo "Hello World!"
cd ..
echo "I just changed the directory"
cd ~/Development/C++
pwd
cd ./Collage
echo "enter the function you would like to run"
read function
cpp = ".cpp"
cpp = $function$cpp
g++ -o $function $cpp Color.cpp
echo "Input the file name"
read fName
echo "Input the output file name"
read oName
echo "Input the desired scale"
read scale
echo "filter split count"
read sCount
./$function $fName $oName $scale $sCount

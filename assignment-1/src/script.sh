#!/bin/bash

if test -f ./input.txt;then
  rm ./input.txt
fi

g++ -g image_sharpener.cpp libppm.cpp

declare -a average=(
  [0]=0
  [1]=0
  [2]=0
  [3]=0
  [4]=0
)

for num in {1..5}
do
./a.out ../images/$num.ppm result.ppm | cut -d ' ' -f 4 >> input.txt
done

g++ average.cpp -o avg
somevar=$(./avg)
echo "AVERAGE TIME ARE: " >> input.txt
echo $somevar >> input.txt

if test -f ./avg;then
  rm ./avg
fi
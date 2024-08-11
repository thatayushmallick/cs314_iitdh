#!/bin/bash

if test -f ./input.txt;then
  rm ./input.txt
fi

g++ -g image_sharpener.cpp libppm.cpp

if test -f avg.txt;then
  rm ./avg.txt
fi

for num in {1..7}
do
if test -f input.txt;then
  rm ./input.txt
fi
for x in {1..5}
do
./a.out ../images/$num.ppm result.ppm | cut -d ' ' -f 4 >> input.txt
done
g++ average.cpp -o avg
somevar=$(./avg)
echo "AVERAGE TIME FOR FILE $num ARE: " >> avg.txt
echo $somevar >> avg.txt
if test -f ./avg;then
  rm ./avg
fi
done
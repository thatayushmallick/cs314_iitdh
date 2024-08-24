build-part1: part1.out

part1.out: part1_searcher.cpp
	g++ -g part1_searcher.cpp -o part1.out

run-part1: part1.out
	./part1.out file.txt NGTNIJGK 0 67108863

build-part2: part2.out

part2.out: part2_partioner.cpp
	g++ -g part2_partioner.cpp -o part2_partioner.out

run-part2: part1.out part2_partioner.out
	./part2_partioner.out file.txt NGTNIJGK 0 67108863 8388608

clean-part1:
	rm part1.out

clean:
	rm *.out

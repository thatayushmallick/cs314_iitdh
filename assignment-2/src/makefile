build-part1: part1.out

part1.out: part1_searcher.cpp
	g++ -g part1_searcher.cpp -o part1.out

run-part1: part1.out
	./part1.out file.txt AOD 0 67108863

build-part2: part2.out

part2.out: part2_partioner.cpp
	g++ -g part2_partioner.cpp -o part2_partioner.out

run-part2: part1.out part2_partioner.out
	./part2_partioner.out file.txt AOD 0 67108863 8388608

build-part3: part3_partioner.cpp part3_searcher.cpp
	g++ -g part3_searcher.cpp -o part3_searcher.out
	g++ -g part3_partioner.cpp -o part3_partioner.out

run-part3: part3_searcher.out part3_partioner.out ignore_signal.sh
	(trap '' SIGTERM; ./ignore_signal.sh)

clean-part1:
	rm part1.out

clean:
	rm *.out

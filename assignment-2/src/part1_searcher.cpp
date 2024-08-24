#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <signal.h>

using namespace std;

int solve(char* search_in, char* search_for, int search_space, int word_len){
	int i;
	for(i=0; i<=(search_space-word_len); i++){
		int j;
		for(j=0; j<(word_len); j++){
			if(search_in[i+j]!=search_for[j]){
				break;
			}
		}
		if(j==word_len){
			return i;
		}
	}
	return i;
}

int main(int argc, char **argv)
{
	if(argc != 5)
	{
		cout <<"usage: ./partitioner.out <path-to-file> <pattern> <search-start-position> <search-end-position>\nprovided arguments:\n";
		for(int i = 0; i < argc; i++)
			cout << argv[i] << "\n";
		return -1;
	}
	
	char *file_to_search_in = argv[1];
	char *pattern_to_search_for = argv[2];
	int search_start_position = atoi(argv[3]);
	int search_end_position = atoi(argv[4]);

	//TODO
	// start reading file
	ifstream file("file.txt");
	// allocate space 
	int search_space = search_end_position-search_start_position+1;
	char* input = new char[search_space];
	// read from file enter into allocate
	char ch;
	int count1 = 0;
	while(file.get(ch)){
		if(count1>search_end_position){
			break;
		}else if(count1>=search_start_position){
			input[count1-search_start_position] = ch;
		}
		count1++;
	}
	// pass the input, word and their lengths to check;
	int word_length = strlen(pattern_to_search_for);
	int output = solve(input, pattern_to_search_for, search_space, word_length);
	if(output!=search_space-word_length+1){
		cout << "[" << getpid() << "] found at " << output+search_start_position << endl;
		return 1;
	}
	cout << "[" << getpid() << "]" << "didn't find\n";
	return 0;
}

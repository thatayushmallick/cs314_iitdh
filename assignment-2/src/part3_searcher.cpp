#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

using namespace std;

// signal handler
void handle_sigterm(int sig){
  cout << "[" << getpid() << "] recieved SIGTERM" << endl;
  exit(0);
}

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
  // adding signal reader
  struct sigaction sa;

  // clear sigaction structure
  memset(&sa, 0, sizeof(sa));

  // set handler function
  sa.sa_handler = &handle_sigterm;

  // Use sigaction to set the new signal handler
  if (sigaction(SIGTERM, &sa, NULL) == -1) {
      perror("Error setting up signal handler");
      return 1;
  }
	//TODO
  // building ifstream
	ifstream file(file_to_search_in, std::ios::in);
  if(!file.is_open()){
    cerr << "Error can't open file: " << file_to_search_in << endl;
    return -1;
  }
  // setting up start and end position
  streampos start = search_start_position;
  streampos end = search_end_position;
  if(start>end){
    cerr << "Error: start should be small than end" << endl;
    return -1;
  }
  // size of search space
  streamsize length = end-start+1;
  // moving pointer to start position
  file.seekg(start);
  // reading the words upto length
  char* buffer = new char[length+1];
  file.read(buffer,length);
  // end of search space should be /0
  buffer[length] = '\0';
  // close file ptr
  file.close();
	// pass the input, word and their lengths to check;
	int word_length = strlen(pattern_to_search_for);
	int output = solve(buffer, pattern_to_search_for, length, word_length);
	if(output!=length-word_length+1){
		cout << "[" << getpid() << "] found at " << output+search_start_position << endl;
    killpg(getpgrp(),15);
		return 1;
	}
	cout << "[" << getpid() << "] didn't find\n";
	return 0;
}

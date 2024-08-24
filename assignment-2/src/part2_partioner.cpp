#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
	if(argc != 6)
	{
		cout <<"usage: ./partitioner.out <path-to-file> <pattern> <search-start-position> <search-end-position> <max-chunk-size>\nprovided arguments:\n";
		for(int i = 0; i < argc; i++)
			cout << argv[i] << "\n";
		return -1;
	}
	
	char *file_to_search_in = argv[1];
	char *pattern_to_search_for = argv[2];
	int search_start_position = atoi(argv[3]);
	int search_end_position = atoi(argv[4]);
	int max_chunk_size = atoi(argv[5]);
	
	//TODO
	pid_t my_pid = getpid();
	cout << "[" << my_pid << "] start position = " << search_start_position << " ; end position = " << search_end_position << "\n";
	int chunk_size = search_end_position-search_start_position+1;
	if(chunk_size>max_chunk_size){
		int mid = (search_start_position+search_end_position) / 2;
		pid_t lcpid, lw, rcpid, rw;
		int lwstatus, rwstatus;
		(lcpid = fork()) && (rcpid = fork());
		if(lcpid==0){
			cout << "[" << my_pid << "] forked left child " << getpid() << "\n";
			string str_mid = to_string(mid);
			const char* char_mid = str_mid.c_str();
			string str_start = to_string(search_start_position);
			const char* char_start = str_start.c_str();
			string str_size = to_string(max_chunk_size);
			const char* char_size = (str_size).c_str();
			const char* path = "./part2_partioner.out";
			char *const argv[] = {
				(char *)"./part2_partioner.out",
				(char *)file_to_search_in,
				(char *)pattern_to_search_for,
				(char *)char_start,
				(char *)char_mid,
				(char *)char_size,
				NULL
			};
			if(execv(path, argv) == -1){
				perror("execv failed\n");
			}
		}else if(rcpid == 0){
			cout << "[" << my_pid << "] forked right child " << getpid() << "\n";
			string str_mid = to_string(mid+1);
			const char* char_mid = str_mid.c_str();
			string str_end = to_string(search_end_position);
			const char* char_end = (str_end).c_str();
			string str_size = to_string(max_chunk_size);
			const char* char_size = (str_size).c_str();
			const char* path = "./part2_partioner.out";
			char* const argv[] = {
				(char *)"./part2_partioner.out",
				(char *)file_to_search_in,
				(char *)pattern_to_search_for,
				(char *)char_mid,
				(char *)char_end,
				(char *)char_size,
				NULL
			};
			if(execv(path, argv) == -1){
				perror("execv failed\n");
			}
		}else{
			lw = waitpid(lcpid,&lwstatus, WUNTRACED | WCONTINUED);
			cout << "[" << my_pid << "] left child returned\n";
			rw = waitpid(rcpid,&rwstatus, WUNTRACED | WCONTINUED);
			cout << "[" << my_pid << "] right child returned\n";
		}
	}else{
		// const char* path = "/usr/bin/make";
		// char *const argv[] = {
		// 	(char *)"make",
		// 	(char *)"build-part1",
		// 	NULL
		// };
		// if(execv(path, argv) == -1){
		// 	perror("execv failed\n");
		// }
		pid_t search_pid, search_w;
		int search_wstatus;
		search_pid = fork();
		if(search_pid == 0){
			cout << "[" << my_pid << "] forked searcher child " << getpid() << "\n";
			const char* exe_path = "./part1.out";
			char *const exe_argv[] = {
				(char *)"./part1.out",
				(char *)file_to_search_in,
				(char *)pattern_to_search_for,
				(char *)argv[3],
				(char *)argv[4],
				NULL
			};
			if(execv(exe_path, exe_argv) == -1){
				perror("execv failed\n");
			}
		}else{
			search_w = waitpid(search_pid,&search_wstatus, WUNTRACED);
			cout << "[" << my_pid << "] searcher child returned \n";
		}
	}
	//cout << "[" << my_pid << "] left child returned\n";
	//cout << "[" << my_pid << "] right child returned\n";*/
	//cout << "[" << my_pid << "] received SIGTERM\n"; //applicable for Part III of the assignmentpgid

	return 0;
}

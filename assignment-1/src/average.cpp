// this files only calculates average of time for each of 5 steps for 5 inputs.
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
int main(){
  ifstream input_file("input.txt");
  if(!input_file.is_open()){
    cerr << "ERR OPENING FILE!" << endl;
    return 1;
  }
  vector <double> v(5,0);
  string line;
  int i = 0;
  while(getline(input_file, line)){
    double val = stod(line);
    v[i%5] += val;
    i++;
  }
  i = (i+1)/5;
  for(auto j: v){
    j = j/(i);
    cout << j << "\n";
  }
  input_file.close();
  return 0;
}
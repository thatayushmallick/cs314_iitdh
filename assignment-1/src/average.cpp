// this files only calculates average of time for each of 5 steps for 5 inputs.
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
int main(){
// Read input.txt
  ifstream input_file("input.txt");
  if(!input_file.is_open()){
    cerr << "ERR OPENING FILE!" << endl;
    return 1;
  }
  // make a dynamic array to store the average time for each of 5 phases i.e read smooth detail sharpen write
  vector <double> v(5,0);
  string line;
  int i = 0;
  while(getline(input_file, line)){
    double val = stod(line); //convert string to double
    v[i%5] += val; //adds the value
    i++;
  }
  i = (i)/5; //number of iteration for which each experiment was iterated
  for(auto j: v){
    j = j/(i); //averaging each value by dividing sum by number of iteration
    cout << j << "\n"; //output the value for each phase
  }
  input_file.close();
  return 0;
}
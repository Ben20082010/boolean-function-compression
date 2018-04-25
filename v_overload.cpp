#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <bitset>
#include <chrono>


typedef std::vector<uint64_t> term;

term operator~(const term& v1){
  term out;
  out.reserve(v1.size());
  for(term::const_iterator it=v1.begin(); it!=v1.end(); ++it){
    out.push_back(~(*it));
  }

  return out;
}

// term operator~( term& v1){
//   for(int i=0;i<v1.size();i++){
//     v1[i]=~v1[i];
//   }
//   return v1;
// }


template <class T>
void printV(std::vector<T>& v, char end){
  for (int i=v.size()-1;i>=0;--i){
    std::cout << std::bitset<64>(v[i])<<" ";
  }
  std::cout << end;
}


int main(int argc, char const *argv[]) {
  term x=term(1,0);
  x.push_back(7);

  printV(x,'\n');
  ~x;
  printV(x,'\n');

  // printV(y,'\n');
  return 0;
}

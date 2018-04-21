#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <bitset>
#include <chrono>

typedef uint64_t ele;

using namespace std;

int main(int argc, char const *argv[]) {
  ele a=0;
  ele b=~a;

  cout << bitset<64>(b) << '\n';

  return 0;
}

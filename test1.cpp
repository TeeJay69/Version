#include <string>
#include <iostream>

using namespace std;

void removeSubstrs(string& s, string& p) { 
  string::size_type n = p.length();
  for (string::size_type i = s.find(p);
      i != string::npos;
      i = s.find(p))
      s.erase(i, n);
}

int main() {

  string str = "red tuna, blue tuna, black tuna, one tuna";
  string pattern = "tuna";

  removeSubstrs(str, pattern);
  cout << str << endl;
}
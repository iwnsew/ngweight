#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "cmdline.h"
#include "esa.hxx"
#include "wat_array.hpp"

using namespace std;

int getID(const string& str, map<string, int>& word2id){
  map<string, int>::const_iterator it = word2id.find(str);
  if (it == word2id.end()){
    int newID = (int)word2id.size();
    word2id[str] = newID;
    return newID;
  } else {
    return it->second;
  }
}

void printSnipet(const vector<int>& T, const int beg, const int len, const vector<string>& id2word){
  for (int i = 0; i < len; ++i){
    int c = T[beg + i];
    if (id2word.size() > 0){
      cout << id2word[c] << " ";
    } else {
      cout << (isspace((char)c) ? '_' : (char)c);
    }
  }
}

int main(int argc, char* argv[]){
  cmdline::parser p;
  p.add("word", 'w', "word type");

  if (!p.parse(argc, argv)){
    cerr << p.error() << endl
	 << p.usage() << endl;
    return -1;
  }

  if (p.rest().size() > 0){
    cerr << p.usage() << endl;
    return -1;
  }

  vector<int> T;
  vector<int> Doc;

  bool isWord = p.exist("word");
  map<string, int> word2id;
  istreambuf_iterator<char> isit(cin);
  istreambuf_iterator<char> end;

  size_t origLen = 0;
  int docid = 0;
  int flid = getID("\n", word2id);

  if (isWord){
    string word;
    while (isit != end){
      char c = *isit++;
      if (c == 0x02){
        c = *isit++;
        string s = "";
        while (c != 0x03) {
          s += c;
          c = *isit++;
        }
        docid = atoi(s.c_str());
        c = *isit++;
      }
      if (!isspace(c) && !ispunct(c)){
        word += tolower(c);
      } else if (word.size() > 0){
        T.push_back(getID(word, word2id));
        Doc.push_back(docid);
        word = "";
      }
      ++origLen;
    }
    if (word.size() > 0){
      T.push_back(getID(word, word2id));
      Doc.push_back(docid);
    }
  } else {
    cerr << "てやんでい！" << endl;
    return -1;
  }

  vector<string> id2word(word2id.size());
  for (map<string, int>::const_iterator it = word2id.begin();
       it != word2id.end(); ++it){
    id2word[it->second] = it->first;
  }

  vector<int> SA(T.size());
  vector<int> L (T.size());
  vector<int> R (T.size());
  vector<int> D (T.size());
  vector<int> Rn(T.size());
  vector<uint64_t> DA(T.size());
  int n = T.size();

  int k = (isWord) ? (int)id2word.size() : 0x100;
  if (isWord){
    cerr << "origN:" << origLen << endl;
  }
  cerr << "    n:" << n        << endl;
  cerr << "alpha:" << k        << endl;

  int nodeNum = 0;
  if (esaxx(T.begin(), SA.begin(), 
	    L.begin(), R.begin(), D.begin(), 
	    n, k, nodeNum) == -1){
    return -1;
  }
  cerr << " node:" << nodeNum << endl;

  int rank = 0;
  for (int i = 0; i < nodeNum; ++i){
    if (i==0 || T[(SA[i]+n-1)%n] != T[(SA[i-1]+n-1)%n]){
      rank++;
    }
    Rn[i] = rank;
  }

  for(int i = 0; i < n; ++i){
    DA[i] = Doc[SA[i]];
    //cout << DA[i] << endl;
  }
  wat_array::WatArray wa;
  wa.Init(DA);

  for (int i = 0; i < nodeNum; ++i){
    if (Rn[R[i]-1] - Rn[L[i]] > 0){
      cout << i << "\t" << R[i] - L[i] << "\t"  << D[i] << "\t";
      cout << wa.CountDistinct(L[i], R[i], 0, n, 0) << "\t";
      //cout << L[i] << "\t" << R[i] << "\t";
      printSnipet(T, SA[L[i]], D[i], id2word);
      cout << endl;
    }
  }

  return 0;
}

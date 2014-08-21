#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include "cmdline.h"
#include "esa.hxx"
#include "wat_array.hpp"

using namespace std;

int getID(const string& str, unordered_map<string, int>& word2id){
  unordered_map<string, int>::const_iterator it = word2id.find(str);
  if (it == word2id.end()){
    int newID = (int)word2id.size();
    word2id[str] = newID;
    return newID;
  } else {
    return it->second;
  }
}

void printSnipet(const vector<int>& T, const int beg, const int len,
                 const vector<string>& id2word){
  for (int i = 0; i < len; ++i){
    int c = T[beg + i];
    if (id2word.size() > 0){
      cout << id2word[c] << " ";
    } else {
      cout << (isspace((char)c) ? '_' : (char)c);
    }
  }
  return;
}

int main(int argc, char* argv[]){
  cmdline::parser p;
  p.add("word", 'w', "word type");
  p.add("threshold", 't',
        "min freq to reserve N-grams (for N>1)",
        false, 2);

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
  int threshold = p.get<int>("threshold");
  unordered_map<string, int> word2id;
  istreambuf_iterator<char> isit(cin);
  istreambuf_iterator<char> end;

  size_t origLen = 0;
  int docid = 0;
  int lfid = getID("\n", word2id);

  if (isWord){
    cerr << "Word mode:" << endl;
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
      } else{
        if (word.size() > 0){
          T.push_back(getID(word, word2id));
          Doc.push_back(docid);
          word = "";
        }
        if (isspace(c) && c != ' '){
          T.push_back(lfid);
          Doc.push_back(docid);
        }
      }
      ++origLen;
    }
    if (word.size() > 0){
      T.push_back(getID(word, word2id));
      Doc.push_back(docid);
    }
  } else {
    cerr << "Char mode: not supported yet! sorry!" << endl;
    return -1;
  }

  vector<string> id2word(word2id.size());
  for (unordered_map<string, int>::const_iterator it = word2id.begin();
       it != word2id.end(); ++it){
    id2word[it->second] = it->first;
  }

  int n = T.size();
  vector<int> SA(n);
  int k = (isWord) ? (int)id2word.size() : 0x100;
  if (isWord){
    cerr << "origN:" << origLen << endl;
  }
  cerr << "    n:" << n        << endl;
  cerr << "alpha:" << k        << endl;

  if (sais_xx(T.begin(), SA.begin(), n, k) == -1){
    return -1;
  }

  int preword = T[SA[0]];
  int prepos = 0;
  vector<uint64_t> DA(n);
  unordered_map<int, pair<int, int> > w2lr;
  for(int i = 1; i < n; ++i){
    DA[i] = Doc[SA[i]];
    int word = T[SA[i]];
    if (word != preword){
      w2lr[preword] = pair<int, int>(prepos, i);
      preword = word;
      prepos = i;
    }
  }
  vector<int>().swap(Doc);
  w2lr[preword] = pair<int, int>(prepos, n);
  wat_array::WatArray wa;
  wa.Init(DA);
  vector<uint64_t>().swap(DA);

  vector<int> L (n);
  vector<int> R (n);
  vector<int> D (n);

  int nodeNum = 0;
  if (esa_xx(T.begin(), SA.begin(), 
	    L.begin(), R.begin(), D.begin(), 
	    n, k, nodeNum) == -1){
    return -1;
  }
  cerr << " node:" << nodeNum << endl;

  vector<int> Rn(n);
  Rn[0] = 0;
  int rank = 0;
  for (int i = 1; i < n; ++i){
    if (T[(SA[i]-1+n)%n] != T[(SA[i-1]-1+n)%n]){
      rank++;
    }
    Rn[i] = rank;
  }
  cerr << "rank ok" << endl;

  for (int i = 0; i < nodeNum - 1; ++i){
    if (D[i] > 1 && R[i] - L[i] < threshold) continue;
    if (Rn[R[i]-1] - Rn[L[i]] > 0){
      bool skip = false;
      std::vector<uint64_t> beg_pos;
      std::vector<uint64_t> end_pos;
      for (int k = SA[L[i]]; k < SA[L[i]]+D[i]; ++k){
        if (T[k] == lfid){
          skip = true;
          break;
        }
        if ((double)(R[i] - L[i]) < log2(w2lr[T[k]].second - w2lr[T[k]].first)){
          skip = true;
          break;
        }
        beg_pos.push_back(w2lr[T[k]].first);
        end_pos.push_back(w2lr[T[k]].second);
      }
      if (skip) continue;
      printSnipet(T, SA[L[i]], D[i], id2word);
      int df = wa.Count(L[i], R[i], 0, n, 0);
      int udf = wa.Count(beg_pos, end_pos, 0, n, 0);
      cout << i << "\t" << D[i] << "\t" << R[i] - L[i] << "\t";
      cout << df << "\t" << udf << "\t";

      cout << endl;
    }
  }

  return 0;
}

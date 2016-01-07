#include <iostream>
#include <random>
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

string getTerm(const vector<int>& T, const int beg, const int len,
                 const vector<string>& id2word){
  string term = "";
  for (int i = 0; i < len; ++i){
    int c = T[beg + i];
    if (id2word.size() > 0){
      term += id2word[c] + " ";
    } else {
      term += (isspace((char)c) ? '_' : (char)c);
    }
  }
  return term;
}

int main(int argc, char* argv[]){
  cmdline::parser p;
  p.add("word", 'w', "word type");
  p.add("threshold", 't', "min freq to reserve N-grams (for N>1)", false, 2);
  p.add("sdfthreshold", 's', "max df to exactly count (set 0 for exact count)", false, 0);
  p.add("maxlength", 'l', "max length of N-grams", false, 10);
  p.add("rand", 'r', "initial value for random", false, 1);

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
  int sdfthreshold = p.get<int>("sdfthreshold");
  int maxlength = p.get<int>("maxlength");
  int randini = p.get<int>("rand");
  unordered_map<string, int> word2id;
  istreambuf_iterator<char> isit(cin);
  istreambuf_iterator<char> end;

  size_t origLen = 0;
  int docid = -1;
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
        docid++;
        //docid = atoi(s.c_str());
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
  docid++;

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
  cerr << "docid:" << docid    << endl;

  if (sais_xx(T.begin(), SA.begin(), n, k) == -1){
    return -1;
  }

  // Randomize Doc
  vector<int> Drand(docid);
  for (int i = 0; i < docid; ++i){
    Drand[i] = i;
  }
  mt19937 mt(randini);
  for (int i = docid-1; i > 0; --i){
    uniform_int_distribution<> rnd(0, i);
    int r = rnd(mt);
    int x = Drand[r];
    Drand[r] = Drand[i];
    Drand[i] = x;
  }

  int preword = T[SA[0]];
  int prepos = 0;
  vector<uint64_t> DA(n);
  unordered_map<int, pair<int, int> > w2lr;
  for(int i = 1; i < n; ++i){
    DA[i] = Drand[Doc[SA[i]]];
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
    if (SA[i]-1 < 0 || SA[i-1]-1 < 0){
      rank++;
    }
    else if (T[SA[i]-1] != T[SA[i-1]-1]){
      rank++;
    }
    Rn[i] = rank;
  }

  for (int i = nodeNum - 2; i >= 0; --i){
    if (D[i] > 1 && R[i] - L[i] < threshold) continue;
    if (D[i] > maxlength) continue;
    if (Rn[R[i]-1] - Rn[L[i]] > 0){
      int beg = SA[L[i]];
      int len = D[i];
      bool skip = false;
      int gtf = R[i] - L[i];
      for (int k = 0; k < len; ++k){
        if (T[beg+k] == lfid){
          skip = true;
          break;
        }
      }
      if (skip) continue;
      vector<uint64_t> beg_pos;
      vector<uint64_t> end_pos;
      vector<size_t> nums;
      unordered_map<int, int> word2num;
      for (int k = 0; k < len; ++k){
        unordered_map<int, int>::const_iterator it = word2num.find(T[beg+k]);
        if (it == word2num.end()){
          word2num[T[beg+k]] = 1;
        } else {
          word2num[T[beg+k]]++;
        }
      }
      for (unordered_map<int, int>::const_iterator it = word2num.begin();
           it != word2num.end(); ++it){
        beg_pos.push_back(w2lr[it->first].first);
        end_pos.push_back(w2lr[it->first].second);
        nums.push_back(it->second);
      }
      string term = getTerm(T, beg, len, id2word);
      int df = wa.Count(L[i], R[i], 0, n, 0);
      int sdf = 0;
      if (sdfthreshold <= 0){
        sdf = wa.Count(beg_pos, end_pos, nums, 0, n, 0);
      } else {
        sdf = wa.ApproxCount(beg_pos, end_pos, nums, 0, n, 0, sdfthreshold);
      }
      //double ngw = log2((double)docid/sdf);
      //double ngw2 = log2((double)docid*df/(sdf*sdf));
      cout << i << "\t" << len << "\t" << gtf << "\t" << df << "\t" << sdf << "\t" << term << endl;
    }
  }

  return 0;
}

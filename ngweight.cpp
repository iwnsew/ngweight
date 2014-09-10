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
  p.add("threshold", 't',
        "min freq to reserve N-grams (for N>1)",
        false, 2);
  p.add("maxlength", 'l', "max length of N-grams", false, 10);

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
  int maxlength = p.get<int>("maxlength");
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

  vector<int> preterm;
  vector<unordered_map<uint64_t, uint64_t> > constraint;
  for (int i = nodeNum - 2; i >= 0; --i){
    if (D[i] > 1 && R[i] - L[i] < threshold) continue;
    if (D[i] > maxlength) continue;
    if (Rn[R[i]-1] - Rn[L[i]] > 0){
      vector<uint64_t> beg_pos;
      vector<uint64_t> end_pos;
      int beg = SA[L[i]];
      int len = D[i];
      int prefix = 0;
      bool skip = true;
      int gtf = R[i] - L[i];
      //double pudf = (double)docid;
      for (int k = 0; k < len; ++k){
        if (T[beg+k] == lfid){
          skip = true;
          break;
        }
        //int gutf = w2lr[T[beg+k]].second - w2lr[T[beg+k]].first;
        //pudf *= (1 - exp(-(double)gutf/(double)docid));
        if ((double)(R[i] - L[i]) > double(w2lr[T[beg+k]].second - w2lr[T[beg+k]].first)/2000){
          skip = false;
        }
      }
      if (skip) continue;
      //double pdf = (double)docid * (1 - exp(-(double)gtf/(double)docid));
      //double med = log2(pudf/pdf);
      //double ngw = log2((double)docid*pdf/(pudf*pudf));
      //if ((double)docid*pdf/(pudf*pudf) < 2) continue;
      for (int k = 0; k < len; ++k){
        if (k < (int)preterm.size() && preterm[k] == T[beg+k]){
          prefix++;
        }else{
          for (int l = preterm.size()-1; l >= k; --l){
            preterm.pop_back();
            constraint.pop_back();
          }
          beg_pos.push_back(w2lr[T[beg+k]].first);
          end_pos.push_back(w2lr[T[beg+k]].second);
          preterm.push_back(T[beg+k]);
          unordered_map<uint64_t, uint64_t> cnstrnt;
          constraint.push_back(cnstrnt);
        }
      }
      string term = getTerm(T, beg, len, id2word);
      int df = wa.Count(L[i], R[i], 0, n, 0);
      int udf = 0;
      if (prefix > 0){
        if (constraint[prefix-1].size() == 0){
          vector<uint64_t> beg_pos2;
          vector<uint64_t> end_pos2;
          for (int k = 0; k < prefix; ++k){
            beg_pos2.push_back(w2lr[T[beg+k]].first);
            end_pos2.push_back(w2lr[T[beg+k]].second);
          }
          wa.Count(constraint[prefix-1], beg_pos2, end_pos2, 0, n, 0);
        }
        udf = wa.Count(constraint[len-1], constraint[prefix-1], beg_pos, end_pos, 0, n, 0);
      }else{
        udf = wa.Count(constraint[len-1], beg_pos, end_pos, 0, n, 0);
      }
      //double ngw = log2((double)docid*df/(udf*udf));
      cout << i << "\t" << len << "\t" << gtf << "\t" << df << "\t" << udf << "\t" << term << endl;
    }
  }

  return 0;
}

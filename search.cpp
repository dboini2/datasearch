#include "include/search.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string cleanToken(const string& token) {
  string raw = token;
  bool ret = false;
  string clean = "";
  unsigned short int f = 65535;
  unsigned short int l = 0;
  for (int i = 0; i < raw.size(); i++) {
    if (isalpha(raw[i])) {
      ret = 1;
      raw[i] = tolower(raw[i]);
      if (i < f) {
        f = i;
      }
      if (l < i) {
        l = i;
      }
    } else if (!ispunct(raw[i])) {
      if (i < f) {
        f = i;
      }
      if (l < i) {
        l = i;
      }
    }
  }
  // cout << f << " lbound " << l << " ubound " << raw << endl;
  if (ret == false) {
    return "";
  } else {
    for (int i = 0; i < raw.size(); i++) {
      if (i < f && !ispunct(raw[i])) {
        clean += raw[i];
      } else if (i > l && !ispunct(raw[i])) {
        clean += raw[i];
      } else if (i >= f && i <= l) {
        clean += raw[i];
      }
    }
    return clean;
  }
}

set<string> gatherTokens(const string& text) {
  set<string> ret;
  string tmp;
  stringstream s(text);
  while (s >> tmp) {
    tmp = cleanToken(tmp);
    if (tmp != "") {
      ret.emplace(tmp);
    }
  }
  return ret;
}

int buildIndex(const string& filename, map<string, set<string>>& index) {
  map<string, set<string>> te;
  int count = 0;
  ifstream file(filename);
  if (file.fail()) {
    return 0;
  }
  string site;

  string temp;
  while (getline(file, site)) {
    getline(file, temp);
    set<string> key;
    key = gatherTokens(temp);
    for (auto i : key) {
      te[i].insert(site);
    }
    count++;
  }
  file.close();
  index = te;
  return count;
}

set<string> findQueryMatches(const map<string, set<string>>& index,
                             const string& sentence) {
  stringstream s(sentence);
  string tmp;
  vector<string> ab;
  while (s >> tmp) {
    // cout << tmp << endl;
    ab.push_back(tmp);
  }
  set<string> result;
  for (int i = 0; i < ab.size(); i++) {
    tmp = ab.at(i);
    auto s = index.find(cleanToken(tmp));
    if (s != index.end()) {
      // cout << "wordL " << tmp << endl;
      set<string> a = s->second;

      if (isalpha(tmp[0])) {
        set<string> temp = result;
        set_union(a.begin(), a.end(), temp.begin(), temp.end(),
                  inserter(result, result.begin()));
        // for (auto i : result) {
        //   cout << "union cont: " << i << endl;
        // }
      } else if (tmp[0] == '-') {
        set<string> temp = result;
        set<string> mt;
        set_difference(temp.begin(), temp.end(), a.begin(), a.end(),
                       inserter(mt, mt.begin()));
        result = mt;
        // for (auto i : result) {
        //   cout << "diff cont: " << i << endl;
        // }
      } else if (tmp[0] == '+') {
        set<string> temp = result;
        set<string> mt;
        set_intersection(temp.begin(), temp.end(), a.begin(), a.end(),
                         inserter(mt, mt.begin()));
        result = mt;
        // for (auto i : result) {
        //   cout << "inter cont: " << i << endl;
        // }
      }

    } else if (tmp[0] == '+') {
      set<string> a;
      result = a;
    }
  }

  return result;
}

void searchEngine(const string& filename) {
  map<string, set<string>> index;
  int count = buildIndex(filename, index);
  if (count == 0) {
    cout << "Invalid filename." << endl;
  }
  string text = " ";
  cout << "Stand by while building index..." << endl;
  cout << "Indexed " << count << " pages containing " << index.size()
       << " unique terms" << endl;
  while (text != "") {
    cout << "Enter query sentence (press enter to quit): ";
    getline(cin, text);
    if (text == "") {
      cout << "Thank you for searching!" << endl;
      break;
    }
    set<string> res = findQueryMatches(index, text);
    // int i = 0;
    cout << "Found " << res.size() << " matching pages" << endl;
    for (auto a : res) {
      cout << a << endl;
    }
  }
}

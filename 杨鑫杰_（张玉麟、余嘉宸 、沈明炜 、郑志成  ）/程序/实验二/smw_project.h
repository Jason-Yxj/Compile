#include<bits/stdc++.h> 
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
#define rep(i,a,n) for(int i=a;i<n;i++)
#define per(i,a,n) for(int i=n-1;i>=a;i--)
#define all(x) (x).begin(),(x).end()
typedef pair<string, vector<string>> Production;

#ifndef project2
#define project2
struct Project {        //项目集
    string left;
    vector<string> right;
    set<string> expect;

    const bool operator<(const Project &p) const {
        if (left < p.left) return true;
        if (left > p.left) return false;
        if (right < p.right) return true;
        if (right > p.right) return false;
        if (expect < p.expect) return true;
        return false;
    }

    const bool operator==(const Project &p) const {
        if (left == p.left && right == p.right && expect == p.expect) return true;
        return false;
    }
};

/*产生项目集族*/
namespace smw_project {
    /*存储所有的符号*/
    set<string> all_symbols;    
	/*存放所有的符号*/
    vector<string> vec_symbols;
	/*符号哈希*/
    map<string, int> Hash;      
    /*存储终结符*/
    set<string> terminal_symbol;    
	/*存储非终结符*/
    set<string> nonterminal_symbol;              
    /*存放产生式，加·前  */ 
    vector<Production> vec_production;    
	/*存储项目集，加·后 */ 
    set<Production> set_production;

    void smw_project() {
	    string str1, str2;
        vector<string> vec;
        /*读入Grammar.txt文件*/ 
        ifstream fin("zzc_Language.txt");
        while (fin >> str1 >> str2) {
            nonterminal_symbol.insert(str1);
            string str3;
            getline(fin, str3);
            stringstream str_stream;
            str_stream.str(str3);
            vec.clear();
            while (str_stream >> str3) {
                vec.push_back(str3);
                int flag = 0;
                rep(i, 0, str3.size()) {
				    /*非终结符均由小写字母和下划线组成*/ 
                    if (!(str3[i] >= 'a' && str3[i] <= 'z' || str3[i] == '_')) {
                        flag = 1;
                        break;
                    }
                }
				/*除了小写字母和下划线以外的符号均为终结符*/ 
                if (flag) terminal_symbol.insert(str3);
                /*非终结符*/ 
                else nonterminal_symbol.insert(str3);    
            }
            vec_production.push_back(Production(str1, vec));
        }
        terminal_symbol.insert("#");
        /*删除program*/
        nonterminal_symbol.erase("program'");        
        all_symbols.insert(all(terminal_symbol));
        all_symbols.insert(all(nonterminal_symbol));
        for (auto it : terminal_symbol) vec_symbols.push_back(it);
        /*通过vec_symbols输出lr1表的表头*/ 
        for (auto it : nonterminal_symbol) vec_symbols.push_back(it);
        rep(i, 0, vec_symbols.size()) Hash[vec_symbols[i]] = i;
        for (auto it : vec_production) {
            str1 = it.first, vec = it.second;
            rep(i, 0, vec.size() + 1) {
                vector<string> d = vec;
                d.insert(d.begin() + i, ".");
                set_production.insert(Production(str1, d));
            }
        }
    }
    

    void main() {
        smw_project();
    }
}
#endif

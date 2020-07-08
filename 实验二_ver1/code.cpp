#include <map>
#include <set>
#include <list>
#include <ctime>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
using namespace std;
#define rep(i,a,n) for(int i=a;i<n;i++)
#define per(i,a,n) for(int i=n-1;i>=a;i--)
#define all(x) (x).begin(),(x).end()
typedef pair<string, vector<string>> Production;        //产生式
const int WIDTH = 16;    //setw(WIDTH)
// head

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

namespace project {    //产生项目集族
    set<string> terminal;        //终结符
    set<string> not_terminal;    //非终结符
    set<string> all_symbol;        //所有的符号
    vector<string> sss;            //所有的符号
    map<string, int> Hash;        //符号哈希
    vector<Production> vp;        //所有的产生式，加·前
    set<Production> sp;            //所有的项目集，加·后

    void PROJECT() {
        ifstream fin("Grammar.txt");
        string a, b;
        vector<string> c;
        while (fin >> a >> b) {
            not_terminal.insert(a);
            string str;
            getline(fin, str);
            stringstream ss;
            ss.str(str);
            c.clear();
            while (ss >> str) {
                c.push_back(str);
                int fg = 0;
                rep(i, 0, str.size()) {
                    if (!(str[i] >= 'a' && str[i] <= 'z' || str[i] == '_')) {    //文法中非终结符都是小写字母和下划线组成的
                        fg = 1;
                        break;
                    }
                }
                if (fg) terminal.insert(str);    //如果含有小写字母和下划线以外的符号就是终结符
                else not_terminal.insert(str);    //否则是非终结符
            }
            vp.push_back(Production(a, c));
        }
        terminal.insert("#");
        not_terminal.erase("program'");        //删掉program'
        all_symbol.insert(all(terminal));
        all_symbol.insert(all(not_terminal));
        for (auto it : terminal) sss.push_back(it);
        for (auto it : not_terminal) sss.push_back(it);    //sss其实是为了输出lr1表的表头
        rep(i, 0, sss.size()) Hash[sss[i]] = i;
        for (auto it : vp) {
            a = it.first, c = it.second;
            rep(i, 0, c.size() + 1) {
                vector<string> d = c;
                d.insert(d.begin() + i, ".");
                sp.insert(Production(a, d));
            }
        }
    }

    void main() {
        PROJECT();
    }
}

namespace lr1 {    //生成lr表
    using namespace project;

    vector<set<Project>> vsp(1);    //项目集族
    string lr1[1000][1000];        //lr1表
    int n, m;                    //n行，m列

    set<string> FIRST(vector<string> X) {    //求FIRST集族
        set<string> res;
        if (terminal.find(X[0]) != terminal.end()) {
            res.insert(X[0]);    //如果是终结符，直接insert返回
            return res;
        }
        else {
            rep(j, 0, vp.size()) {    //遍历所有的产生式
                if (vp[j].first == X[0]) {
                    if (terminal.find(vp[j].second[0]) != terminal.end())    //如果第一个是终结符
                        res.insert(vp[j].second[0]);                            //插入到res中
                    else {
                        set<string> t = FIRST(vp[j].second);                    //否则递归求FIRST集
                        res.insert(all(t));
                    }
                }
            }
        }
        return res;
    }

    set<Project> GO(set<Project> I, string X) {    //GO函数
        set<Project> J;
        for (auto it : I) {
            vector<string> vs = it.right;
            auto pos = find(all(vs), ".");
            if (pos == vs.end() - 1) continue;    //如果·是最后一个，continue
            if (*(pos + 1) == X) {
                swap(*pos, *(pos + 1));            //交换·和后面的一个字符串
                J.insert(Project{ it.left, vs, it.expect });
            }
        }
        return J;
    }

    set<Project> CLOSURE(set<Project> I) {        //求closure
        while (1) {
            bool update = false;                    //判断此次循环是否有更新
            for (auto it : I) {
                vector<string> B = it.right;
                auto pos = find(all(B), ".");        //找到·的位置
                if (pos == B.end() - 1) continue;    //如果·是最后一个，continue
                string c = *(pos + 1);                //c等于·后面的字符
                if (terminal.find(c) != terminal.end()) continue;    //如果c是终结符，continue
                B.erase(B.begin(), pos + 2);            //删掉·后面的一个字符之前的所有字符，包括它自己
                string last;                            //为了剪枝，记录上一次求FIRST集的第一个字符串
                for (auto ite : it.expect) {
                    B.push_back(ite);                //把expect插入到B的后面
                    if (last == B[0]) continue;        //如果B[0]和上次的last一样，就不求了，因为文法中没有空产生式
                    else last = B[0];
                    set<string> First = FIRST(B);    //求B的FIRST集
                    B.pop_back();
                    for (auto it1 : sp) {
                        if (it1.first == c && it1.second[0] == ".") {    //找到·开头的
                            set<string> ss;                                //ss为空，为了二分查找
                            Project p{ it1.first,it1.second,ss };
                            auto pos = I.lower_bound(p);                    //查找第一个大于等于p的位置
                            if (pos != I.end() && (*pos).left == it1.first && (*pos).right == it1.second) {    //如果pos指向的first，second符合
                                p = *pos;
                                I.erase(p);                        //因为不能直接插入，所以需要先删除在插入
                                p.expect.insert(all(First));
                                I.insert(p);
                            }
                            else {
                                update = true;        //插入了新的一项，所以update为true。经过测试发现只更新expext可以不用设置update为true
                                I.insert(Project{ it1.first, it1.second, First });
                            }
                        }
                    }
                }
            }
            if (!update) break;    //如果没更新，跳出循环
        }
        return I;
    }

    void LR1() {
        for (auto it : sp) {        //先插入第一条program' -> program
            if (it.first == "program'" && it.second[0] == ".") {
                set<string> ss;
                ss.insert("#");
                vsp[0].insert(Project{ it.first,it.second, ss });
                break;
            }
        }
        vsp[0] = CLOSURE(vsp[0]);    //然后求第一个项目集的闭包
        rep(i, 0, vsp.size()) {
            time_t t = time(0);        //为了在控制台显示每个项目集计算的时间，debug
            char tmp[64];
            strftime(tmp, sizeof(tmp), ": %X", localtime(&t));
            cout << i;
            puts(tmp);
            //规约
            for (auto it : vsp[i]) {
                int len = it.right.size();
                if (it.right[len - 1] == ".") {        //如果最后一个字符串是·，就可以规约了
                    it.right.erase(it.right.end() - 1);        //删除那个·
                    Production p(it.left, it.right);            //构造产生式p
                    rep(j, 0, vp.size()) {
                        if (vp[j] == p) {                    //此次循环是为了找到产生式p的编号为j
                            string t = "r" + to_string(j);    //lr1表
                            if (j == 0) t = "acc";            //如果是第0条，应该是acc
                            for (auto its : it.expect)
                                lr1[i][Hash[its]] = t;        //填lr1表
                        }
                    }
                }
            }

            for (auto X : all_symbol) {                //遍历所有的符号
                set<Project> J = GO(vsp[i], X);        //J = GO(I, X)
                if (!J.empty()) {                    //如果J不为空
                    J = CLOSURE(J);                    //求J的闭包
                    int k;
                    auto pos = find(all(vsp), J);
                    if (pos != vsp.end()) {            //如果vsp中已经存在了这个项目集族
                        k = pos - vsp.begin();        //记下存在的位置
                    }
                    else {
                        k = vsp.size();                //不存在就插入，标号k就是push之前的size
                        vsp.push_back(J);
                    }

                    //移进和GOTO
                    int j = Hash[X];
                    if (terminal.find(X) != terminal.end())    //如果X为终结符
                        lr1[i][j] = "s" + to_string(k);        //就移进
                    else lr1[i][j] = to_string(k);            //否则goto
                }
            }
        }
    }

    void PRINT() {    //此函数为了输出项目集族和lr表到文件中
        ofstream fout("output.out");
        cout<<"运行到这里吗"<<endl; 
        fout << "      CLOSURE" << endl;
        rep(i, 0, 20) fout << '-';
        fout << endl;
        rep(i, 0, vsp.size()) {
            fout << i << ":" << endl;
            set<Project> sp = vsp[i];
            for (auto it : sp) {
                string b;
                rep(i, 0, it.right.size()) b += ' ' + it.right[i];
                b.erase(0, 1);
                string c;
                for (auto it2 : it.expect) c += it2 + '/';
                c.pop_back();
                fout << "    " << it.left << " -> "
                    << b << ", " << c << endl;
            }
            rep(i, 0, 20) fout << '-';
            fout << endl;
        }
        fout << endl << endl << endl;

        rep(i, 0, 3) fout << setw(WIDTH) << ' ';
        fout << "LR(1)分析表" << endl;
        fout << '|' << endl << '|';
        fout << setw(WIDTH) << "状态";
        fout << '|';
        n = vsp.size();
        m = sss.size();
        rep(i, 0, m) {
            fout << setw(WIDTH) << sss[i];
            if (i == terminal.size() - 1) fout << '|';
        }
        fout << '|' << endl << '|';
        rep(i, 0, 200) fout << '-';
        fout << '|' << endl << '|';
        rep(i, 0, n) {
            fout << setw(WIDTH) << i;
            fout << '|';
            rep(j, 0, m) {
                fout << setw(WIDTH) << lr1[i][j];
                if (j == terminal.size() - 1) fout << '|';
            }
            fout << '|' << endl << '|';
        }
        rep(i, 0, 200) fout << '-';
        fout << '|';
        fout << endl << endl << endl;
    }

    void OUT_LR1() {        //输出lr1表到文件中
        ofstream fout("LR(1).txt");
        fout << n << " " << m << endl;
        rep(i, 0, n) {
            rep(j, 0, m) {
                if (lr1[i][j] == "") fout << "err ";        //如果为空，输出err
                else fout << lr1[i][j] << ' ';
            }
            fout << endl;
        }
    }

    void main() {
        project::main();
        LR1();
        PRINT();
        OUT_LR1();
    }
}

namespace grammar {
    using namespace project;
    struct E {            //E，记录一下信息
        string place;    //位置
        vector<pair<int, vector<string>>> code;    //四元式
        string name;        //变量的name
        string value;    //常量的value
        int Begin;
        int Next;
        int True;
        int False;;
    };

    string lr1[1000][1000];
    int n, m;
    int cnt = 0;                //Tree的节点数
    vector<int> G[1000];        //用邻接表存Tree
    vector<string> symbol;    //每个节点对应的符号名字
    vector<E> attr;            //每个节点对用的属性，类型是E

    void IN_LR1() {    //读入lr表
        ifstream fin("LR(1).txt");
        fin >> n >> m;
        rep(i, 0, n) rep(j, 0, m) fin >> lr1[i][j];
    }

    void MORPHOLOGY() {    //词法分析器
        ifstream fin("Program.txt");
        ofstream fout("Token.txt");
        string keyword[27] = { "main","if","else","int","return","void","while",
            "=","+","-","*","/","<","<=",">",">=","!=","==",",",";",":","{","}","[","]","(",")" };
        map<string, int> H;
        rep(i, 0, 27) H[keyword[i]] = i + 1;
        string s;
        while (fin >> s) {
            rep(i, 0, s.length()) {
                if (isalpha(s[i])) {
                    if (s[i] == 'm' && s[i + 1] == 'a' && s[i + 2] == 'i'
                        && s[i + 3] == 'n' && !isalnum(s[i + 4])) {
                        fout << "ID" << endl;
                        i += 3;
                    }
                    else if (s[i] == 'i' && s[i + 1] == 'f' && !isalnum(s[i + 2])) {
                        fout << "IF" << endl;
                        i += 1;
                    }
                    else if (s[i] == 'e' && s[i + 1] == 'l' && s[i + 2] == 's'
                        && s[i + 3] == 'e' && !isalnum(s[i + 4])) {
                        fout << "ELSE" << endl;
                        i += 3;
                    }
                    else if (s[i] == 'i' && s[i + 1] == 'n' && s[i + 2] == 't'
                        && !isalnum(s[i + 3])) {
                        fout << "INT" << endl;
                        i += 2;
                    }
                    else if (s[i] == 'r' && s[i + 1] == 'e' && s[i + 2] == 't' && s[i + 3] == 'u'
                        && s[i + 4] == 'r' && s[i + 5] == 'n' && !isalnum(s[i + 6])) {
                        fout << "RETURN" << endl;
                        i += 5;
                    }
                    else if (s[i] == 'v' && s[i + 1] == 'o' && s[i + 2] == 'i'
                        && s[i + 3] == 'd' && !isalnum(s[i + 4])) {
                        fout << "VOID" << endl;
                        i += 3;
                    }
                    else if (s[i] == 'w' && s[i + 1] == 'h' && s[i + 2] == 'i'
                        && s[i + 3] == 'l' && s[i + 4] == 'e' && !isalnum(s[i + 5])) {
                        fout << "WHILE" << endl;
                        i += 4;
                    }
                    else {
                        string token;
                        while (isalnum(s[i])) token += s[i++];
                        i--;
                        fout << "ID" << ' ' << token << endl;
                    }
                }
                else if (isdigit(s[i])) {
                    int num = 0;
                    while (isdigit(s[i])) num = num * 10 + s[i++] - '0';
                    i--;
                    fout << "NUM" << ' ' << num << endl;
                }
                else {
                    if (s[i] == '!') {
                        fout << "!=" << endl;
                        i++;
                    }
                    else if (s[i] == '=' && s[i + 1] == '=') {
                        fout << "==" << endl;
                        i++;
                    }
                    else if (s[i] == '<' && s[i + 1] == '=') {
                        fout << "<=" << endl;
                        i++;
                    }
                    else if (s[i] == '>' && s[i + 1] == '=') {
                        fout << ">=" << endl;
                        i++;
                    }
                    else {
                        string t;
                        t += s[i];
                        fout << t << endl;
                    }
                }
            }
        }
    }

    void JUDGE() {    //生成语法树
        ifstream fin("Token.txt");
        ofstream fout("Analysis.txt");

        vector<pair<string, string>> vs;        //存Token
        pair<string, string> str;
        while (fin >> str.first) {
            if (str.first == "NUM" || str.first == "ID")
                fin >> str.second;    //如果是NUM和ID还要读入name或value
            vs.push_back(str);
        }

        int a[1000], top = 1;    //a位状态栈，top为栈顶指针
        a[0] = 0;                                        //初始状态
        E e;                                                //初始状态
        vector<pair<string, E>> b{ make_pair("#",e) };    //初始状态
        vector<pair<string, string>> c = vs;                //初始状态
        c.push_back(make_pair("#", ""));                    //初始状态

        fout << left << setw(WIDTH / 2) << "";
        fout << left << setw(WIDTH * 4) << "状态";
        fout << left << setw(WIDTH * 10) << "符号";
        fout << right << setw(WIDTH * 7) << "输入串" << endl;

        int id = 1;    //id为行数
        stack<int> S;    //S同步每一次操作，为了生成Tree
        while (1) {
            string temp;
            rep(i, 0, top) temp += to_string(a[i]) + ' ';
            fout << left << setw(WIDTH / 2) << id++;
            fout << left << setw(WIDTH * 4) << temp;
            string sb, sc;
            for (auto it : b) sb += ' ' + it.first;
            sb.erase(0, 1);
            for (auto it : c) sc += ' ' + it.first;
            sc.erase(0, 1);
            fout << left << setw(WIDTH * 10) << sb;
            fout << right << setw(WIDTH * 7) << sc << endl;    //以上都是为了输出
			cout<<"a[top-1]"<<a[top - 1]<<endl;
            string action = lr1[a[top - 1]][Hash[c[0].first]];    //查lr1表
            cout<<"action值："<<action<<endl; 
            if (action == "acc") break;                            //如果是acc表示结束，break
            if (action == "err") {                                //如果是err表示出错，直接return
                cerr << "error" << endl;
                return;
            }
            if (action[0] == 's') {                                //如果action第一个字符为s，表示移进
                action.erase(0, 1);                                //删掉第一个字符
                int num = atoi(action.c_str());                    //转成数字
                a[top++] = num;                                    //push进栈a
                E e;
                if (c[0].first == "ID") e.name = c[0].second;    //如果是ID，记下name
                else if (c[0].first == "NUM") e.value = c[0].second;    //如果是NUM，记下value
                b.push_back(make_pair(c[0].first, e));            //push进栈b
                symbol.push_back(c[0].first);                    //push进栈symbol
                attr.push_back(e);                                //push进栈attr
                S.push(cnt++);                                    //S中push的当前的节点号
                c.erase(c.begin());                                //pop出栈c
            }
            else {
                action.erase(0, 1);
                int num = atoi(action.c_str());
                cout<<"规约的数字为："<<num<<endl;
                Production p = vp[num];                    //用第num条产生式规约
                int len = p.second.size();                //second的长度
                queue<int> son;                            //son暂时记录栈S中pop的节点
                while (len--) {
                    top--;                                //pop栈a
                    b.pop_back();                        //pop栈b
                    son.push(S.top());                    //son先记录下S需要pop的节点
                    S.pop();                                //pop栈S
                }
                b.push_back(make_pair(p.first, e));        //push栈b
                symbol.push_back(p.first);                //push栈symbol
                cout<<"p.first:"<<p.first<<endl;
                attr.push_back(e);                        //push栈attr
                S.push(cnt++);                            //push栈S
                while (!son.empty()) {
                    G[cnt - 1].push_back(son.front());    //构造Tree
                    son.pop();
                }
                //yjc
                //a[top++] = atoi(lr1[a[top-1]][Hash[p.first]].c_str());    //push栈a
                cout<<"a[top-1]:"<<a[top-1]<<endl;
                cout<<"Hash[p.first]:"<<Hash[p.first]<<endl;
                cout<<"lr1[a[top-1]][Hash[p.first]]:"<<lr1[a[top-1]][Hash[p.first]]<<endl;
                cout<<"atoi(lr1[a[top-1]][Hash[p.first]].c_str()):"<<atoi(lr1[a[top-1]][Hash[p.first]].c_str())<<endl;
                int preAtop = atoi(lr1[a[top-1]][Hash[p.first]].c_str());    //push栈a
                //居然被我改出来了！！！！！ 
                a[top++] = preAtop;
                cout<<"a[top-1]:"<<a[top-1]<<endl;
            }
        }
        fout << endl << endl;

        fout << "语法树" << endl;    //输出语法树
        per(i, 1, cnt) if (!G[i].empty()) {
            fout << '(' << i << ',' << symbol[i] << ')' << "  ---->  ";
            sort(all(G[i]));
            rep(j, 0, G[i].size()) fout << '(' << G[i][j] << ',' << symbol[G[i][j]] << ')' << ' ';
            fout << endl;
        }
        fout << endl << endl;
    }

    void main() {
        project::main();
        IN_LR1();
        MORPHOLOGY();
        JUDGE();
    }
}

namespace translate {    //翻译
    using namespace grammar;
    ofstream fout("Code.txt");

    vector<vector<string>> table;    //符号表
    int temp_cnt = 0;                //变量计数
    int nextquad = 100;                //四元式标号

    void mktable() {                    //新建符号表
        table.push_back(vector<string>());
    }

    void rmtable() {                    //删除符号表
        table.pop_back();
    }

    void enter(string name) {        //声明变量
        table.back().push_back(name);
    }

    string lookup(string name) {        //查看变量是否存在
        per(i, 0, table.size()) rep(j, 0, table[i].size())
            if (table[i][j] == name) return name;
        return "nil";
    }

    string newtemp() {                //新建一个变量
        return "T" + to_string(++temp_cnt);
    }

    pair<int, vector<string>> gen(string a, string b, string c, string d) {    //生成四元式
        vector<string> vs{ a,b,c,d };
        return make_pair(nextquad++, vs);
    }

    void dfs(int u) {    //遍历语法树
        if (G[u].empty()) return;                        //如果为空直接return
        if (symbol[G[u].front()] == "{") mktable();        //如果第一个儿子为{，新建符号表
        rep(i, 0, G[u].size()) dfs(G[u][i]);                //遍历所有的儿子节点

        E &e = attr[u];                                    //e为attr[u]的引用，便于直接修改
        e = attr[G[u][0]];                                //简写，把第一个儿子的直接赋值给attr[u]
        attr[u].code.clear();                            //先清空code四元式
        rep(i, 0, G[u].size()) {                            //把所有儿子的四元式加进来
            for (auto it : attr[G[u][i]].code) {
                e.code.push_back(it);
            }
        }

        if (symbol[u] == "variable_definition") {//函数定义
            string name = attr[G[u][1]].name;
            enter(name);
        }
        else if (symbol[u] == "assignment_expression" && symbol[G[u][0]] == "ID") {//赋值语句
            string p = lookup(attr[G[u][0]].name);    //查符号表
            if (p == "nil") {
                cerr << "变量未声明" << endl;
                return;
            }
            e.place = p;
            e.code.push_back(gen("=", attr[G[u][2]].place, "-", e.place));
        }
        else if (symbol[u] == "primary_expression" && symbol[G[u][0]] == "NUM") {//规约NUM
            e.place = newtemp();
            e.value = attr[G[u][0]].value;
            e.code.push_back(gen("=", e.value, "-", e.place));
        }
        else if (symbol[u] == "primary_expression" && symbol[G[u][0]] == "ID") {    //规约ID
            string p = lookup(attr[G[u][0]].name);
            if (p == "nil") {
                cerr << "变量未声明" << endl;
                return;
            }
            e.place = p;
            e.name = attr[G[u][0]].name;
        }
        else if (symbol[u] == "multiplicative_expression" && G[u].size() > 1 && symbol[G[u][1]] == "*") {//乘法表达式
            E e1 = attr[G[u][0]];
            E e2 = attr[G[u][2]];
            e.place = newtemp();
            e.code.push_back(gen("*", e1.place, e2.place, e.place));
        }
        else if (symbol[u] == "additive_expression" && G[u].size() > 1 && symbol[G[u][1]] == "+") {//加法表达式
            E e1 = attr[G[u][0]];
            E e2 = attr[G[u][2]];
            e.place = newtemp();
            e.code.push_back(gen("+", e1.place, e2.place, e.place));
        }
        else if (symbol[u] == "relational_expression" && G[u].size() > 1) {//关系表达式
            E id1 = attr[G[u][0]];
            E id2 = attr[G[u][2]];
            e.code.push_back(gen("j" + symbol[G[u][1]], id1.place, id2.place, "0"));
            e.True = e.code.back().first;
            e.code.back().second[3] = to_string(e.code.back().first + 2);
            e.code.push_back(gen("j", "-", "-", "0"));
            e.False = e.code.back().first;
        }
        else if (symbol[G[u][0]] == "WHILE") {//WHILE语句
            e = attr[G[u][0]];
            attr[u].code.clear();
            for (auto it : attr[G[u][2]].code)
                e.code.push_back(it);
            for (auto it : attr[G[u][4]].code)
                e.code.push_back(it);
            e.code.push_back(gen("j", "-", "-", to_string(e.code.front().first)));

            for (auto &it : e.code) {
                if (it.first == attr[G[u][2]].False) {
                    it.second[3] = to_string(e.code.back().first + 1);
                    break;
                }
            }
            e.code.back().second[3] = to_string(e.code.front().first);
        }
        else if (symbol[G[u][0]] == "IF" && G[u].size() == 5) {//IF语句
            e = attr[G[u][0]];
            attr[u].code.clear();
            for (auto it : attr[G[u][2]].code)
                e.code.push_back(it);
            for (auto it : attr[G[u][4]].code)
                e.code.push_back(it);
            for (auto &it : e.code) {
                if (it.first == attr[G[u][2]].False) {
                    it.second[3] = to_string(e.code.back().first + 1);
                    break;
                }
            }
        }
        else if (symbol[G[u][0]] == "IF" && G[u].size() == 7) {//IF ELSE语句
            e = attr[G[u][0]];
            attr[u].code.clear();
            for (auto it : attr[G[u][2]].code)
                e.code.push_back(it);
            for (auto it : attr[G[u][4]].code)
                e.code.push_back(it);
            e.code.push_back(gen("j", "-", "-", to_string(e.code.back().first + attr[G[u][6]].code.size() + 2)));
            e.code.back().first = e.code.front().first + e.code.size() - 1;
            for (auto &it : e.code) {
                if (it.first == attr[G[u][2]].False) {
                    it.second[3] = to_string(e.code.back().first + 1);
                    break;
                }
            }
            for (auto it : attr[G[u][6]].code) {
                it.first++;
                if (it.second[0][0] == 'j') {
                    int num = atoi(it.second[3].c_str());
                    it.second[3] = to_string(num + 1);
                }
                e.code.push_back(it);
            }
        }

        if (symbol[G[u].back()] == "}") rmtable();        //如果最后一个儿子为}，删除符号表
    }

    void main() {
        int rt = cnt - 1;    //rt为根节点
        dfs(rt);
        for (auto it : attr[rt].code)
            fout << it.first << " (" << it.second[0] << ", " << it.second[1]
            << ", " << it.second[2] << ", " << it.second[3] << ")" << endl;
        fout << nextquad << endl;    //多输出一行
    }
}

int main() {
    //lr1::main();
    grammar::main();
    translate::main();
    return 0;
}

#include<bits/stdc++.h>

#ifndef yjc_parsing_def
#define yjc_parsing_def
#include "yjc_parsing.h"
#endif

using namespace std;

/*语言制导翻译*/
namespace yjc_translate {
	using namespace yjc_parsing;
    ofstream fout("yjc_quaternion.txt");

    /*变量号 T1、T2*/
    int tempVariable = 0;
    /*初始地址100*/
    int address = 100;
    /*变量*/
    /*代码块的符号数组*/
    vector<vector<string>> symbolTable;

    pair<int, vector<string>> genarateQuaternion(string a, string b, string c, string d);
    string searchVariable(string name);

    variable_definition_function(Trans tran,int u){
        symbolTable.back().push_back(attribution[GraphOfAdjacencyList[u][1]].name);
	}
	assignment_id_function(Trans tran,int u){

	}

	/*
	* returnType：void
	* function：id赋值语句的四元式
	* params：Trans int
	*/
	primary_num_function(Trans tran,int u){
		/*变量*/
        tran.valiableAddress = "T" + to_string(++tempVariable);
        tran.value = attribution[GraphOfAdjacencyList[u][0]].value;
        tran.code.push_back(genarateQuaternion("=", tran.value, "-", tran.valiableAddress));
	}
	/*
	* returnType：void
	* function：识别id的四元式
	* params：Trans int
	*/
	primary_id_function(Trans tran,int u){

	}
	/*
	* returnType：void
	* function：乘法的四元式
	* params：Trans int
	*/
	term_mul_function(Trans tran,int u){
		Trans e1 = attribution[GraphOfAdjacencyList[u][0]];
        Trans e2 = attribution[GraphOfAdjacencyList[u][2]];
        tran.valiableAddress = "T" + to_string(++tempVariable);
        tran.code.push_back(genarateQuaternion("*", e1.valiableAddress, e2.valiableAddress, tran.valiableAddress));
	}
	/*
	* returnType：void
	* function：加法四元式
	* params：Trans int
	*/
	expr_add_function(Trans tran,int u){
		Trans e1 = attribution[GraphOfAdjacencyList[u][0]];
        Trans e2 = attribution[GraphOfAdjacencyList[u][2]];
        tran.valiableAddress = "T" + to_string(++tempVariable);
        tran.code.push_back(genarateQuaternion("+", e1.valiableAddress, e2.valiableAddress, tran.valiableAddress));
	}
	/*
	* returnType：void
	* function：比较表达式四元式
	* params：Trans int
	*/
	rel_function(Trans tran,int u){
		Trans id1 = attribution[GraphOfAdjacencyList[u][0]];
        Trans id2 = attribution[GraphOfAdjacencyList[u][2]];
        tran.code.push_back(genarateQuaternion("j" + symbol[GraphOfAdjacencyList[u][1]], id1.valiableAddress, id2.valiableAddress, "0"));
        tran.True = tran.code.back().first;
        tran.code.back().second[3] = to_string(tran.code.back().first + 2);
        tran.code.push_back(genarateQuaternion("j", "-", "-", "0"));
        tran.False = tran.code.back().first;
	}
	/*
	* returnType：void
	* function：while语句的四元式
	* params：Trans int
	*/
	while_function(Trans tran,int u){
		tran = attribution[GraphOfAdjacencyList[u][0]];
        attribution[u].code.clear();
        for (auto it : attribution[GraphOfAdjacencyList[u][2]].code)
            tran.code.push_back(it);
        for (auto it : attribution[GraphOfAdjacencyList[u][4]].code)
            tran.code.push_back(it);
        tran.code.push_back(genarateQuaternion("j", "-", "-", to_string(tran.code.front().first)));

        for (auto &it : tran.code) {
            if (it.first == attribution[GraphOfAdjacencyList[u][2]].False) {
                it.second[3] = to_string(tran.code.back().first + 1);
                break;
            }
        }
        tran.code.back().second[3] = to_string(tran.code.front().first);
	}
	/*
	* returnType：void
	* function：if语句四元式
	* params：Trans int
	*/
	if_function(Trans tran,int u){
		tran = attribution[GraphOfAdjacencyList[u][0]];
        attribution[u].code.clear();
        for (auto it : attribution[GraphOfAdjacencyList[u][2]].code)
            tran.code.push_back(it);
        for (auto it : attribution[GraphOfAdjacencyList[u][4]].code)
            tran.code.push_back(it);
        for (auto &it : tran.code) {
            if (it.first == attribution[GraphOfAdjacencyList[u][2]].False) {
                it.second[3] = to_string(tran.code.back().first + 1);
                break;
            }
        }
	}
	/*
	* returnType：void
	* function：if else语句四元式
	* params：Trans int
	*/
	if_else_function(Trans tran,int u){
		tran = attribution[GraphOfAdjacencyList[u][0]];
        attribution[u].code.clear();
        for (auto it : attribution[GraphOfAdjacencyList[u][2]].code)
            tran.code.push_back(it);
        for (auto it : attribution[GraphOfAdjacencyList[u][4]].code)
            tran.code.push_back(it);
        tran.code.push_back(genarateQuaternion("j", "-", "-", to_string(tran.code.back().first + attribution[GraphOfAdjacencyList[u][6]].code.size() + 2)));
        tran.code.back().first = tran.code.front().first + tran.code.size() - 1;
        for (auto &it : tran.code) {
            if (it.first == attribution[GraphOfAdjacencyList[u][2]].False) {
                it.second[3] = to_string(tran.code.back().first + 1);
                break;
            }
        }
        for (auto it : attribution[GraphOfAdjacencyList[u][6]].code) {
            it.first++;
            if (it.second[0][0] == 'j') {
                int num = atoi(it.second[3].c_str());
                it.second[3] = to_string(num + 1);
            }
            tran.code.push_back(it);
            }
	}
    /*
	* returnType：void
	* function：深度优先遍历四元式内容
	* params：int
	*/
    void dfs(int u) {
    	Trans &tran = attribution[u];
    	/*如果为空，跳出当前循环 */
        if (GraphOfAdjacencyList[u].empty()) {
            return;
        }

        /*{，代码块的开始，则新建符号表数组*/
        if (symbol[GraphOfAdjacencyList[u].front()] == "{")
        {
        	symbolTable.push_back(vector<string>());
		}

		/*遍历*/
        for(int i=0;i<GraphOfAdjacencyList[u].size();i++){
			dfs(GraphOfAdjacencyList[u][i]);
		}

		/*清空四元式内容*/
		attribution[u].code.clear();
        tran = attribution[GraphOfAdjacencyList[u][0]];

		/*添加儿子节点的四元式*/
		for(int i=0;i<GraphOfAdjacencyList[u].size();i++){
			for(auto quaternion : attribution[GraphOfAdjacencyList[u][i]].code){
				tran.code.push_back(quaternion);
			}
		}

        if (symbol[u] == "variable_definition") {//函数定义
        	variable_definition_function(tran,u);
        }
        else if (symbol[u] == "assignment"
                 && symbol[GraphOfAdjacencyList[u][0]] == "ID") {//赋值语句
        	string p = searchVariable(attribution[GraphOfAdjacencyList[u][0]].name);    //查符号表
	        if (p == "unknown") {
	            cerr << "变量未声明" << endl;
	            return;
	        }
	        tran.valiableAddress = p;
	        tran.code.push_back(genarateQuaternion("=", attribution[GraphOfAdjacencyList[u][2]].valiableAddress, "-", tran.valiableAddress));

        }
        else if (symbol[u] == "primary"
                 && symbol[GraphOfAdjacencyList[u][0]] == "ID") {    //规约ID
        	string p = searchVariable(attribution[GraphOfAdjacencyList[u][0]].name);
	        if (p == "unknown") {
	            cerr << "变量未声明" << endl;
	            return;
	        }
	        tran.valiableAddress = p;
	        tran.name = attribution[GraphOfAdjacencyList[u][0]].name;

        }
        else if (symbol[u] == "primary"
                 && symbol[GraphOfAdjacencyList[u][0]] == "NUM") {//规约NUM
        	primary_num_function(tran,u);
        }

        else if (symbol[u] == "term"
                 && GraphOfAdjacencyList[u].size() > 1
                 && symbol[GraphOfAdjacencyList[u][1]] == "*") {//乘法表达式
            term_mul_function(tran,u);
        }

        else if (symbol[u] == "rel"
                 && GraphOfAdjacencyList[u].size() > 1) {//关系表达式
            rel_function(tran,u);
        }
         else if (symbol[u] == "expr"
                   && GraphOfAdjacencyList[u].size() > 1
                  && symbol[GraphOfAdjacencyList[u][1]] == "+") {//加法表达式
            expr_add_function(tran,u);
        }
         else if (symbol[GraphOfAdjacencyList[u][0]] == "IF"
                  && GraphOfAdjacencyList[u].size() == 5) {//IF语句
            if_function(tran,u);
        }

        else if (symbol[GraphOfAdjacencyList[u][0]] == "IF"
                  && GraphOfAdjacencyList[u].size() == 7) {//IF ELSE语句
            if_else_function(tran,u);
        }

        else if (symbol[GraphOfAdjacencyList[u][0]] == "WHILE") {//WHILE语句
            while_function(tran,u);
        }

        if (symbol[GraphOfAdjacencyList[u].back()] == "}")//如果最后一个儿子为}，删除符号表
        {
        	symbolTable.pop_back();
		}
    }

    /*查询符号表中的变量值*/
    string searchVariable(string name) {
        per(i, 0, symbolTable.size()) rep(j, 0, symbolTable[i].size())
            if (symbolTable[i][j] == name) return name;
        return "unknown";
    }

    /*构造四元式*/
    pair<int, vector<string>> genarateQuaternion(string a, string b, string c, string d) {
        vector<string> vec{ a,b,c,d };
        pair<int, vector<string>> p = make_pair(address++, vec);
        return p;
    }

    void main() {
        int root = cnt - 1;
        dfs(root);
        for (pair<int, vector<string>> it : attribution[root].code){
		    fout << "地址" << it.first << "对应的四元式:" << " (" << it.second[0] << ", " << it.second[1]<< ", " << it.second[2] << ", " << it.second[3] << ")" << endl;
		}

    }
}

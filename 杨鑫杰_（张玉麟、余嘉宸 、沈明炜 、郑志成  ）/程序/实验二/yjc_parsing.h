#include<bits/stdc++.h> 

#ifndef project
#define project
#include "smw_project.h"
#endif

using namespace std;

namespace yjc_parsing{ 
	using namespace smw_project;
    /*cnt记录个数*/
	int cnt = 0;
    vector<int> GraphOfAdjacencyList[1005];
    
	/*保存LR(1)分析表内容*/
	string lr1[1005][1005];
	/*记录LR(1)分析表行数、列数*/
	int m, n;	
	/*读取词法分析的结果，进行语法分析后将数据存入另一个文件中*/
	ifstream fin("yjc_resultOfWordAnalysis.txt");
	ofstream fout("yjc_parsingProcess.txt");
	/*状态栈以及状态栈初始化,栈顶指针*/
	int stateStack[1000]; 	
	int top = 1; 
	struct Trans{
		vector<pair<int, vector<string>>> code;    //四元式
		string valiableAddress;
		int True;
        int False;
	    string name;     //变量的name
	    string value;    //常量的value
	};
	
	/*节点结构数组*/
	vector<Trans> attribution;
	vector<string> symbol;   	
	/*即将弹出的头节点*/
    stack<int> S;
    queue<int> DyingAttr; 
   
	/*
	* returnType：void
	* function：读取分析表中的数据
	* params：null 
	*/
    void yjc_inputTableLR1() {
        ifstream fin("yxj_LR(1)Table.txt");
        fin >> n >> m;
        for(int i=0;i<n;i++){
        	for(int j=0;j<m;j++){
        		 fin >> lr1[i][j];
			}
		} 
    }   
	
	/*
	* returnType：void
	* function：通过分析表，进行输入串的语法分析过程 
	* params：null 
	*/
	void yjc_parsing(){
		/*定义变量*/
		int lineNum = 1; //当前行 
		stateStack[0] = 0; //状态栈初始值 
		/*输出表头*/
		fout << left << setw(5) << "";
	    fout << left << setw(80) << "状态栈";
	    fout << left << setw(150) << "符号栈";
	    fout << left << setw(80) << "输入串" << endl;
		/*词法分析的结果，num具体数值*/
	    pair<string, string> word;
	    /*用vector存词法分析的结果*/
	    vector<pair<string, string>> words;
	    while (fin >> word.first) {
	        if (word.first == "NUM" || word.first == "ID")fin >> word.second;
	        words.push_back(word);
	    }
	    /*结束标志#*/
	    words.push_back(make_pair("#", ""));
	    
	    Trans trans;                                            //初始状态
	    vector<pair<string, Trans>> symbolStack{ make_pair("#",trans) };    //初始状态
	    
	    /*语法分析过程*/
	    while (1) {
	    	/*输出行号*/    	
	        fout << left << setw(5) << lineNum++;
	        
	    	/*输出状态栈的内容*/
	        string stateStackString;
	        for(int i=0;i<top;i++){
	        	stateStackString += to_string(stateStack[i]) + ' ';
			}
	        fout << left << setw(50) << stateStackString;
	        
	        /*输出符号栈的内容*/
	        string symbolStackString;
	        for (pair<string, Trans> str : symbolStack){
	        	symbolStackString += str.first + ' ';
			} 
			fout << left << setw(150) << symbolStackString;
	        
	        /*输出输入串的剩余内容*/
	        string inputStackString;
	        for (pair<string, string> word : words) inputStackString += word.first + ' ';
	        fout << left << setw(80) << inputStackString << endl;
	        
	        
			cout<<"当前状态栈栈顶元素："<<stateStack[top - 1]<<endl;
			/*查LR（1）表*/
	        string tableValue = lr1[stateStack[top - 1]][Hash[words[0].first]];
	        
	        cout<<"查询LR（1）表的对应值："<<tableValue<<endl; 
	        
	        /*若不是acc也不是err，说明是s移进或者r规约*/
	        if (tableValue[0] == 's') { 
	        	cout<<"--------------------------分界线-----------------------------"<<endl;
	        	cout<<"正在进行移进操作..."<<endl;
	            /*删去开头的s*/
	            tableValue.erase(0, 1);
	            /*字符串转为数字*/
	            int num = atoi(tableValue.c_str());            
	                
				/*新建一个转换关系*/
	            Trans trans;
	            if (words[0].first == "ID") trans.name = words[0].second;
	            else if (words[0].first == "NUM") trans.value = words[0].second;
	            
	            /*入栈*/
	            stateStack[top++] = num; 
	            cout<<"状态栈入栈："<<num<<endl;
	            symbolStack.push_back(make_pair(words[0].first, trans));
	            cout<<"符号栈入栈："<<words[0].first<<endl;
	            
	            symbol.push_back(words[0].first);                    //push进栈symbol
                attribution.push_back(trans);                                //push进栈attribution
                S.push(cnt++);                                    //S中push的当前的节点号
	            
	            /*出栈*/
	            words.erase(words.begin());
	            cout<<"输入串内容减少为："<<inputStackString<<endl;
	        }
	        else if(tableValue[0] == 'r'){
	        	cout<<"--------------------------分界线-----------------------------"<<endl;
	        	cout<<"正在进行规约操作..."<<endl;
	        	
	        	DyingAttr = queue<int>();

	        	
	            /*删去开头的s*/
	            tableValue.erase(0, 1);
	            /*字符串转为数字*/
	            int num = atoi(tableValue.c_str());  
	            
	            cout<<"规约的语法号数为："<<num<<endl;
	            
	            /*用第num条产生式规约*/
	            Production production = vec_production[num];
	            /*记录second的长度*/
	            int len = production.second.size();
	            for(int i=0;i<len;i++){
	            	/*状态栈出栈*/
	                top--;
	                cout<<"状态栈出栈："<<stateStack[top]<<endl;
	                /*符号栈出栈*/
	                cout<<"符号栈出栈："<<symbolStack.back().first<<endl;
	                symbolStack.pop_back();
	                
	                DyingAttr.push(S.top());                    //DyingAttr先记录下S需要pop的节点
                    S.pop();                                //pop栈S
	            }
	            /*入栈*/
	            symbolStack.push_back(make_pair(production.first, trans)); 
	            cout<<"符号栈入栈："<<symbolStack[0].first<<endl;
	            
	            symbol.push_back(production.first);                //push栈symbol
                cout<<"production.first:"<<production.first<<endl;
                attribution.push_back(trans);                        //push栈attribution
                S.push(cnt++);                            //push栈S
                while (!DyingAttr.empty()) {
                    GraphOfAdjacencyList[cnt - 1].push_back(DyingAttr.front());    //构造Tree
                    DyingAttr.pop();
                }
	            
	            //cout<<"production.first:"<<production.first<<endl;
	            //yjc
	            //stateStack[top++] = atoi(lr1[stateStack[top-1]][Hash[p.first]].c_str());    //push栈a
	            //cout<<"stateStack[top-1]:"<<stateStack[top-1]<<endl;
	            //cout<<"Hash[p.first]:"<<Hash[production.first]<<endl;
	            //cout<<"lr1[stateStack[top-1]][Hash[p.first]]:"<<lr1[stateStack[top-1]][Hash[production.first]]<<endl;
	            //cout<<"atoi(lr1[stateStack[top-1]][Hash[p.first]].c_str()):"<<atoi(lr1[stateStack[top-1]][Hash[production.first]].c_str())<<endl;
	            int preAtop = atoi(lr1[stateStack[top-1]][Hash[production.first]].c_str());    //push栈a
	            //居然被我改出来了！！！！！ 
	            stateStack[top++] = preAtop;
	            cout<<"状态栈入栈："<<stateStack[top]<<endl;
	            //cout<<"stateStack[top-1]:"<<stateStack[top-1]<<endl;
	        }
	        if (tableValue == "acc"){
	        	cout<<"--------------------------分界线-----------------------------"<<endl;
			    cout<<"语法分析结束且成功!语句没有语法错误！"<<endl; 
				break; 
			} 
	        if (tableValue == "err") {
	        	cout<<"--------------------------分界线-----------------------------"<<endl;
	        	string pre;
				pre =  symbolStackString;
	            cout<<"语法分析结束且失败!语句在处理"+pre+"语句时出现了错误！"<<endl; 
	            return;
	        }	        
	    }
	}
	void main() {
        smw_project::main();
        yjc_inputTableLR1();
        yjc_parsing();
    }
    
} 

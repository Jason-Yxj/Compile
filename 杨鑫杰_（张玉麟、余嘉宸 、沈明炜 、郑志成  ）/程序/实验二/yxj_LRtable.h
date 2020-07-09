#include "smw_PJSet.h"

//生成LR(1)的分析表的部分
namespace yxj_LRtable {
    using namespace smw_project;

    int row, col;                      //row分析表的行数，col分析表的列数
    string LRtalbe[1005][1005];        //存储LR(1)分析表的结果
    vector<set<Project>> pset(1);      //项目的集合

	//求FIRST集
	set<string> FirstSet(vector<string> X) {
        set<string> First;
        if (terminal_symbol.find(X[0])!=terminal_symbol.end()){ //如果是终结符，直接insert返回
            First.insert(X[0]);
            return First;
        }
        else {
            for(int i=0;i<vec_production.size();i++){
                if (vec_production[i].first==X[0]){
                    if (terminal_symbol.find(vec_production[i].second[0])==terminal_symbol.end()){     //如果第一个不是终结符的话
                        set<string> temp = FirstSet(vec_production[i].second);                         //就递归求FIRST集
                        First.insert(temp.begin(),temp.end());
					}
                    else																			   //如果第一个是终结符的话
						First.insert(vec_production[i].second[0]);									   //就插入到First中
                }
            }
        }
        return First;
    }

    set<Project> yxj_Go(set<Project> SP, string S){
        set<Project> G;
        for (auto it : SP){
            vector<string> p=it.right;
            auto positon=find(p.begin(), p.end(), ".");

			if(positon==p.end()-1)
				continue;
            if(*(positon+1)==S){
                swap(*positon, *(positon+1));
                G.insert(Project{it.left,p,it.expect});
            }
        }
        return G;
    }

    //函数功能：用于求closure
    set<Project> zyl_Closure(set<Project> I) {
        while (1){
            //is_update用来判断此次的循环是否有更新
            bool is_update=false;

            for (auto it : I) {
                vector<string> B=it.right;

                //position用于记录找到的·的位置
                auto position=find((B).begin(),(B).end(), ".");
                if (position==B.end()-1){
                    continue;
                }

                string the_char=*(position+1);
                if (terminal_symbol.find(the_char)!=terminal_symbol.end()){
                    continue;
                }

                B.erase(B.begin(), position+2);
                string last;//last用于记录上一次求FIRST集的第一个字符串
                for (auto ite : it.expect) {
                    B.push_back(ite);//把expect插入到B的后面
                    if (last==B[0]){
                        continue;//文法中没有空产生式,所以不求了
                    }

                    else last=B[0];

                    //求B的FIRST集
                    set<string> First=FirstSet(B);
                    B.pop_back();

                    for (auto it1 : set_production) {
                        if (it1.first==the_char&&it1.second[0]==".") {
                            set<string> a_set;
                            Project p{it1.first,it1.second,a_set};
                            auto position=I.lower_bound(p);//查找第一个大于等于p的位置
                            if (position!=I.end()&&(*position).left==it1.first&&(*position).right==it1.second){
                                p=*position;
                                I.erase(p);//先删除再插入
                                p.expect.insert((First).begin(),(First).end());
                                I.insert(p);
                            }
                            else {
                                is_update=true;
                                I.insert(Project{it1.first,it1.second,First});
                            }
                        }
                    }
                }
            }
            //没更新则跳出循环
            if (!is_update) {
                break;
            }
        }
        return I;
    }

	//求出LR1(1)分析表的过程
    void yxj_LR1(){
        for (auto it : set_production){
            if (it.first=="program'"&&it.second[0]=="."){
                set<string> setString;
                setString.insert("#");
                pset[0].insert(Project{it.first,it.second,setString});
                break;
            }
        }

        pset[0]=zyl_Closure(pset[0]);
        for(int i=0;i<pset.size();i++){

			//进行规约
            for (auto it : pset[i]){
                int length=it.right.size();

                if (it.right[length-1]=="."){
                    it.right.erase(it.right.end() - 1);
                    Production p(it.left, it.right);
					for(int j=0;j<vec_production.size();j++){
                        if (vec_production[j]==p){
                            string res="r" + to_string(j);
							if (j==0)
								res="acc";
                            for (auto its : it.expect)
                                LRtalbe[i][Hash[its]]=res;
                        }
                    }
                }
            }

            for (auto S : all_symbols){               //遍历所有的符号
                set<Project> P=yxj_Go(pset[i], S);    //P = GO(I, X)
                if (!P.empty()){                      //如果P不为空
                    P=zyl_Closure(P);                 //求P的闭包
                    int t;
                    auto positon=find(pset.begin(),pset.end(),P);
                    if(positon==pset.end()){
                    	t=pset.size();
                        pset.push_back(P);
                    }
                    else{
                        t=positon-pset.begin();
                    }

                    //GOTO和移进
                    int j;
					j=Hash[S];
                    if (terminal_symbol.find(S)==terminal_symbol.end())
  						LRtalbe[i][j]=to_string(t);
                    else
                    	LRtalbe[i][j]="s"+to_string(t);
                }
            }
        }
    }

    //输出项目集和LR(1)的分析表到文件中
    void Print_Pj() {
        ofstream fout("yxj_result.txt");//输出到文件yxj_result.txt

        fout<<"      CLOSURE"<<endl;
        for(int i=0;i<40;i++)
			fout<<'*';
        fout<<endl;

        //输出项目集
		for(int i=0;i<pset.size();i++){
            fout<<i<<":"<<endl;
            set<Project> set_production=pset[i];
            for (auto it1 : set_production){
                string b;
                for(int i=0;i<it1.right.size();i++)
					b+=' '+it1.right[i];
                b.erase(0, 1);

				string c;
                for(auto it2 : it1.expect)
					c+=it2+'/';
                c.pop_back();
                fout<<"    "<<it1.left<<" -> "<<b<<", "<<c<<endl;
            }
            for(int i=0;i<40;i++)
				fout<<'*';
            fout<<endl;
        }
        fout<<endl<<endl<<endl<<endl;

		//输出LR(1)的分析表
        for(int i=0;i<3;i++)
			fout<<setw(16)<<' ';
        fout<<"LR(1)的分析表"<< endl;
        fout<<'|'<<endl<<'|';
        fout<<setw(16)<<"状态";
        fout<<'|';
        row=pset.size();
        col=vec_symbols.size();
        for(int i=0;i<col;i++){
            fout<<setw(16)<<vec_symbols[i];
            if(i==terminal_symbol.size()-1)
				fout<<'|';
        }
        fout<<'|'<<endl<<'|';
       	for(int i=0;i<200;i++)
		   fout<<'-';
        fout<<'|'<<endl<<'|';
        for(int i=0;i<row;i++){
            fout<<setw(16)<<i;
            fout<<'|';
            for(int j=0;j<col;j++){
                fout<<setw(16)<<LRtalbe[i][j];
                if(j==terminal_symbol.size()-1)
					fout<<'|';
            }
            fout<<'|'<<endl<<'|';
        }
        for(int i=0;i<200;i++)
			fout<<'-';
        fout<<'|';
        fout<<endl<<endl<<endl<<endl;
    }

	//将LR(1)分析表输出到文件yxj_LR(1)Table.txt中
    void Print_LR1(){
        cout<<"we do it "<<endl;
        ofstream fout("yxj_LR(1)Table.txt");
        fout<<row<<" "<<col<<endl;;
        for(int i=0;i<row;i++){
            for(int j=0;j<col;j++) {
                if(LRtalbe[i][j]!="") //判断是否为空
					fout<<LRtalbe[i][j]<<' ';
                else //如果为空，输出err
					fout<<"err ";
            }
            fout<<endl;
        }
    }

    //输出到文件
    void yxj_Print(){
    	Print_Pj();
    	Print_LR1();
	}

    void main() {
        smw_project::main();
        yxj_LR1();
        yxj_Print();
    }
}

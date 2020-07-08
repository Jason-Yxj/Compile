package yxj;

public class Lexer {	
	public int begin=0;//开始指针
	public int forward=0;//搜索指针
	public int state=0;	//状态
	public String Key[]= {"if","else"};//关键字
	

	public String word;//单词符号
	public String type;//类别

	
	public void analyse(char[] bf) {
		ReadFile.readFile(bf, forward);//先载入一轮
		while(Cache.cache(bf, forward)){
			int fwd=forward;
			while(fwd==forward) {//处理同一个字符的循环
				switch (state) {
				case 0:
					state=relop(bf);
					break;
				case 9:
					state=id(bf);
					break;
				case 12:
					state=fail();
					break;
				case 20:
					state=fail();
					break;
				case 25:
					state=fail();
					break;
				default:
					break;
				}
			}
			state=0;
		}
	}

	//比较运算符DFA
	public int relop(char[] bf) {
		int state=0;
		char c;
		while(toolFunction.isRelop(c=bf[forward])) {
			switch (c) {
			case '<':
				state=1;
				c=bf[++forward];
				switch (c) {
				case '=':
					state=2;
					word="<=";
					type="relop";
					break;
				case '>':
					state=3;
					word="<>";
					type="relop";
					break;
				default:
					state=4;
					retract(1);//回退
					word="<";
					type="relop";				
					break;
				}
				break;
			case '=':
				state=5;
				word="=";
				type="赋值";
				break;
			case '>':
				state=6;
				c=bf[++forward];
				switch (c) {
				case '=':
					state=7;
					word=">=";
					type="relop";
					break;
				default:
					state=8;
					retract(1);//回退
					word=">";
					type="relop";
					break;
				}
				break;
			default:
				state=fail();
				break;
			}
			forward++;
			System.out.print("\n<"+word+","+type+">");		
		}
		if(state==0)
			state=fail();
		return state;
	}
	
	//标识符、关键字的DFA
	public int id(char[] bf) {
		int state=9;
		char c;
		String str="";
		if(toolFunction.isLetter(c=bf[forward])) {
			state=10;
			str+=c;
			forward++;
			while(toolFunction.isLetter(c=bf[forward])||toolFunction.isDigit(c=bf[forward])) {
				state=10;
				str+=c;
				forward++;
			}
			state=11;
			retract(1);//回退
			word=str;
			type="id";
			for (String s : Key) {//判断类别是标识符还是关键字
				if(word.equals(s)){
					type="key";
				}
				//System.out.print("\nword:  "+word+"  s:  "+s+"  结果："+(word.equals(s)));
			}
			forward++;
			System.out.print("\n<"+word+","+type+">");	
		}
		else 
			state=fail();
		return state;
	}
	
	//带*的终结状态回退，多扫描的拿掉
	public void retract(int n) {
		forward-=n;
	}
	
	
	//连接几个DFA
	public int fail() {
		int start=state;
		switch (start) {
        case 0:   start = 9;  break;
        case 9:   start = 12; break;
        case 12:  start = 20; break;
        case 20:  start = 25; break;
        case 25:  forward++;/*recover();*/  break;
        default:  /* compiler error */break;			
		}
		return start;
	}
	
}

package 实验二;

import java.util.HashMap;
import java.util.Map;
import java.util.Stack;
/*
 * function:grammatical analysis * 
 */
public class yjc_parsing {
	private Stack<Integer> stateStack = new Stack<>();
	private Stack<String> symbolStack = new Stack<>();
	private Stack<String> inputStringStack = new Stack<>();
	private Map<Integer,Map<String,String>> actionTable = new HashMap<>();//action表
	private Map<Integer,Map<String,String>> gotoTable = new HashMap<>();//goto表
	private Map<Integer,type> rulesMap=new HashMap<>();
	/*定义变量*/
	private int step=1;
	/*
	 * function:whether is Terminator
	 * params:
	 */
	public boolean isTerminator() {
		return true;
	}
	/*
	 * function:search Action Table
	 * params:
	 */
	public void searchAction(String inputStringStackTop,int stateStackTop) {		
		/*获取action表中数据*/
		Map<String,String> actionMapValue= new HashMap<>();
		actionMapValue = actionTable.get(stateStackTop);
		String actionString = actionMapValue.get(inputStringStackTop);
		/*表中数据肢解*/
		char actionStringHead = actionString.charAt(0);
		String actionStringLeft = actionString.substring(1);
		
		/*规约*/
		if(actionStringHead=='r') {
			int ruleNum=Integer.parseInt(actionStringLeft);
			type type=new type();
			type=rulesMap.get(ruleNum);
			/*要弹出几条*/
			int popnum=type.pop;
			System.out.println(step,stateStack.toString(), symbolStack.toString(),"按照"+type.left+"->"+type.right+"的规则进行规约");
			for(int i=0;i<popnum;i++)
			{
				stateStack.pop();//状态栈弹出
				symbolStack.pop();//符号栈弹出
			}
			stateStackTop=stateStack.peek();//顶端
			symbolStack.push(type.right);
			/*非终结符查goto表*/
			Map<String,String> gotoMap=new HashMap<>();
			gotoMap=gotoTable.get(stateStackTop);//非终结符用goto
		    if(gotoMap.containsKey(type.right))
		    {
		    	symbolStack.push(Integer.parseInt(gotoMap.get(type.right)));
		    	stateStackTop=Integer.parseInt(gotoMap.get(type.right));		    	
		    }
		    else
		    {
		    	System.out.println("error sen!");
		    	System.out.println(("语法错误!当前状态 ："+stateStackTop+"无法解析符号:"+actionString));
		    	break;
		    }
		}
		else if(actionStringHead=='s')/*移入*/
		{
			System.out.println(step+ stateStack.toString()+symbolStack.toString()+actionString);
			stateStack.push(Integer.parseInt(actionStringLeft));
			stateStackTop=Integer.parseInt(actionStringLeft);
			symbolStack.push(actionString);
		}
		else if(actionString.equals("acc"))
		{
			//归约成功
			System.out.println(step+stateStack.toString()+symbolStack.toString());
			System.out.println("success!");
			break;
		}
		else
		{
			System.out.println("error");
			System.out.println(("\r\n语法错误!\r\n当前状态 : "+stateStackTop+"\r\n无法解析符号 ："+actionString+"\r\n").getBytes());
			break;
		}
	}
	/*
	 * function:search Goto Table
	 * params:
	 */
	public void searchGoto(String inputStringStackTop,int stateStackTop) {
		/*获取goto表中数据gotoValue*/
		Map<String,String> gotoMapValue= new HashMap<>();
		gotoMapValue = gotoTable.get(stateStackTop);
		String gotoValue=gotoMapValue.get(inputStringStackTop);
		/*stateStackTop:第几个规则*/
		stateStackTop = Integer.parseInt(gotoValue);
		stateStack.push(stateStackTop);
		
	}
	/*
	 * function:working starter
	 * params:useless params
	 */
	public void main(String[] args) {
		// TODO Auto-generated method stub
		/*获取预处理好的数据*/
		while(true) {
			if(isTerminator()) {/*终结符查action*/
				searchAction(inputStringStack.peek(),stateStack.peek());
			}else {/*非终结符查goto*/
				searchGoto(inputStringStack.peek(),stateStack.peek());
			}
			step+=1;//步骤加一
		}
		System.out.println("运行到了main");
	}

}

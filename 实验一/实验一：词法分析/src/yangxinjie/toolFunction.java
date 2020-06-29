package yangxinjie;

public class toolFunction {
	public static boolean isLetter(char c)
	{
		if(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z')
		{
			return true;
		}
		else return false;
	}
	public static boolean isDigit(char ch)
	{
		
		if(ch >= '0' && ch <= '9')
		{
			return true;
		}
		else return false;
	}
	public static boolean isRelop(char c) {//判断是否是比较运算符
		if(c=='<'||c=='='||c=='>')
			return true;
		else return false;
	}	
	public static boolean isDelim(char c) {//判断是否为分隔符
		if(c=='{' || c=='}' || c=='(' || c==')' || c==';' || c=='[' || c==']' || c=='.')
			return true;
		else {
			return false;
		}
	}
	public static boolean isOp(char c) {
		if(c=='+' || c=='-' || c=='*' || c=='/')
			return true;
		else {
			return false;
		}
	}
}

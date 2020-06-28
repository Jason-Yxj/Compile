package yxj;

public class Main {	
	public static void main(String[] args) {
		char[] bf=new char[2048];//定义双缓冲区
		bf[1023]='~';//EOF符号结尾
		bf[2047]='~';//EOF符号结尾
		Lexer lexer=new Lexer();
		lexer.analyse(bf);			
	}
}

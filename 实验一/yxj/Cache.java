package yxj;

/*
 * 当遇到终止符EOF时的处理
 * 我们将字符'~'当作EOF
 * @yxj*/
public class Cache {
	public static boolean cache(char[] bf,int forword) {
		boolean flag=true;//判断是否到达了字符串末尾
		switch (bf[forword]) {
		case '~'://缓存区操作
			if(forword==1023) {
				forword++;
				ReadFile.readFile(bf, forword);					
			}
			else if (forword==2047) {
				forword=0;
				ReadFile.readFile(bf, forword);
			}
			else{//字符串末尾的EOF
				flag=false;
				break;
			}
		default:
			break;
		}
		return flag;
	}
}

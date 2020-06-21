package yxj;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.Reader;

/*实现文本的预处理
 * 载入双缓冲区
 * 分段保存
 * 每次载入不超过1K
 * @yxj*/
public class ReadFile {
	public static String fileName="test.txt";
	//读取文件中的程序
	public static void readFile(char[] bf,int fwd) {
		File file=new File(fileName);
		Reader reader=null;
		try {
			reader=new InputStreamReader(new FileInputStream(file));
			int ch;
			while((ch=reader.read())!=-1) {				
				if((char)ch!='\r'&&(char)ch!='\n'&&(char)ch!=' ') {//预处理换行、空格				
					bf[fwd]=(char) ch;//将字符存入缓冲区
					fwd++;										
					System.out.print((char)ch);//测试					
				}
				if(bf[fwd]=='~')//每次最多载入1K个字符
					break;
			}
			bf[fwd]='~';//EOF符号结尾
			reader.close();
		}catch(Exception e) {
			e.printStackTrace();  
		}
	}
}

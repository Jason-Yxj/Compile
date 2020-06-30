package yangxinjie;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

/*写入文件yxj_output.txt
@yxj*/
public class WriteFile {
	public static void write(String str) {
		try {
			File file=new File("yxj_output.txt");
			if(!file.exists()) {
				file.createNewFile();
			}
			FileWriter fileWriter=new FileWriter(file.getName(),true);//使用true，即进行append file
			fileWriter.write(str);
			fileWriter.close();
		}catch(IOException e) {
			e.printStackTrace();
		}		
	}
	public static void new_write(String str) {
		try {
			File file=new File("yxj_output.txt");
			if(!file.exists()) {
				file.createNewFile();
			}
			FileWriter fileWriter=new FileWriter(file.getName());//覆盖原文件中的内容
			fileWriter.write(str);
			fileWriter.close();
		}catch(IOException e) {
			e.printStackTrace();
		}		
	}
}

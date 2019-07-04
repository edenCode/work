
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.StringBufferInputStream;
import java.net.InetAddress;
import java.nio.CharBuffer;

import com.sun.xml.internal.fastinfoset.util.CharArrayString;

class Test{
    
    Test(){

    }

    public void test_file(){
        
        File f = new File("test.txt");

        try {
            
            /**
             *  char 数组转换成string
             *      数组  char[] buffer 
             *  1.   str = new String(buffer)
             *  2. StringBuffer b = new StringBuffer()
             *      b.append(buffer)
             *      b.toString()
             */

            
            // 文件字符流
            FileWriter fw = new FileWriter(f);
            String str = "This is test file \n";
            
            fw.write(str.toCharArray());
            System.out.println(new String(str.toCharArray()));     
            //fw.append('a');  // 追加文件末尾
            fw.close();

            FileReader fr = new FileReader(f);

            char [] buffer = new char[1024];
            
            //CharBuffer
            int ret = fr.read(buffer);

            fr.close();
            System.out.println("ret ： " + ret + " buffer : " + new String(buffer));

            // 文件 字节流
            FileOutputStream fos = new FileOutputStream(f);

            fos.write("str".getBytes("utf-8"));
            fos.close();

            FileInputStream fis = new FileInputStream(f);
    
            //StringBufferInputStream sis = new St
            byte[] b = new byte[1024];
            ret = fis.read(b);
            fis.close();

            //StringBuffer sb = new StringBuffer();
            //sb.append(b);
            System.out.println("ret ： " + ret + " buffer : " + new String(b,"utf-8") );
            
        } catch (Exception e) {
            System.out.println(e.toString());
            System.out.println(e.getMessage());
        }
    }


    public void test_net(){
        Server s = new Server(9999);

        try {
            
            Thread.sleep(1000);
            Client c = new Client(InetAddress.getLocalHost().getHostAddress(), 9999);
            System.out.println("Start host " + InetAddress.getLocalHost().getHostName()+"\t port " + 9999);
            c.start();
        } catch (Exception e) {
            //TODO: handle exception
            System.out.println(e.getMessage());
        }


    }
    
    public static void main(String[] args) {
        Test t = new Test();
        t.test_file();

        t.test_net();
    }
}
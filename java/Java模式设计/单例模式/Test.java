import java.lang.management.ThreadInfo;

import Utils;
import Utils1;

class Test{

    private static int count;
    private static Test t;

    public  synchronized void do_task(int count){
        
        //Test t1 = (Test)t;
        //int count = t.count; //t1.count;
        while(true){
            Utils1 utils = Utils1.newInstance();
            System.out.println("Count : " + count + "\t"+ utils.toString());
            count ++;
            try {
                Thread.sleep(1000*1);
            } catch (Exception e) {
                //TODO: handle exception
                e.printStackTrace();
            }
            if(count == 10){
                //return;
                break;
            }
        }
    }

    public static synchronized Test instance(){
        
        if(t== null){
            t = new Test();
        }

        return t;
    }

    private Test(){

    }
    
    public static void main(String[] args) {
        
        // 	The constructor Utils() is not visible
        //Utils utils = new Utils(); // error 
        
        Utils utils = Utils.newInstance();

        utils.printf("singel Instance \n");

        // can not  a static refrence a not static 
        //final Test t = new Test();
        
        // That is illega modified for parameter count； it must set be final
        
        // The Thread run function signature is void run(void)
        new Thread(()->{Test t = Test.instance() ; t.do_task(t.count); t.count ++; }).start();
        new Thread(()->{Test t = Test.instance() ; t.do_task(t.count); t.count ++;}).start();

        Utils1 utils1 = Utils1.newInstance();

        utils1.printf("Singel Instance  1 : " + utils.toString());

    }
}
import com.sun.corba.se.impl.orbutil.StackImpl;

class Utils1{
    private static Utils1 utils1;

    public void printf(String str){
        System.out.println(" " + str);
    }


    public static synchronized Utils1 newInstance(){

        if(utils1 == null){

            // 利用synchronized 进行同步
            // java 中船检对象有两步： 首先分配内存，然后在赋值。
            //synchronized(utils1){ // NullPointerException
                utils1 = new Utils1();
            //}
        }

        return utils1;
    }

    private Utils1(){
        
    }

}
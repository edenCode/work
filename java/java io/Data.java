import java.io.Serializable;

class Data implements Serializable{

    private static final long serialVersionUID = 1L;

    private static int count;
    private String time;

    public static Data newInstance(){
        static Data d;
        if(d == null){
            d = new Data();
        }

        return d;
    }

    public static void init(){
        newInstance();
    }

    public  void setTime(String s){
        time = s;
    }
    
    public  String getTime(){
        return time;
    }

    public  int increamcount(){
        return count++;
    }

    public int getCount(){
        return count;
    }

    Data(){
    }
}
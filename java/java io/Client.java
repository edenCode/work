import java.io.InputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.lang.Thread;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.time.LocalTime;

//import java.util.Date; deprecate

class Client extends Thread{
    private int port;
    private String ip;
    private Socket s;
    //private final  Data d;
    //private Inet4Address address;
    Client(String ip,int port){
        this.ip = ip;
        this.port = port;
        //address = new InetAddress();
       // d = Data.newInstance();
        init();
    }
    
    private void init(){
        
        s = new Socket();
        // 初始化一个 socket
        //s = new Socket(ip,port);
        try {
            SocketAddress address = new InetSocketAddress(InetAddress.getByName(ip), port);
            s.connect(address);
            
        } catch (Exception e) {
            //TODO: handle exception
            System.out.println("socket " + e.getMessage());
        }
    }

    //public Data getData(){
     //   return d;
   // }

    @Override
    public void run() {
        super.run();
        try {
            do_task();
            
        } catch (Exception e) {
            //TODO: handle exception
            System.out.println("doTask" + e.getMessage());
        }
    }

    private void do_task() throws Exception{
    
            OutputStream os = s.getOutputStream();
            InputStream is = s.getInputStream();
            Data d = new Data();
            //d = new Data();
            while(true){
                LocalTime t = LocalTime.now();
                d.setTime(t.toString());
               
                int ret = d.increamcount();

                ObjectOutputStream oos = new ObjectOutputStream(os);

                oos.writeObject(d);

                if(ret == 10){
                    oos.write(new String("bye bye").getBytes("utf-8"));
                    break;
                }
            }

        }
    
}
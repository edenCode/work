import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

class Server{
    //private Data d;
    private ServerSocket s;
    Server(int port){

        try {
            
            s = new ServerSocket(port);
            new Thread(()-> do_Task() ).start();
        } catch (Exception e) {
            //TODO: handle exception
        }
        //this.s = s;
        //Data.init();
        //d= Data.newInstance();
        // java 8 开启一个线程
    }

    private void do_Task(){
        
        while(true){
            try {
                Socket socket = s.accept();
                System.out.println("Client : " + socket.getInetAddress().getHostName() +" : " + socket.getPort()); 
                
                socket.setKeepAlive(true);
                
                OutputStream os = socket.getOutputStream();
                InputStream is = socket.getInputStream();

                //is.available()
            
                ObjectInputStream ois = new ObjectInputStream(is);
                int count = 0;
                while(true){
                    
                    System.out.println("index : " + count++);
                    if(true){

                        int len = is.available();

                        byte [] buffer = new byte[len];
                        //is.read(buffer,len);
                        Data d = (Data)ois.readObject();
                        if(d == null){
                            Thread.sleep(1*1000);
                            
                            continue;
                        }
                        System.out.println("Data time : " + d.getTime() + " Data count : " + d.getCount());
                    }
                
                    //if(){
                    //   String str =(String)ois.readObject();
                     //  System.out.println("server " + str);
                    //}
                }
            } catch (Exception e) {
                //TODO: handle exception
                System.out.println("server : " + e.getMessage()+" : "+ e.toString());
            
            }
        }
    }
}
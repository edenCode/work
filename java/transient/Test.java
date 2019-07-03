import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.FileOutputStream;
import java.io.FileInputStream;

class Test{
    public static void main(String[] args) throws Exception{
        Rectangle rect = new Rectangle(3, 4);
        
        // area is set 12
        System.out.println("start : "+rect);
        
        // save Object into a file
        ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream("rectangle"));
        out.writeObject(rect);
        out.close();

        // Read object from file 
        ObjectInputStream in = new ObjectInputStream(new FileInputStream("rectangle"));
        Rectangle r =(Rectangle)in.readObject();
        in.close();

        // ares is set 0 
        System.out.println("transfer : " + r);

        // Reset area attribute
        r.setArea();

        // area is set 12
        System.out.println("recover : "+r);
    }
}

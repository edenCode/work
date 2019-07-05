

public interface Interface{
    
    public final static int a = 0;
    default void printf(){
        System.out.println("Default printf");
    }

    static int id(){
        return a;
    }
}

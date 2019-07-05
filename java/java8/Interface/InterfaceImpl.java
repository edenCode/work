import Interface;

class InterfaceImpl  implements Interface{
    // overrid printf
    public static int id = 2;

    @Override
    public void printf(){
        System.out.println("InterfaceImpl implemets Interface printf function ，id " + Interface.id());
    }

    

    @Override
    public static int id(){
        System.out.println("id " + id);
        return id ++ ;
    }
    public static void main(String[] args) {
        InterfaceImpl inter = new InterfaceImpl();
        
        inter.printf();

        int id = inter.id();
        System.out.println("This id extends from Interface id " + id);
    }
}
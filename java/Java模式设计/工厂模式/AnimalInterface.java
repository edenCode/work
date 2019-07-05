
//public abstract class AnimalInterface{
//    public void eat(String str){}
//}

public interface AnimalInterface{
    default void eat(String str){
        System.out.println("This Function can be covert by impl");
    }
}
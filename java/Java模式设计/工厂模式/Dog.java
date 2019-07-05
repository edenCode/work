
import AnimalInterface;

class Dog implements AnimalInterface{
//class Dog extends AnimalInterface{
    private int id = 0;

    @Override
    public void eat(String str){
        System.out.println("Dog eat ." + str);
    }

    public void printf(){
        System.out.println("dog id  : " + id);
    }

    Dog(){
    }
}
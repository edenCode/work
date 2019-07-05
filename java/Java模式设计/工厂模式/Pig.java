
import AnimalInterface;

class Pig implements AnimalInterface{
 // class Pig extends AnimalInterface{
    private int id = 1;

    Pig(){

    }

    @Override
    public void eat(String str){
        System.out.println("Pig eat " + str);
    }

    public void printf(){
        System.out.println("id : " + id);
    }
}
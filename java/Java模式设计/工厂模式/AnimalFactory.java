import DogFactory;
import PigFactory;
import FactoryInterface;

class AnimalFactory{

    public static void main(String[] args) {
        // Factory
        FactoryInterface factory = new DogFactory();

    
        AnimalInterface dog = factory.produce();

        dog.eat("🍖🍗");

        FactoryInterface factory1 = new PigFactory();
        AnimalInterface pig  = factory1.produce();

        pig.eat("🐖");
    }
}
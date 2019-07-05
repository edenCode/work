import FactoryInterface;

class DogFactory implements FactoryInterface{

    @Override
    public AnimalInterface produce(){

        return new Dog();
    }

    DogFactory(){

    }
}
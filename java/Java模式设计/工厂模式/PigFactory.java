
import FactoryInterface;


class PigFactory implements FactoryInterface{

    @Override
    public AnimalInterface produce(){

        return new Pig();
    }

    PigFactory(){

    }
}
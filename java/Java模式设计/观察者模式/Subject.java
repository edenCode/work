import java.util.List;
import java.util.ArrayList;

public class Subject {

    List<ObserverInterface> observers = new ArrayList<>();
    void attach(ObserverInterface obj){
        observers.add(obj);
        System.out.println("Observer attach");
    }

    void deattch(ObserverInterface obj){
        observers.remove(obj);
        System.out.println("Observer dettach");
    }

    void notifyObserverAll(String state){
        for(ObserverInterface obj:observers){
            obj.update(state);
            System.out.println("Notify all Observer update");
        }
    }
}
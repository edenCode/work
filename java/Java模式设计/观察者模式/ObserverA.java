
/*
 观察者实例
*/
public class ObserverA implements ObserverInterface{
    @Override
    public void update(String state) {
        System.out.println("Observer A object update");
    }
}


public class ObserverB implements ObserverInterface{
    @Override
    public void update(String state) {
        System.out.println("Observer B update "+state);
    }
}
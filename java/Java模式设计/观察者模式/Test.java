import java.util.Timer;
import java.util.TimerTask;


class Test{
    public static void main(String[] args) throws Exception {
        ObserverA a = new ObserverA(); // 观察者
        ObserverB b = new ObserverB();

        SubjectObject object = new SubjectObject(); // 被观察者

        object.attach(a);
        object.attach(b);

        Timer t = new Timer();
        
        // 5s 之后 被观察者状态发生改变。
        t.schedule(new TimerTask(){
        
            @Override
            public void run() {
                System.out.println("After 5 seconds ,notify all Observer");
                object.notifyObserverAll("update"); // 被观察者发生改变了，被观察者通知所有观察，
            }
        }, 5000);


        Thread.sleep(5000);
        t.cancel();

    }
}
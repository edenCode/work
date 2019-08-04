
/* 
    被观察者实例
    被观察发生变化时通知其他观察者
*/
 public class SubjectObject extends Subject{
    private String state;
    SubjectObject(){

    }

    public String getState(){
        return state;
    }

    public void change(String state){
        this.state = state;
        System.out.println("Subject Object changed");

        notifyObserverAll(state);
    }
}
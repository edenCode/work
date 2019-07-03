

//java.lang.Class
@Inter(msg="haha")
@SuppressWarnings
class Test{
    public static void main(String[] args) {

        System.setProperty("sun.misc.ProxyGenerator.saveGeneratedFiles","true");

        // Inter extends Annatation
        Inter annotation = Test.class.getAnnotation(Inter.class);
        System.out.println("Inter msg : "+ annotation.msg());        
    }
}
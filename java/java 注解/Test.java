

java.lang.Class
@Inter(msg="haha")
class Test{
    public static void main(String[] args) {
        System.setProperty("sun.misc.ProxyGenerator.saveGeneratedFiles", "true");

        Inter annotation = Test.class.getAnnotation(Inter.class);
        System.out.println("Inter msg : "+ annotation.msg());        
    }
}

import java.lang.reflect.*;


class Test{
    
    public static void main(String[] args) {
        //Class clazz;
        // Not import Student.class
        try {
            Class clazz = Class.forName("Student");    
            
            System.out.println("Class Name : \n \t" +clazz.getName());
            //System.out.println(clazz.getFields());
            Field [] fields = clazz.getDeclaredFields();
            
            System.out.println("Fileds : ");
            for(Field f: fields){
                System.out.println("\t"+f);
            }

            Constructor [] constructors = clazz.getDeclaredConstructors();
            System.out.println("Constructors : ");
            for(Constructor c : constructors){
                System.out.println("\t"+c);
            } 

            Method [] methods = clazz.getMethods();
            System.out.println("Methods : ");
            for(Method m: methods ){
                System.out.println("\t" + m);
            }
            //Integer
            Object obj = clazz.getDeclaredConstructor(String.class,int.class).newInstance("Gongcm",12);
            
            Method m1 = clazz.getDeclaredMethod("toString");
            System.out.println(m1.invoke(obj));
            
            Method m2 = clazz.getDeclaredMethod("setSex",int.class);
            m2.invoke(obj,1);
            System.out.println(m1.invoke(obj));

            Method m3 = clazz.getMethod("setAge",int.class);
            m3.invoke(obj,12);
            System.out.println(m1.invoke(obj));

            //System.out.println("load student ");
            
        } catch (Exception e) {
            //TODO: handle exception
            System.out.println(e.getMessage()+": " + e.toString());
        }

            //Object obj = clazz.newInstance();
        
        
        
    }
}
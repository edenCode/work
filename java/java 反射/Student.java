//package test;

class Student{
    private int age;
    private String name;
    private int sex;

    Student(){

    }

    Student(String name,int sex,int age){
        this.name = name;
        this.age = age;
    }

    Student(String name,int sex){
        this.name = name;
        this.sex = sex;
        
    }


    public void setAge(int age){
        this.age = age;
    }

    public void setSex(int sex){
        this.sex = sex;
    }

    @Override
    public String toString() {
        StringBuffer b = new StringBuffer();
        b.append("Name : ");
        b.append(name);
        b.append("\tAge : ");
        b.append(age);
        b.append("\tSex : ");
        b.append(sex);
    
        return b.toString();
    }
}
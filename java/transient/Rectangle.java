

import java.io.Serializable;
import java.lang.String;
import java.io.Serializable;

class Rectangle implements Serializable{

    private static final long serialVersionUID = 1223232313132313L;
    private int width;
    private int height;

    // area can not be serialized
    private transient int area;

    Rectangle(int width,int height){
        this.width = width;
        this.height = height;
        this.area = width * height;
    }

    public int setArea(){
        this.area = width * height;
        return this.area;
    }

    @Override
    public String toString() {
        StringBuffer buffer = new StringBuffer();
        buffer.append("\n\twidth : ");
        buffer.append(width);
        buffer.append("\t");
        buffer.append("height : ");
        buffer.append(height);
        buffer.append("\tarea : ");
        buffer.append(area);

        return buffer.toString();
    }

}
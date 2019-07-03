import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

//
@Retention(RetentionPolicy.RUNTIME)
public @interface Inter{
    String msg() default "aaa";
}

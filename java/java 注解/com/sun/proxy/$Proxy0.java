package com.sun.proxy;

import java.lang.reflect.*;
import java.lang.annotation.*;

public final class $Proxy0 extends Proxy implements Retention
{
    private static Method m1;
    private static Method m2;
    private static Method m4;
    private static Method m0;
    private static Method m3;
    
    public $Proxy0(final InvocationHandler invocationHandler) {
        super(invocationHandler);
    }
    
    @Override
    public final boolean equals(final Object o) {
        try {
            return (boolean)super.h.invoke(this, $Proxy0.m1, new Object[] { o });
        }
        catch (Error | RuntimeException error) {
            throw;
        }
        catch (Throwable t) {
            throw new UndeclaredThrowableException(t);
        }
    }
    
    @Override
    public final String toString() {
        try {
            return (String)super.h.invoke(this, $Proxy0.m2, null);
        }
        catch (Error | RuntimeException error) {
            throw;
        }
        catch (Throwable t) {
            throw new UndeclaredThrowableException(t);
        }
    }
    
    public final Class annotationType() {
        try {
            return (Class)super.h.invoke(this, $Proxy0.m4, null);
        }
        catch (Error | RuntimeException error) {
            throw;
        }
        catch (Throwable t) {
            throw new UndeclaredThrowableException(t);
        }
    }
    
    @Override
    public final int hashCode() {
        try {
            return (int)super.h.invoke(this, $Proxy0.m0, null);
        }
        catch (Error | RuntimeException error) {
            throw;
        }
        catch (Throwable t) {
            throw new UndeclaredThrowableException(t);
        }
    }
    
    public final RetentionPolicy value() {
        try {
            return (RetentionPolicy)super.h.invoke(this, $Proxy0.m3, null);
        }
        catch (Error | RuntimeException error) {
            throw;
        }
        catch (Throwable t) {
            throw new UndeclaredThrowableException(t);
        }
    }
    
    static {
        try {
            $Proxy0.m1 = Class.forName("java.lang.Object").getMethod("equals", Class.forName("java.lang.Object"));
            $Proxy0.m2 = Class.forName("java.lang.Object").getMethod("toString", (Class<?>[])new Class[0]);
            $Proxy0.m4 = Class.forName("java.lang.annotation.Retention").getMethod("annotationType", (Class<?>[])new Class[0]);
            $Proxy0.m0 = Class.forName("java.lang.Object").getMethod("hashCode", (Class<?>[])new Class[0]);
            $Proxy0.m3 = Class.forName("java.lang.annotation.Retention").getMethod("value", (Class<?>[])new Class[0]);
        }
        catch (NoSuchMethodException ex) {
            throw new NoSuchMethodError(ex.getMessage());
        }
        catch (ClassNotFoundException ex2) {
            throw new NoClassDefFoundError(ex2.getMessage());
        }
    }
}

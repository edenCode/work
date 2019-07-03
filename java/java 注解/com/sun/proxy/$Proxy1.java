package com.sun.proxy;

import java.lang.reflect.*;

public final class $Proxy1 extends Proxy implements Inter
{
    private static Method m1;
    private static Method m2;
    private static Method m3;
    private static Method m4;
    private static Method m0;
    
    public $Proxy1(final InvocationHandler invocationHandler) {
        super(invocationHandler);
    }
    
    public final boolean equals(final Object o) {
        try {
            return (boolean)super.h.invoke(this, $Proxy1.m1, new Object[] { o });
        }
        catch (Error | RuntimeException error) {
            throw;
        }
        catch (Throwable t) {
            throw new UndeclaredThrowableException(t);
        }
    }
    
    public final String toString() {
        try {
            return (String)super.h.invoke(this, $Proxy1.m2, null);
        }
        catch (Error | RuntimeException error) {
            throw;
        }
        catch (Throwable t) {
            throw new UndeclaredThrowableException(t);
        }
    }
    
    public final String msg() {
        try {
            return (String)super.h.invoke(this, $Proxy1.m3, null);
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
            return (Class)super.h.invoke(this, $Proxy1.m4, null);
        }
        catch (Error | RuntimeException error) {
            throw;
        }
        catch (Throwable t) {
            throw new UndeclaredThrowableException(t);
        }
    }
    
    public final int hashCode() {
        try {
            return (int)super.h.invoke(this, $Proxy1.m0, null);
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
            $Proxy1.m1 = Class.forName("java.lang.Object").getMethod("equals", Class.forName("java.lang.Object"));
            $Proxy1.m2 = Class.forName("java.lang.Object").getMethod("toString", (Class<?>[])new Class[0]);
            $Proxy1.m3 = Class.forName("Inter").getMethod("msg", (Class<?>[])new Class[0]);
            $Proxy1.m4 = Class.forName("Inter").getMethod("annotationType", (Class<?>[])new Class[0]);
            $Proxy1.m0 = Class.forName("java.lang.Object").getMethod("hashCode", (Class<?>[])new Class[0]);
        }
        catch (NoSuchMethodException ex) {
            throw new NoSuchMethodError(ex.getMessage());
        }
        catch (ClassNotFoundException ex2) {
            throw new NoClassDefFoundError(ex2.getMessage());
        }
    }
}

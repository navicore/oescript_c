/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.4
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.oescript.java.swig;

public class OesThreadDispatcher {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  public OesThreadDispatcher(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr(OesThreadDispatcher obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        oescriptimplJNI.delete_OesThreadDispatcher(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public OesThreadDispatcher(long nthreads) {
    this(oescriptimplJNI.new_OesThreadDispatcher(nthreads), true);
  }

  public void start() {
    oescriptimplJNI.OesThreadDispatcher_start(swigCPtr, this);
  }

}

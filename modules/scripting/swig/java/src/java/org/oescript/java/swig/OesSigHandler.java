/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.4
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.oescript.java.swig;

public class OesSigHandler {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  public OesSigHandler(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr(OesSigHandler obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        oescriptimplJNI.delete_OesSigHandler(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public OesSigHandler(String filename, String appname, String loglvl) {
    this(oescriptimplJNI.new_OesSigHandler(filename, appname, loglvl), true);
  }

  public void add(OesDispatcher manageme) {
    oescriptimplJNI.OesSigHandler_add__SWIG_0(swigCPtr, this, OesDispatcher.getCPtr(manageme), manageme);
  }

  public void add(OesThreadDispatcher manageme) {
    oescriptimplJNI.OesSigHandler_add__SWIG_1(swigCPtr, this, OesThreadDispatcher.getCPtr(manageme), manageme);
  }

  public void add(OesStore manageme) {
    oescriptimplJNI.OesSigHandler_add__SWIG_2(swigCPtr, this, OesStore.getCPtr(manageme), manageme);
  }

  public void add(OesServer manageme) {
    oescriptimplJNI.OesSigHandler_add__SWIG_3(swigCPtr, this, OesServer.getCPtr(manageme), manageme);
  }

  public void add(OesKernel manageme) {
    oescriptimplJNI.OesSigHandler_add__SWIG_4(swigCPtr, this, OesKernel.getCPtr(manageme), manageme);
  }

  public void add(OesNet manageme) {
    oescriptimplJNI.OesSigHandler_add__SWIG_5(swigCPtr, this, OesNet.getCPtr(manageme), manageme);
  }

}

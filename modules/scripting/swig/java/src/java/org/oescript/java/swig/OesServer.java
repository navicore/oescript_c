/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.4
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.oescript.java.swig;

public class OesServer {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  public OesServer(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr(OesServer obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        oescriptimplJNI.delete_OesServer(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public OesServer(OesDispatcher dispatcher, String spec, OesNet net, OesStore store) {
    this(oescriptimplJNI.new_OesServer(OesDispatcher.getCPtr(dispatcher), dispatcher, spec, OesNet.getCPtr(net), net, OesStore.getCPtr(store), store), true);
  }

  public void addLoginModule(OesLoginModule m) {
    oescriptimplJNI.OesServer_addLoginModule(swigCPtr, this, OesLoginModule.getCPtr(m), m);
  }

  public void addKernel(String extentname, OesKernel kernel) {
    oescriptimplJNI.OesServer_addKernel(swigCPtr, this, extentname, OesKernel.getCPtr(kernel), kernel);
  }

}

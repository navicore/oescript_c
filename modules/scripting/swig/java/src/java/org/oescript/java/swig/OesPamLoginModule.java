/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.4
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.oescript.java.swig;

public class OesPamLoginModule {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  public OesPamLoginModule(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr(OesPamLoginModule obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        oescriptimplJNI.delete_OesPamLoginModule(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public OesPamLoginModule(String pamdomain, String defgroup) {
    this(oescriptimplJNI.new_OesPamLoginModule(pamdomain, defgroup), true);
  }

  public int addPrivilege(String extentname, String groupname, String mtemplate) {
    return oescriptimplJNI.OesPamLoginModule_addPrivilege(swigCPtr, this, extentname, groupname, mtemplate);
  }

  public OesLoginModule getModule() {
    long cPtr = oescriptimplJNI.OesPamLoginModule_getModule(swigCPtr, this);
    return (cPtr == 0) ? null : new OesLoginModule(cPtr, false);
  }

}

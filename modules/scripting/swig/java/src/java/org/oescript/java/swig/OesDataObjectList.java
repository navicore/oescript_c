/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.4
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.oescript.java.swig;

public class OesDataObjectList {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  public OesDataObjectList(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr(OesDataObjectList obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        oescriptimplJNI.delete_OesDataObjectList(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public OesDataObjectList() {
    this(oescriptimplJNI.new_OesDataObjectList(), true);
  }

  public int length() {
    return oescriptimplJNI.OesDataObjectList_length(swigCPtr, this);
  }

  public OesListIterator iter() {
    long cPtr = oescriptimplJNI.OesDataObjectList_iter(swigCPtr, this);
    return (cPtr == 0) ? null : new OesListIterator(cPtr, false);
  }

  public OesDataObject newDataObject() {
    long cPtr = oescriptimplJNI.OesDataObjectList_newDataObject(swigCPtr, this);
    return (cPtr == 0) ? null : new OesDataObject(cPtr, false);
  }

}

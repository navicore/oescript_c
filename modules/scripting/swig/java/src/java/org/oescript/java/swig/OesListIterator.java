/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.4
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.oescript.java.swig;

public class OesListIterator {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  public OesListIterator(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr(OesListIterator obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        oescriptimplJNI.delete_OesListIterator(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public OesDataObject nextItem() {
    long cPtr = oescriptimplJNI.OesListIterator_nextItem(swigCPtr, this);
    return (cPtr == 0) ? null : new OesDataObject(cPtr, false);
  }

  public boolean hasMore() {
    return oescriptimplJNI.OesListIterator_hasMore(swigCPtr, this);
  }

  public OesListIterator() {
    this(oescriptimplJNI.new_OesListIterator(), true);
  }

}
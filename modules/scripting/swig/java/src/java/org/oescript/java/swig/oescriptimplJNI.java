/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.4
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.oescript.java.swig;

public class oescriptimplJNI {
  public final static native long new_OesDataObject();
  public final static native void delete_OesDataObject(long jarg1);
  public final static native int OesDataObject_get_nattrs(long jarg1, OesDataObject jarg1_);
  public final static native void OesDataObject_add_attr(long jarg1, OesDataObject jarg1_, String jarg2);
  public final static native void OesDataObject_set_nbytes(long jarg1, OesDataObject jarg1_, long jarg2);
  public final static native int OesDataObject_get_nbytes(long jarg1, OesDataObject jarg1_);
  public final static native long OesDataObject_iter(long jarg1, OesDataObject jarg1_);
  public final static native void OesDataObject_set_bytes(long jarg1, OesDataObject jarg1_, String jarg2);
  public final static native String OesDataObject_get_bytes(long jarg1, OesDataObject jarg1_);
  public final static native long new_OesSigHandler(String jarg1, String jarg2, String jarg3);
  public final static native void delete_OesSigHandler(long jarg1);
  public final static native void OesSigHandler_add__SWIG_0(long jarg1, OesSigHandler jarg1_, long jarg2, OesDispatcher jarg2_);
  public final static native void OesSigHandler_add__SWIG_1(long jarg1, OesSigHandler jarg1_, long jarg2, OesThreadDispatcher jarg2_);
  public final static native void OesSigHandler_add__SWIG_2(long jarg1, OesSigHandler jarg1_, long jarg2, OesStore jarg2_);
  public final static native void OesSigHandler_add__SWIG_3(long jarg1, OesSigHandler jarg1_, long jarg2, OesServer jarg2_);
  public final static native void OesSigHandler_add__SWIG_4(long jarg1, OesSigHandler jarg1_, long jarg2, OesKernel jarg2_);
  public final static native void OesSigHandler_add__SWIG_5(long jarg1, OesSigHandler jarg1_, long jarg2, OesNet jarg2_);
  public final static native long new_OesDispatcher(long jarg1, String jarg2);
  public final static native void delete_OesDispatcher(long jarg1);
  public final static native void OesDispatcher_start(long jarg1, OesDispatcher jarg1_);
  public final static native long new_OesThreadDispatcher(long jarg1);
  public final static native void delete_OesThreadDispatcher(long jarg1);
  public final static native void OesThreadDispatcher_start(long jarg1, OesThreadDispatcher jarg1_);
  public final static native long new_OesStore(int jarg1, String jarg2, boolean jarg3);
  public final static native void delete_OesStore(long jarg1);
  public final static native long new_OesNet(long jarg1, OesDispatcher jarg1_, boolean jarg2, String jarg3, String jarg4);
  public final static native void delete_OesNet(long jarg1);
  public final static native void OesNet_setName(long jarg1, OesNet jarg1_, String jarg2);
  public final static native String OesNet_getName(long jarg1, OesNet jarg1_);
  public final static native void delete_OesLoginModule(long jarg1);
  public final static native long new_OesLoginModule();
  public final static native long new_OesBuiltInLoginModule();
  public final static native void delete_OesBuiltInLoginModule(long jarg1);
  public final static native int OesBuiltInLoginModule_addAccount(long jarg1, OesBuiltInLoginModule jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native int OesBuiltInLoginModule_addPrivilege(long jarg1, OesBuiltInLoginModule jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native long OesBuiltInLoginModule_getModule(long jarg1, OesBuiltInLoginModule jarg1_);
  public final static native long new_OesPamLoginModule(String jarg1, String jarg2);
  public final static native void delete_OesPamLoginModule(long jarg1);
  public final static native int OesPamLoginModule_addPrivilege(long jarg1, OesPamLoginModule jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native long OesPamLoginModule_getModule(long jarg1, OesPamLoginModule jarg1_);
  public final static native long new_OesServer(long jarg1, OesDispatcher jarg1_, String jarg2, long jarg3, OesNet jarg3_, long jarg4, OesStore jarg4_);
  public final static native void delete_OesServer(long jarg1);
  public final static native void OesServer_addLoginModule(long jarg1, OesServer jarg1_, long jarg2, OesLoginModule jarg2_);
  public final static native void OesServer_addKernel(long jarg1, OesServer jarg1_, String jarg2, long jarg3, OesKernel jarg3_);
  public final static native long new_OesDataObjectList();
  public final static native void delete_OesDataObjectList(long jarg1);
  public final static native int OesDataObjectList_length(long jarg1, OesDataObjectList jarg1_);
  public final static native long OesDataObjectList_iter(long jarg1, OesDataObjectList jarg1_);
  public final static native long OesDataObjectList_newDataObject(long jarg1, OesDataObjectList jarg1_);
  public final static native void delete_OesIterator(long jarg1);
  public final static native String OesIterator_nextItem(long jarg1, OesIterator jarg1_);
  public final static native boolean OesIterator_hasMore(long jarg1, OesIterator jarg1_);
  public final static native long new_OesIterator();
  public final static native void delete_OesListIterator(long jarg1);
  public final static native long OesListIterator_nextItem(long jarg1, OesListIterator jarg1_);
  public final static native boolean OesListIterator_hasMore(long jarg1, OesListIterator jarg1_);
  public final static native long new_OesListIterator();
  public final static native void delete_OesLease(long jarg1);
  public final static native int OesLease_lid(long jarg1, OesLease jarg1_);
  public final static native void OesLease_cancel(long jarg1, OesLease jarg1_);
  public final static native long new_OesLease();
  public final static native void delete_OesTxn(long jarg1);
  public final static native int OesTxn_tid(long jarg1, OesTxn jarg1_);
  public final static native int OesTxn_status(long jarg1, OesTxn jarg1_);
  public final static native void OesTxn_commit(long jarg1, OesTxn jarg1_);
  public final static native void OesTxn_rollback(long jarg1, OesTxn jarg1_);
  public final static native long new_OesTxn();
  public final static native long new_OesKernel(long jarg1, OesDispatcher jarg1_, long jarg2, OesThreadDispatcher jarg2_, long jarg3, OesStore jarg3_, String jarg4, long jarg5, OesNet jarg5_, String jarg6, String jarg7);
  public final static native void delete_OesKernel(long jarg1);
  public final static native long OesKernel_write(long jarg1, OesKernel jarg1_, long jarg2, OesDataObject jarg2_, int jarg3, boolean jarg4);
  public final static native long OesKernel_begin(long jarg1, OesKernel jarg1_, int jarg2);
  public final static native void OesKernel_setCurrent(long jarg1, OesKernel jarg1_, long jarg2, OesTxn jarg2_);
  public final static native long OesKernel_read(long jarg1, OesKernel jarg1_, long jarg2, OesDataObjectList jarg2_, int jarg3, boolean jarg4, int jarg5, boolean jarg6, boolean jarg7);
  public final static native long OesKernel_take(long jarg1, OesKernel jarg1_, long jarg2, OesDataObjectList jarg2_, int jarg3, boolean jarg4, int jarg5, boolean jarg6, boolean jarg7);
  public final static native long new_OesJsonKernel(long jarg1, OesKernel jarg1_);
  public final static native void delete_OesJsonKernel(long jarg1);
  public final static native long OesJsonKernel_exec(long jarg1, OesJsonKernel jarg1_, String jarg2);
  public final static native void delete_OesFuture(long jarg1);
  public final static native String OesFuture_getString(long jarg1, OesFuture jarg1_, int jarg2);
  public final static native long new_OesFuture();
}

<?php

/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.4
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */

// Try to load our extension if it's not already loaded.
if (!extension_loaded('oescriptimpl')) {
  if (strtolower(substr(PHP_OS, 0, 3)) === 'win') {
    if (!dl('php_oescriptimpl.dll')) return;
  } else {
    // PHP_SHLIB_SUFFIX gives 'dylib' on MacOS X but modules are 'so'.
    if (PHP_SHLIB_SUFFIX === 'dylib') {
      if (!dl('oescriptimpl.so')) return;
    } else {
      if (!dl('oescriptimpl.'.PHP_SHLIB_SUFFIX)) return;
    }
  }
}



abstract class oescriptimpl {
	const ACTIVE = 0;

	const ROLLEDBACK = ROLLEDBACK;

	const COMMITTED = COMMITTED;
}

/* PHP Proxy Classes */
class OesDataObject {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($res=null) {
		if (is_resource($res) && get_resource_type($res) === '_p_OesDataObject') {
			$this->_cPtr=$res;
			return;
		}
		$this->_cPtr=new_OesDataObject();
	}

	function get_nattrs() {
		return OesDataObject_get_nattrs($this->_cPtr);
	}

	function add_attr($entry) {
		OesDataObject_add_attr($this->_cPtr,$entry);
	}

	function set_nbytes($nbytes) {
		OesDataObject_set_nbytes($this->_cPtr,$nbytes);
	}

	function get_nbytes() {
		return OesDataObject_get_nbytes($this->_cPtr);
	}

	function iter() {
		$r=OesDataObject_iter($this->_cPtr);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesIterator($r);
			}
			return new $c($r);
		}
		return $r;
	}

	function set_bytes($bytes) {
		OesDataObject_set_bytes($this->_cPtr,$bytes);
	}

	function get_bytes() {
		return OesDataObject_get_bytes($this->_cPtr);
	}
}

class OesSigHandler {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($filename,$appname,$loglvl) {
		if (is_resource($filename) && get_resource_type($filename) === '_p_OesSigHandler') {
			$this->_cPtr=$filename;
			return;
		}
		$this->_cPtr=new_OesSigHandler($filename,$appname,$loglvl);
	}

	function add($manageme) {
		OesSigHandler_add($this->_cPtr,$manageme);
	}
}

class OesDispatcher {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($nthreads,$name) {
		if (is_resource($nthreads) && get_resource_type($nthreads) === '_p_OesDispatcher') {
			$this->_cPtr=$nthreads;
			return;
		}
		$this->_cPtr=new_OesDispatcher($nthreads,$name);
	}

	function start() {
		OesDispatcher_start($this->_cPtr);
	}
}

class OesThreadDispatcher {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($nthreads) {
		if (is_resource($nthreads) && get_resource_type($nthreads) === '_p_OesThreadDispatcher') {
			$this->_cPtr=$nthreads;
			return;
		}
		$this->_cPtr=new_OesThreadDispatcher($nthreads);
	}

	function start() {
		OesThreadDispatcher_start($this->_cPtr);
	}
}

class OesStore {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($persist_level,$homedir,$threaded) {
		if (is_resource($persist_level) && get_resource_type($persist_level) === '_p_OesStore') {
			$this->_cPtr=$persist_level;
			return;
		}
		$this->_cPtr=new_OesStore($persist_level,$homedir,$threaded);
	}
}

class OesNet {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($d,$use_ssl,$cert,$pkey) {
		if (is_resource($d) && get_resource_type($d) === '_p_OesNet') {
			$this->_cPtr=$d;
			return;
		}
		$this->_cPtr=new_OesNet($d,$use_ssl,$cert,$pkey);
	}

	function setName($name) {
		OesNet_setName($this->_cPtr,$name);
	}

	function getName() {
		return OesNet_getName($this->_cPtr);
	}
}

class OesLoginModule {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($res=null) {
		if (is_resource($res) && get_resource_type($res) === '_p_OesLoginModule') {
			$this->_cPtr=$res;
			return;
		}
		$this->_cPtr=new_OesLoginModule();
	}
}

class OesBuiltInLoginModule {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($res=null) {
		if (is_resource($res) && get_resource_type($res) === '_p_OesBuiltInLoginModule') {
			$this->_cPtr=$res;
			return;
		}
		$this->_cPtr=new_OesBuiltInLoginModule();
	}

	function addAccount($groupname,$username,$pwd) {
		return OesBuiltInLoginModule_addAccount($this->_cPtr,$groupname,$username,$pwd);
	}

	function addPrivilege($extentname,$groupname,$mtemplate) {
		return OesBuiltInLoginModule_addPrivilege($this->_cPtr,$extentname,$groupname,$mtemplate);
	}

	function getModule() {
		$r=OesBuiltInLoginModule_getModule($this->_cPtr);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesLoginModule($r);
			}
			return new $c($r);
		}
		return $r;
	}
}

class OesPamLoginModule {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($pamdomain,$defgroup) {
		if (is_resource($pamdomain) && get_resource_type($pamdomain) === '_p_OesPamLoginModule') {
			$this->_cPtr=$pamdomain;
			return;
		}
		$this->_cPtr=new_OesPamLoginModule($pamdomain,$defgroup);
	}

	function addPrivilege($extentname,$groupname,$mtemplate) {
		return OesPamLoginModule_addPrivilege($this->_cPtr,$extentname,$groupname,$mtemplate);
	}

	function getModule() {
		$r=OesPamLoginModule_getModule($this->_cPtr);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesLoginModule($r);
			}
			return new $c($r);
		}
		return $r;
	}
}

class OesServer {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($dispatcher,$spec,$net,$store) {
		if (is_resource($dispatcher) && get_resource_type($dispatcher) === '_p_OesServer') {
			$this->_cPtr=$dispatcher;
			return;
		}
		$this->_cPtr=new_OesServer($dispatcher,$spec,$net,$store);
	}

	function addLoginModule($m) {
		OesServer_addLoginModule($this->_cPtr,$m);
	}

	function addKernel($extentname,$kernel) {
		OesServer_addKernel($this->_cPtr,$extentname,$kernel);
	}
}

class OesDataObjectList {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($res=null) {
		if (is_resource($res) && get_resource_type($res) === '_p_OesDataObjectList') {
			$this->_cPtr=$res;
			return;
		}
		$this->_cPtr=new_OesDataObjectList();
	}

	function length() {
		return OesDataObjectList_length($this->_cPtr);
	}

	function iter() {
		$r=OesDataObjectList_iter($this->_cPtr);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesListIterator($r);
			}
			return new $c($r);
		}
		return $r;
	}

	function newDataObject() {
		$r=OesDataObjectList_newDataObject($this->_cPtr);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesDataObject($r);
			}
			return new $c($r);
		}
		return $r;
	}
}

class OesIterator {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function nextItem() {
		return OesIterator_nextItem($this->_cPtr);
	}

	function hasMore() {
		return OesIterator_hasMore($this->_cPtr);
	}

	function __construct($res=null) {
		if (is_resource($res) && get_resource_type($res) === '_p_OesIterator') {
			$this->_cPtr=$res;
			return;
		}
		$this->_cPtr=new_OesIterator();
	}
}

class OesListIterator {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function nextItem() {
		$r=OesListIterator_nextItem($this->_cPtr);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesDataObject($r);
			}
			return new $c($r);
		}
		return $r;
	}

	function hasMore() {
		return OesListIterator_hasMore($this->_cPtr);
	}

	function __construct($res=null) {
		if (is_resource($res) && get_resource_type($res) === '_p_OesListIterator') {
			$this->_cPtr=$res;
			return;
		}
		$this->_cPtr=new_OesListIterator();
	}
}

class OesLease {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function lid() {
		return OesLease_lid($this->_cPtr);
	}

	function cancel() {
		OesLease_cancel($this->_cPtr);
	}

	function __construct($res=null) {
		if (is_resource($res) && get_resource_type($res) === '_p_OesLease') {
			$this->_cPtr=$res;
			return;
		}
		$this->_cPtr=new_OesLease();
	}
}

class OesTxn {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function tid() {
		return OesTxn_tid($this->_cPtr);
	}

	function status() {
		return OesTxn_status($this->_cPtr);
	}

	function commit() {
		OesTxn_commit($this->_cPtr);
	}

	function rollback() {
		OesTxn_rollback($this->_cPtr);
	}

	function __construct($res=null) {
		if (is_resource($res) && get_resource_type($res) === '_p_OesTxn') {
			$this->_cPtr=$res;
			return;
		}
		$this->_cPtr=new_OesTxn();
	}
}

class OesKernel {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($dispatcher,$tdispatcher,$store,$netspec,$net,$username,$pwd) {
		if (is_resource($dispatcher) && get_resource_type($dispatcher) === '_p_OesKernel') {
			$this->_cPtr=$dispatcher;
			return;
		}
		$this->_cPtr=new_OesKernel($dispatcher,$tdispatcher,$store,$netspec,$net,$username,$pwd);
	}

	function write($slist,$dur,$is_tuple) {
		$r=OesKernel_write($this->_cPtr,$slist,$dur,$is_tuple);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesLease($r);
			}
			return new $c($r);
		}
		return $r;
	}

	function begin($dur) {
		$r=OesKernel_begin($this->_cPtr,$dur);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesTxn($r);
			}
			return new $c($r);
		}
		return $r;
	}

	function setCurrent($txn_holder) {
		OesKernel_setCurrent($this->_cPtr,$txn_holder);
	}

	function read($data,$timeout,$is_tuple,$max_responses,$return_attrs,$return_bytes) {
		$r=OesKernel_read($this->_cPtr,$data,$timeout,$is_tuple,$max_responses,$return_attrs,$return_bytes);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesDataObjectList($r);
			}
			return new $c($r);
		}
		return $r;
	}

	function take($data,$timeout,$is_tuple,$max_responses,$return_attrs,$return_bytes) {
		$r=OesKernel_take($this->_cPtr,$data,$timeout,$is_tuple,$max_responses,$return_attrs,$return_bytes);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesDataObjectList($r);
			}
			return new $c($r);
		}
		return $r;
	}
}

class OesJsonKernel {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function __construct($kernel) {
		if (is_resource($kernel) && get_resource_type($kernel) === '_p_OesJsonKernel') {
			$this->_cPtr=$kernel;
			return;
		}
		$this->_cPtr=new_OesJsonKernel($kernel);
	}

	function exec($json) {
		$r=OesJsonKernel_exec($this->_cPtr,$json);
		if (is_resource($r)) {
			$c=substr(get_resource_type($r), (strpos(get_resource_type($r), '__') ? strpos(get_resource_type($r), '__') + 2 : 3));
			if (!class_exists($c)) {
				return new OesFuture($r);
			}
			return new $c($r);
		}
		return $r;
	}
}

class OesFuture {
	public $_cPtr=null;
	protected $_pData=array();

	function __set($var,$value) {
		if ($var === 'thisown') return swig_oescriptimpl_alter_newobject($this->_cPtr,$value);
		$this->_pData[$var] = $value;
	}

	function __isset($var) {
		if ($var === 'thisown') return true;
		return array_key_exists($var, $this->_pData);
	}

	function __get($var) {
		if ($var === 'thisown') return swig_oescriptimpl_get_newobject($this->_cPtr);
		return $this->_pData[$var];
	}

	function getString($timeout) {
		return OesFuture_getString($this->_cPtr,$timeout);
	}

	function __construct($res=null) {
		if (is_resource($res) && get_resource_type($res) === '_p_OesFuture') {
			$this->_cPtr=$res;
			return;
		}
		$this->_cPtr=new_OesFuture();
	}
}


?>

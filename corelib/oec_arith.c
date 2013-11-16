/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

static char rcsid[] = "$Id: H:/drh/idioms/book/RCS/inter.doc,v 1.11 1997/02/21 19:42:15 drh Exp $";
/*
  file forked and renamed 9/2008 for onextent name space and inclusion in onextent
  open source software.  see file cii_COPYRIGHT.txt for dhr's license.
*/
#include "arith.h"
int arith_max(int x, int y) {
	return x > y ? x : y;
}
int arith_min(int x, int y) {
	return x > y ? y : x;
}
int arith_div(int x, int y) {
	if (-13/5 == -2
	&&	(x < 0) != (y < 0) && x%y != 0)
		return x/y - 1;
	else
		return x/y;
}
int arith_mod(int x, int y) {
	if (-13/5 == -2
	&&	(x < 0) != (y < 0) && x%y != 0)
		return x%y + y;
	else
		return x%y;
}
int arith_floor(int x, int y) {
	return arith_div(x, y);
}
int arith_ceiling(int x, int y) {
	return arith_div(x, y) + (x%y != 0);
}

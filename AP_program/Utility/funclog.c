/*
Copyright (c) 2016 National Central University, Studio215 ALL RIGHTS RESERVED

CAPCAM function logger

ver 0.0.1

Author: Chia-Ray Lin

Input  : 
	1. Use as printf function.
		
Output : Output message and time by the way you #define.
	Default file name is CAPCAM_func.log

Attribute:
1. Just use log_debug() same as printf().
2. Use %s the output __FUNC__, which is the function name writing the log.
*/

#include "funclog.h"
#include <assert.h>
#include <stdarg.h>

void funclog (const char *format,... )
{	
	if(format==NULL){
		return;
	}

	va_list args;
	memset(&args,0,sizeof(va_list));
	
	va_start(args, format);
	vprintf(format, args);//print information on console
	va_end(args);
}

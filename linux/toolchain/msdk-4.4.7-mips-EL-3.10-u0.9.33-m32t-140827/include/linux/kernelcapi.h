/*
 * $Id: kernelcapi.h,v 1.1.1.1 2016/11/01 07:34:02 hyrtlac Exp $
 * 
 * Kernel CAPI 2.0 Interface for Linux
 * 
 * (c) Copyright 1997 by Carsten Paeth (calle@calle.in-berlin.de)
 * 
 */

#ifndef __KERNELCAPI_H__
#define __KERNELCAPI_H__

#define CAPI_MAXAPPL	240	/* maximum number of applications  */
#define CAPI_MAXCONTR	32	/* maximum number of controller    */
#define CAPI_MAXDATAWINDOW	8


typedef struct kcapi_flagdef {
	int contr;
	int flag;
} kcapi_flagdef;

typedef struct kcapi_carddef {
	char		driver[32];
	unsigned int	port;
	unsigned	irq;
	unsigned int	membase;
	int		cardnr;
} kcapi_carddef;

/* new ioctls >= 10 */
#define KCAPI_CMD_TRACE		10
#define KCAPI_CMD_ADDCARD	11	/* OBSOLETE */

/* 
 * flag > 2 => trace also data
 * flag & 1 => show trace
 */
#define KCAPI_TRACE_OFF			0
#define KCAPI_TRACE_SHORT_NO_DATA	1
#define KCAPI_TRACE_FULL_NO_DATA	2
#define KCAPI_TRACE_SHORT		3
#define KCAPI_TRACE_FULL		4



#endif /* __KERNELCAPI_H__ */
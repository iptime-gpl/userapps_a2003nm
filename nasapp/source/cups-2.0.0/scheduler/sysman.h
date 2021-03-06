/*
 * "$Id: sysman.h,v 1.1.1.1 2014/12/23 05:45:35 thki81 Exp $"
 *
 * System management definitions for the CUPS scheduler.
 *
 * Copyright 2007-2014 by Apple Inc.
 * Copyright 2006 by Easy Software Products.
 *
 * These coded instructions, statements, and computer programs are the
 * property of Apple Inc. and are protected by Federal copyright
 * law.  Distribution and use rights are outlined in the file "LICENSE.txt"
 * which should have been included with this file.  If this file is
 * file is missing or damaged, see the license at "http://www.cups.org/".
 */

/*
 * Constants...
 */

#define CUPSD_DIRTY_NONE	0	/* Nothing is dirty */
#define CUPSD_DIRTY_PRINTERS	1	/* printers.conf is dirty */
#define CUPSD_DIRTY_CLASSES	2	/* classes.conf is dirty */
#define CUPSD_DIRTY_PRINTCAP	4	/* printcap is dirty */
#define CUPSD_DIRTY_JOBS	8	/* jobs.cache or "c" file(s) are dirty */
#define CUPSD_DIRTY_SUBSCRIPTIONS 16	/* subscriptions.conf is dirty */


/*
 * Globals...
 */

VAR int			DirtyFiles	VALUE(CUPSD_DIRTY_NONE),
					/* What files are dirty? */
			DirtyCleanInterval VALUE(DEFAULT_KEEPALIVE);
					/* How often do we write dirty files? */
VAR time_t		DirtyCleanTime	VALUE(0);
					/* When to clean dirty files next */
VAR int			ACPower		VALUE(-1),
					/* Is the system on AC power? */
			Sleeping	VALUE(0);
					/* Non-zero if machine is entering or *
					 * in a sleep state...                */
VAR time_t		SleepJobs	VALUE(0);
					/* Time when all jobs must be         *
					 * canceled for system sleep.         */
#ifdef __APPLE__
VAR int			SysEventPipes[2] VALUE2(-1,-1);
					/* System event notification pipes */
#endif	/* __APPLE__ */


/*
 * Prototypes...
 */

extern void	cupsdAllowSleep(void);
extern void	cupsdCleanDirty(void);
extern void	cupsdMarkDirty(int what);
extern void	cupsdSetBusyState(void);
extern void	cupsdStartSystemMonitor(void);
extern void	cupsdStopSystemMonitor(void);


/*
 * End of "$Id: sysman.h,v 1.1.1.1 2014/12/23 05:45:35 thki81 Exp $".
 */

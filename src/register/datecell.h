
#ifndef __XACC_DATE_CELL_C__
#define __XACC_DATE_CELL_C__

#include "single.h"

/*
typedef struct _DateCell {
   SingleCell cell;
   double amount;
} DateCell;
*/

/* installs a callback to handle date recording */
SingleCell * xaccMallocDateCell (void);
void         xaccInitDateCell (SingleCell *);

#endif /* __XACC_DATE_CELL_C__ */

/* --------------- end of file ---------------------- */

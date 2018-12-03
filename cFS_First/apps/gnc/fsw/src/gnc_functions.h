/*
** $Id: $
**
** Purpose:  cFE Application "template" (GNC) header file
**
** Author: Shinhye Moon  
**
** Notes:
**
** $Log: $
**
*/


#ifndef _gnc_functions_h_
#define _gnc_functions_h_



#include "cfe.h"
#include "gnc_msgids.h"
#include "gnc_perfids.h"

void GNC_OEM719_Signal_Output(void);
void GNC_Orbit_Update(void);
double* GNC_GPS_Solution(void);
void GNC_GPS_OD(double*GPS_sol);


#endif /* _gnc_app_ */

/************************/
/*  End of File Comment */
/************************/

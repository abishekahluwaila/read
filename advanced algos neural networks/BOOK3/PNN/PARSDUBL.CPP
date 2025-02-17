// Copyright (c) 1994 John Wiley & Sons, Inc.  All rights reserved.
// Reproduction or translation of this work beyond that permitted in
// section 117 of the 1976 United States Copyright Act without the
// express written permission of the copyright owner is unlawful.
// Requests for further information should be addressed to the
// Permission Department, John Wiley & Sons, Inc.  The purchaser may
// make backup copies for his/her own use only and not for distribution
// or resale.  The publisher assumes no responsibility for errors,
// omissions, or damages, caused by the use of these programs or from
// the use of the information contained herein.

/******************************************************************************/
/*                                                                            */
/*  PARSDUBL - ParseDouble routine to parse a double from a string            */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "const.h"       // System and limitation constants, typedefs, structs
#include "classes.h"     // Includes all class headers
#include "funcdefs.h"    // Function prototypes

double ParseDouble ( char **str )
{
   double num ;

   while (! (isdigit ( **str )  ||  (**str == '-')  ||  (**str == '.'))) {
      if (**str)
         ++(*str) ;
      else
         return 0. ;
      }

   num = atof ( *str ) ;

   while ((isdigit ( **str )  ||  (**str == '-')  ||  (**str == '.')))
      ++(*str) ;

   return num ;
}


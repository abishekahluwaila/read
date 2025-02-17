/******************************************************************************/
/*                                                                            */
/*  MOV_AVG - Apply a moving average filter to a signal.                      */
/*  Normally, this returns 0.  It returns 1 if insufficient memory.           */
/*                                                                            */
/* Copyright (c) 1995 Timothy Masters.  All rights reserved.                  */
/* Reproduction or translation of this work beyond that permitted in section  */
/* 117 of the 1976 United States Copyright Act without the express written    */
/* permission of the copyright owner is unlawful.  Requests for further       */
/* information should be addressed to the Permissions Department, John Wiley  */
/* & Sons, Inc.  The purchaser may make backup copies for his/her own use     */
/* only and not for distribution or resale.                                   */
/* Neither the author nor the publisher assumes responsibility for errors,    */
/* omissions, or damages, caused by the use of these programs or from the     */
/* use of the information contained herein.                                   */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>
#include "const.h"     // System, limitation constants, typedefs, structs
#include "classes.h"   // Includes all class headers
#include "funcdefs.h"  // Function prototypes

int mov_avg ( MiscParams *misc , Signal *sig , int period ,
              int *nsigs , Signal ***signals , char *error )
{
   int i, j, n, ivar, nvars ;
   double *data, *temp, *x, sum ;
   Signal **sptr ;
   enum SignalType save_type ;

   nvars = misc->names->nreal ;

   if (! nvars) {
      strcpy ( error , "No signal names specified" ) ;
      return 1 ;
      }

   n = sig->n ;
   x = sig->sig ;
   save_type = sig->type ;

   MEMTEXT ( "MOV_AVG: data" ) ;
   data = (double *) MALLOC ( n * sizeof(double) ) ;
   if (data == NULL) {
      strcpy ( error , "Insufficient memory to filter signal" ) ;
      return 1 ;
      }

/*
   Apply the filter.
   Note that this method is inefficient in that the complete sum is computed
   for each output point.  A more efficient method is to add a new point and
   subtract an old one each time.  The time difference in practice is so
   tiny that it is not worth complicating the demonstration.  Time fanatics
   may feel free to change the code.
*/

   for (i=0 ; i<period/2 ; i++) {  // Tails
      sum = 0.0 ;
      for (j=0 ; j<=i+period/2 ; j++)
         sum += x[j] ;
      data[i] = sum / (i+period/2+1) ;

      sum = 0.0 ;
      for (j=0 ; j<=i+period/2 ; j++)
         sum += x[n-j-1] ;
      data[n-i-1] = sum / (i+period/2+1) ;
      }

   if (period % 2) {      // Odd period
      for (i=period/2 ; i<n-period/2 ; i++) {
         sum = 0.0 ;
         for (j=i-period/2 ; j<=i+period/2 ; j++)
            sum += x[j] ;
         data[i] = sum / period ;
         }
      }

   else {                 // Even period
      for (i=period/2 ; i<n-period/2 ; i++) {
         sum = 0.5 * (x[i-period/2] + x[i+period/2]) ;
         for (j=i-period/2+1 ; j<i+period/2 ; j++)
            sum += x[j] ;
         data[i] = sum / period ;
         }
      }

/*
   Count how many of these signals have names not already in use.
   Then allocate additional memory for their pointers.
*/

   MEMTEXT ( "MOV_AVG: signals array" ) ;
   if (*nsigs) {                             // If signals already exist
      ivar = *nsigs ;                        // This many signals so far
      sptr = *signals ;                      // Array of pointers to them
      for (i=0 ; i<misc->names->n ; i++) {   // Check every new name
         if (! misc->names->len[i])          // Some may be NULL
            continue ;                       // Obviously skip them
         for (j=*nsigs-1 ; j>=0 ; j--) {     // Check every existing signal
            if (! strcmp ( misc->names->start[i] , sptr[j]->name )) // There?
               break ;                       // If found, quit looking
            }
         if (j < 0)                          // Means not there
            ++ivar ;                         // So count this new entry
         }
      sptr = (Signal **) REALLOC ( sptr , ivar * sizeof(Signal *) ) ;
      }
   else
      sptr = (Signal **) MALLOC ( nvars * sizeof(Signal *) ) ;

   if (sptr == NULL) {
      FREE ( data ) ;
      strcpy ( error , "Insufficient memory to create signal" ) ;
      return 1 ;
      }
   *signals = sptr ;

/*
   Now create new signals for each variable.
   If a signal of the same name exists, delete it first.
*/

   ivar = 0 ;
   for (i=0 ; i<misc->names->n ; i++) { // Check all names
      if (! misc->names->len[i])        // Some may be NULL
         continue ;                     // Obviously skip them
      for (j=*nsigs-1 ; j>=0 ; j--) {   // Search existing signals for same name
         if (! strcmp ( misc->names->start[i] , sptr[j]->name )) {  // There?
            MEMTEXT ( "MOV_AVG: delete duplicate signal" ) ;
            delete ( sptr[j] ) ;        // If so, delete this signal
            break ;                     // And quit looking
            }
         }
      if (j < 0) {                      // Means new, unique name
         j = *nsigs ;                   // Tack it onto end of signal array
         ++*nsigs ;                     // And count it
         }

      if (ivar) {   // In this case, must allocate for new signal
         MEMTEXT ( "MOV_AVG: temp signal" ) ;
         temp = (double *) MALLOC ( n * sizeof(double) ) ;
         if (temp == NULL) {
            strcpy ( error , "Insufficient memory to create signal" ) ;
            return 1 ;
            }
         memcpy ( temp , data , n * sizeof(double) ) ;
         }
      else
         temp = data ;

      MEMTEXT ( "MOV_AVG: new Signal" ) ;
      sptr[j] = new Signal ( misc->names->start[i] , n , temp ) ;
      if ((sptr[j] == NULL)  ||  ! sptr[j]->n) {
         if (sptr[j] != NULL) {
            delete sptr[j] ;
            sptr[j] = NULL ;
            }
         strcpy ( error , "Insufficient memory to create signal" ) ;
         return 1 ;
         }
      if (save_type == SpectrumSignal)
         sptr[j]->type = SpectrumSignal ;
      ++ivar ;
      } // For all names

   return 0 ;
}




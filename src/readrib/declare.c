/* $RCSfile: declare.c,v $  $Revision: 1.7 $ $Date: 1999/10/16 09:15:19 $
 *
 * Copyright (c) 1995, 1996, 1997, 1998 Thomas E. Burge.  All rights reserved.
 * 
 * Affine (R) is a registered trademark of Thomas E. Burge
 *
 * THIS SOFTWARE IS DISTRIBUTED "AS-IS" WITHOUT WARRANTY OF ANY KIND
 * AND WITHOUT ANY GUARANTEE OF MERCHANTABILITY OR FITNESS FOR A 
 * PARTICULAR PURPOSE.  
 *
 * In no event shall Thomas E. Burge be liable for any indirect or
 * consequential damages or loss of data resulting from use or performance 
 * of this software.
 * 
 * Permission is granted to include compiled versions of this code in
 * noncommercially sold software provided the following copyrights and
 * notices appear in all software and any related documentation:
 *
 *                 The Affine (R) Libraries and Tools are 
 *          Copyright (c) 1995, 1996, 1997, 1998 Thomas E. Burge.  
 *                          All rights reserved.
 *         Affine (R) is a registered trademark of Thomas E. Burge.
 *
 * Also refer to any additional requirements presently set by Pixar 
 * in regards to the RenderMan (R) Interface Procedures and Protocol.
 *
 * Those wishing to distribute this software commercially and those wishing 
 * to redistribute the source code must get written permission from the 
 * author, Thomas E. Burge.  
 *
 * Basically for now, I would like folks to get the source code directly 
 * from me rather than to have a bunch of different versions circulating
 * about.
 *
 *
 * Affine Toolkit
 *
 * FILE:  declare.c
 *
 * DESCRIPTION:  Declaration function.
 *   
 *    Contains:
 * 
 *    References:
 *          [PIXA89]  Pixar Animation Studios, The RenderMan Interface, 
 *                    Version 3.1, Richmond, CA, September 1989.
 *
 *    History:
 *                 RibDeclare() was in readrib/, then in ribhash/ and 
 *                 back in readrib/.  RibDeclare() doesn't fit nicely
 *                 into ribhash/ because there are to many error codes
 *                 for specific problems readrib/ would want feedback on.
 *       06-11-99  Finalized RibDeclare changes to handle inline 
 *                 declarations and better subclassing by making an
 *                 an extra copy of name.  
 *       10-10-99  Commented out warning about system declared token being
 *                 redclared.  It just seemed annoying to have.
 *       10-14-99  Moved commenting out to error.c.  Now someone can setup
 *                 an error handler if they want to set redeclaration errors
 *                 printed.
 *       05-30-06  Corrected a conflict naming with the type modifier
 *                 inline and the inline list. The list is now called as
 *                 inlined.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             RenderMan (R) is a registered trademark of Pixar
 *
 */
#define __RIB_FILE_DECLARE
#define _RIPRIV_TYPES
#define  _RIPRIV_FUNC_TYPES 
#include <string.h>
#include <ctype.h>
#include <ripriv.h>
#include <ribrdr.h>



static int HandleExpandInlineOption( PRIB_INSTANCE rib, PRIB_HASHATOM a )
{
   auto unsigned int   l;
   char  *na; /* Temp token name.  */
   char  *cl; /* Temp class name.  */
   char  *ty; /* Temp type name.   */
   char  *de; /* Temp declaration. */
   

   /* Make a copy of the name. */
   l = strlen( a->data ) + 1;
   na = (char*)_RibMalloc( l );
   if (!na)
      return 1;
   strcpy( na, a->data );

   /* Make a copy of the declaration string. */
   RibGetClassTypeNames( a->code, &cl, &ty );

   /* Calculate length of declaration string.  The +1 at end is for '\0'. */
   l = strlen(cl) + strlen(ty) + ( a->with.n>1 ? a->with.n/10 + 1 : 0 ) + 1;
   
   de = (char*)_RibMalloc( l );
   if (!de)
   {
      _RibFree(na);
      return 1;
   }
   
   if (a->with.n>1)
      sprintf( de, "%s %s[%u]", cl, ty, a->with.n );
   else
      sprintf( de, "%s %s", cl, ty );

   /* Send a Declare statement down the chain, so everyone knows
    *   what the new class and type is for the token.
    */
   ((PRiDeclare)*rib->ritable[kRIB_DECLARE])( na, de );
   
   if ( RibShouldFreeData(rib) )
   {
#ifdef RIB_MEMORY_TESTS
      RibMemoryTestStepOverAllowed();
#endif
      _RibFree(de);
      _RibFree(na);
   }

   return 0;
}

/* RibDeclare()
 *
 *    No longer assumes that name was a malloced piece of data that 
 *    is freeable.
 *
 *    The library libribhash.a just calls RibAddItem() directly.  
 *    RibDeclare() is really provided for the libraries libribrdr.a,
 *    libribwrtr.a and libributil.a to use.
 *
 */
PRIB_HASHATOM RibDeclare( RIB_HANDLE hrib, char *name, char *declaration )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   char                *p;
   unsigned int        classNtype;
   PRIB_HASHATOM       a;
   PRIB_INLINEDECL     inlined = NULL;
   auto int            i;
   auto unsigned int   l;
   auto char           *c;
   auto int            n;
   

   /* If declaration is NULL then assume it is an inlined declaration and
    *   the string name has the class and type along with the name.
    */
   p = ( declaration ? declaration : name );
   while( isspace(*p) )
     p++;
   
   /* Get class. */
   i = RibReadStringForToken( p, gRibClassTable, &p );
   if (i<0)
   {
      /* Nothing was found to match in the table, so assume
       *   that the class was not specified and what was
       *   just checked is actually the name of the type.
       */
      i = 0;
      classNtype = kRIB_UNIFORMCLASS; /* Default to uniform class. */

      /* Reset p to be at the beginning again. */
      p = ( declaration ? declaration : name ); 
   }
   else
   {
      i = RibIntToClassEnum( i+1 /* Enums aren't zero based, so add one. */ );
      classNtype = i;
   }

   /* If i was set to a class then there should be whitespace. */
   if ( i!=0 && !isspace(*p) )
   {
      rib->error.type = kRIB_ERRTYPE_BAD_CLASSNAME;
      rib->error.expected.sys_string = name;
      rib->error.unexpected.user_string = declaration;
      RibSetError( rib, RIE_CONSISTENCY, RIE_WARNING, &(rib->error) );

      return kRIB_ERRRC_PTR; 
   }

   while( isspace(*p) )
     p++;

   /* Get type. */
   i = RibReadStringForToken( p, gRibTypeTable, &p );
   if (i<0)
   {
      /* Only report an error if not in-line declaration. */
      if (declaration)
      {
	 rib->error.type = kRIB_ERRTYPE_BAD_TYPENAME;
	 rib->error.expected.sys_string = name;
	 rib->error.unexpected.user_string = declaration;
	 RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );
      }

      return kRIB_ERRRC_PTR; 
   }
   classNtype |= i+1;

   /* Get array depth.  Default to 1. */
   if ( *p == '[' )
   {
      p++;
      n = 0;
      i = 0;

      while ( *p >= '0' && *p <= '9' && i < 5 )
      {
         n = n*10 + (*p-'0');
         p++;
         i++;
      }

      if ( *p != ']' || n == 0 )
        return kRIB_ERRRC_PTR; /* error bad array number given */

      p++;

      if ( n==2 )
      {
         i = classNtype & kRIB_TYPE_MASK;
         
         if ( i == kRIB_FLOATTYPE )
         {
            classNtype = (classNtype & ~kRIB_TYPE_MASK) | kRIB_FLOATPAIRTYPE;
	    n = 1;
         }
         else if ( i == kRIB_INTTYPE )
         {
            classNtype = (classNtype & ~kRIB_TYPE_MASK) | kRIB_INTPAIRTYPE;
	    n = 1;
         }
         else
         {
            classNtype |= kRIB_ARRAY_DECLARED;
         }
      }
      else
      {
         classNtype |= kRIB_ARRAY_DECLARED;
      }
   }
   else
   {
      n = 1;
   }

   while( isspace(*p) )
     p++;

   if (!declaration)
   {
      inlined = (PRIB_INLINEDECL)_RibMalloc( sizeof(RIB_INLINEDECL) );
      if (!inlined)
	 return NULL;

      inlined->next = rib->inlinelist;
      rib->inlinelist = inlined;

      while( isspace(*p) )
	 p++;

      /* Have name point passed the inlined declaration to the actual
       *    variable name being declared.
       */
      name = p;      
   }

   /* Add declaration to the hash table.  */
   if (n==1) /* Check if an array type is being declared. */
   {
      /*
       * (type == kRIB_HASH_VARIABLE) && !(classNtype & kRIB_ARRAY_DECLARED)
       *   RibAddItem( RIB_HASHHND hash, int type, int classNtype,
       *               char *variablename );
       */
      a = RibAddItem( rib->phashtable, 
                     kRIB_HASH_VARIABLE | kRIB_HASH_FREEDATA, 
                     classNtype, name );
   }
   else
   {
      /*
       * (type == kRIB_HASH_VARIABLE) && (classNtype & kRIB_ARRAY_DECLARED)
       *   RibAddItem( RIB_HASHHND hash, int type, int classNtype,
       *               char *variablename, int ntype );
       */
      a = RibAddItem( rib->phashtable, 
                     kRIB_HASH_VARIABLE | kRIB_HASH_FREEDATA,
                     classNtype, name, n );
   }

   if (!a)
   {
      a = RibGetConflictingHashAtom( rib->phashtable );
      if (!a)
      {
         /* No already existing atom to get in the way, so there must have
          *    been a problem adding to the hash table.
          */
         rib->error.type = kRIB_ERRTYPE_ADDING_TO_HASH;
         RibSetError( rib, RIE_SYNTAX, RIE_ERROR, &(rib->error) );

	 if (inlined)
	 {
	    rib->inlinelist = inlined->next;
	    _RibFree(inlined);
	 }
	 return NULL;
      }

      /* If in-line, save off the data. */
      if (inlined)
      {
	 /* Note that inlined->code does not get kRIB_INLINE_DECLARED
	  *   so that RibUndoInLineList() knows that an atom existed
	  *   before that should be restored to it's old code and n
	  *   values instead of being deleted from the hash table.
	  */
	 inlined->code = a->code;
	 inlined->n = a->with.n;
	 inlined->atom = a;
      }

      /* The name has already been declared. */
      a->with.n = n;

      /* Don't print a warning if this is an inlined declaration.
       *    Found it to be too annoying:
       *
       *      WARNING: "inline.rib", line 2: byte offset 79: Parameters 
       *          inconsistent.
       *      System declared parameter "roughness" previously declared 
       *          "uniform float" is redeclared as "uniform color".
       */
      /* 10-10-99  Comment out this warning as it seems annoying. */
      /* 10-14-99  Moved to error.c to allow error handlers to decide
       *           whether or not to print something.
       */
      if ( a->code & kRIB_SYS_DECLARED /*&& !inlined*/ )
      {
         rib->error.type = kRIB_ERRTYPE_SYSTEM_PARAM;
         rib->error.var = a;
         rib->error.unexpected.user_int = classNtype;
         RibSetError( rib, RIE_CONSISTENCY, RIE_WARNING, &(rib->error) );
      }
      a->code = classNtype;
   }
   else
   {
      /* If in-line, save off the data. */
      if (inlined)
      {
	 /* Inline item has never been declared before and is therefore
	  *   not already in the hash table.
	  */
	 inlined->code = a->code | kRIB_INLINE_DECLARED;
	 inlined->n = a->with.n;
	 inlined->atom = a;
      }

      /* Change the data pointer to be a copy of name. */
      l = strlen( name ) + 1;
      c = (char*)_RibMalloc( l );
      if (!c)
	 goto Error;
      strcpy( c, name );
      a->data = c;
   }

   /* The user may have an old client library that doesn't understand
    *    token names with inlined declarations.  To get around that 
    *    use the kRIB_OPTION_EXPAND_INLINE option which causes a before
    *    and after declaration to be used with simple token names 
    *    going down to the client library.
    */
   if ( rib->options & kRIB_OPTION_EXPAND_INLINE && inlined )
   {
      if ( HandleExpandInlineOption(rib,a) )
	 goto Error;
   }
   return a; 

Error:
   if (inlined)
      _RibFree(inlined);
   return NULL;
}


PRIB_HASHATOM RibHandleInLineDeclaration( RIB_HANDLE hrib, char **pname )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   RIB_HASHATOM        *p;
   auto char           *name = *pname;


   /* Find if the in-line parameter declaration option is allowed. */
   if ( rib->options & kRIB_OPTION_NO_INLINE )
      return NULL;

   /* Find if there is an in-line parameter declaration. */
   p = RibDeclare( (RIB_HANDLE)rib, name, NULL );

   if (p && pname)
   {
      *pname = p->data;
   }

   return p;
}


int RibUndoInLineList( RIB_HANDLE hrib )
{
   auto PRIB_INSTANCE  rib = (PRIB_INSTANCE)hrib;
   RIB_INLINEDECL      *p,*pp;


   p = rib->inlinelist;
   while (p)
   {
      pp = p->next;

      if ( p->code & kRIB_INLINE_DECLARED )
      {
	 RibRemoveItem( RibGetHashHandle(rib), p->atom );
      }
      else
      {
	 auto PRIB_HASHATOM  a = p->atom;

	 /* Restore original data. */
	 a->code = p->code;
	 a->with.n = p->n;

	 if (rib->options & kRIB_OPTION_EXPAND_INLINE)
	 {
	    /* If error, keep going so that atleast the inline structures 
	     *    in the inlined list can be freed. 
	     * Inshort ignore the return code.
	     */
	    (void)HandleExpandInlineOption(rib,a);
	 }
      }
      _RibFree( p );

      p = pp;
   }
   rib->inlinelist = NULL;
   
   return kRIB_OK;
}


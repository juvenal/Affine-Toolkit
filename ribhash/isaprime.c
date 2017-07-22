/* $RCSfile: isaprime.c,v $  $Revision: 1.3 $ $Date: 1999/10/13 04:20:29 $
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
 * FILE:  isaprime.c
 *
 * DESCRIPTION:  Finds out if a number is prime or not.  For use in 
 *               writing hashing functions.
 *
 *               This is basically a simple little tool for those who
 *               like to play with hash functions.  
 *
 * 
 *    References:
 *
 *    History:
 *       03-17-98  Moved code into toolkit.
 *       10-12-99  Added code for when TABLE is defined and argc not equal
 *                 to 2 or 3.
 *
 *
 *         The RenderMan (R) Interface Procedures and Protocol are:
 *                    Copyright 1988, 1989, Pixar
 *                        All Rights Reserved
 *
 *             RenderMan (R) is a registered trademark of Pixar
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Compile with the macro TABLE defined and the following table is printed
 *    when using isaprime and max value as a parameter:
 *
 *  % isaprime 3000
 *  1 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97 
 *  101 103 107 109 113 127 131 137 139 149 151 157 163 167 173 179 181 191 
 *  193 197 199 211 223 227 229 233 239 241 251 257 263 269 271 277 281 283 
 *  293 307 311 313 317 331 337 347 349 353 359 367 373 379 383 389 397 401 
 *  409 419 421 431 433 439 443 449 457 461 463 467 479 487 491 499 503 509 
 *  521 523 541 547 557 563 569 571 577 587 593 599 601 607 613 617 619 631 
 *  641 643 647 653 659 661 673 677 683 691 701 709 719 727 733 739 743 751 
 *  757 761 769 773 787 797 809 811 821 823 827 829 839 853 857 859 863 877 
 *  881 883 887 907 911 919 929 937 941 947 953 967 971 977 983 991 997 1009 
 *  1013 1019 1021 1031 1033 1039 1049 1051 1061 1063 1069 1087 1091 1093 
 *  1097 1103 1109 1117 1123 1129 1151 1153 1163 1171 1181 1187 1193 1201 
 *  1213 1217 1223 1229 1231 1237 1249 1259 1277 1279 1283 1289 1291 1297 
 *  1301 1303 1307 1319 1321 1327 1361 1367 1373 1381 1399 1409 1423 1427 
 *  1429 1433 1439 1447 1451 1453 1459 1471 1481 1483 1487 1489 1493 1499 
 *  1511 1523 1531 1543 1549 1553 1559 1567 1571 1579 1583 1597 1601 1607 
 *  1609 1613 1619 1621 1627 1637 1657 1663 1667 1669 1693 1697 1699 1709 
 *  1721 1723 1733 1741 1747 1753 1759 1777 1783 1787 1789 1801 1811 1823 
 *  1831 1847 1861 1867 1871 1873 1877 1879 1889 1901 1907 1913 1931 1933 
 *  1949 1951 1973 1979 1987 1993 1997 1999 2003 2011 2017 2027 2029 2039 
 *  2053 2063 2069 2081 2083 2087 2089 2099 2111 2113 2129 2131 2137 2141 
 *  2143 2153 2161 2179 2203 2207 2213 2221 2237 2239 2243 2251 2267 2269 
 *  2273 2281 2287 2293 2297 2309 2311 2333 2339 2341 2347 2351 2357 2371 
 *  2377 2381 2383 2389 2393 2399 2411 2417 2423 2437 2441 2447 2459 2467 
 *  2473 2477 2503 2521 2531 2539 2543 2549 2551 2557 2579 2591 2593 2609 
 *  2617 2621 2633 2647 2657 2659 2663 2671 2677 2683 2687 2689 2693 2699 
 *  2707 2711 2713 2719 2729 2731 2741 2749 2753 2767 2777 2789 2791 2797 
 *  2801 2803 2819 2833 2837 2843 2851 2857 2861 2879 2887 2897 2903 2909 
 *  2917 2927 2939 2953 2957 2963 2969 2971 2999 
 *
 */
/* See above comment for including "#define TABLE" into the code. */
#if 1
#define TABLE
#endif

/* To check if a number p is prime, check that p is not evenly
 * divisible by any integer n such that 2 <= n < p.  This range would 
 * be the simplest range to use, but the upper part of the range can 
 * be lowered to inclusively use sqrt(p) so that any integer n 
 * where 2 <= n <= sqrt(p) does not evenly divide p.  For the upper 
 * range to be sqrt(p), note that the numbers represented by n are 
 * being tested to find atleast one prime factor.  
 * 
 * When checking if p is prime, divide p by the integers upto and 
 * including the square root of p.  If any of the divisions result
 * in an integer value, a prime factor has been found.  If no prime 
 * factors have been found, you know the number is prime because 
 * any integer above the square root of p can not be a prime factor
 * without there being a prime factor less than the square root of p.  
 * 
 * This fact can be shown by assuming that there is a value m such 
 * that m > sqrt(p) and m is a prime factor of p.  Note that m*m > p 
 * because m > sqrt(p).  So for m to be a factor some integer value n 
 * made up of one or more prime factors multiplied together would have 
 * to exist such that m*n = p.  The value n can not equal sqrt(p) 
 * because then m = sqrt(p) and that would contradict m > sqrt(p).  
 * The value n would have to be less than sqrt(p) because m > sqrt(p)  
 * and there is no way to have n*m = p if both n and m are greater 
 * than the square root of p.  
 * 
 * So if there is a prime factor m greater than sqrt(p) and less 
 * than p then there also has to be a value n such that 
 *             n < sqrt(p) < m < p and 
 *             n * m = p.
 * Since there has to be a value n, checking from 2 upto and including
 * the sqrt(p) is sufficient enough to show that p is prime.
 * 
 */
int main(int argc, char *argv[]) {
   int     i,j,s,prime;
   float   f,v;
#ifdef TABLE
   int     min;
   int     max;
#endif

   i = 1;

#ifndef TABLE
   while (i < argc) {
#else
   if (argc == 3) {
      min = atoi(argv[1]);
      max = atoi(argv[2]);
   }
   else if (argc == 2) {
      min = 2;
      max = atoi(argv[1]);
   }
   else {
      printf("isaprime [min] max\n");
      return 1;
   }
   i = min;
   while (i <= max) {
#endif
      prime = 1;
#ifndef TABLE
# ifdef atof
      /* Just using atof() as a better command-line parser for numbers. */
      f = atof(argv[i]);
# else
      f = (float) atoi(argv[i]);
# endif
#else
      f = i;
#endif
      v = floor(f);
      if (v != f) {
         printf("Value %g is not an integer.\n", f);
      }
      else if (v <= 0) {
         printf("Value %g must be greater than zero.\n", v);
      }
      else {
         s = (int) sqrt(f);
         for (j = 2; j <= s; j++) {
            f = v / (float) j;
            if (floor(f) == f) {
#ifndef TABLE
               printf("%g not prime because %d divides it.\n",
                      v, j);
#endif
               prime = 0;
               break;
            }
         }
         if (prime) {
#ifndef TABLE
            printf("%g is prime.\n", v);
#else
            printf("%g ", v);
#endif
         }
      }
      i++;
   }
#ifdef TABLE
   printf("\n");
#endif
   return 0;
}

/*
** Copyright 2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/
#ifndef CC_COMMON_TOOLS_IDIOMS_HH
#define   CC_COMMON_TOOLS_IDIOMS_HH


/**
 * @function is in
 *
 *  @brief   Saves you writing chain equals-OR-equals statements
 *  @param1  val : RValue of interest
 *  @param2  arr : an a array of values to be checked
 *  @return  Returns whether the <val> is in <arr>
 **/

/*
   example :
   string result("Yeh");

   string went_well[]={ "Yeh", "cool", "sure", right on"};

   if ( is_in( result, went_well ) ){
     //....
   }
         // -bunch of literals with no meaning attached
         // -operand result repeated over and over
   if (  result == "Yeh ||
         result =="cool" ||
         result =="sure"  ||
         result =="right on")
     ...
*/
template < typename T1, typename T2,  int N >
int is_in( const T1& val, T2(& arr)[N] ){
  return   std::find( arr, arr + N, val) !=  arr + N;
}

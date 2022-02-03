//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


#pragma once


/*!
 Automatic initializer macro: put this macro in your header file
 if you want to ensure the given function will be called when starting the execution.
 Remember to declare your function before using this macro, otherwise
 you will get an "identifier not found" error.
 Please note that your function will be executed jut one time.

 E.g., in the header file: @code
 void myInitFunc();
 GPV_AUTOINITFUNC(myInitFunc)
 @endcode
 ... and in the source file
 @code
void myInitFunc() {
  // initializations...
} @endcode
will call myInitFunc() in any source
 that includes your header file (only one time).
*/
#define GPVULC_AUTOINITFUNC(f) \
static struct _##f##_Initializer {\
  _##f##_Initializer() {\
    static bool guard = false;\
    if ( guard ) return;\
    guard = true;\
    f();\
  }\
} __##f##_Initializer;


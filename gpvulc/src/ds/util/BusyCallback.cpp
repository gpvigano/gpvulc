//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


#include <gpvulc/ds/util/BusyCallback.h>


namespace gpvulc
{
	
gvBusyCallBack* gvBusyCB = nullptr;
void* gvBusyCB_data = nullptr;


void SetBusyCallBack( gvBusyCallBack* cb, void* data ) {
  gvBusyCB = cb;
  gvBusyCB_data = data;
}


gvBusyCallBack* GetBusyCallBack(void** data) {
  if (data) *data=gvBusyCB_data;
  return gvBusyCB;
}


void gvCallBusyCallBack( const char* msg ) {
  if( gvBusyCB ) gvBusyCB( msg, gvBusyCB_data );
}
}


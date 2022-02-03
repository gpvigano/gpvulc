//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Console Messaging functions: errors, warnings, info...
/// @file ConsoleMessage.h
/// @author Giovanni Paolo Vigano'

#ifndef MSG_H_
#define MSG_H_

#include <string>

namespace gpvulc
{

/// @defgroup ConsoleMessages ConsoleMessages
/// @brief Multi level console messages logging support.
/// @author Giovanni Paolo Vigano'
  
/// @addtogroup DS
/// @{

/// @addtogroup Util
/// @{

/// @addtogroup ConsoleMessages
/// @{

//! Logging level of severity.
enum NotifyLevelEnum { LOG_FATAL=1, LOG_WARN, LOG_NOTICE, LOG_INFO, LOG_DEBUG, LOG_VERBOSE };




//**************************************************************************
//                          NOTIFY
//**************************************************************************

/*!
 Notify an event writing a message to the console.
 The first parameter is a NotifyLevelEnum.
 The following parameters are directly passed to printf or sprintf.
*/
void NotifyMsg(int lev, const char *fmt, ...);

/*!
 Log an event writing a message to a file.
 The first parameter is a NotifyLevelEnum.
 The following parameters are directly passed to vfprintf.
*/
void LogMsg(int lev, const char *file_name, const char *fmt, ...);

/*!
 Macro that tests if the current notification level set is compatible with the given one
 before calling NotifyMsg().
*/
#define GPVULC_NOTIFY(lev, fmt, ...) if ( !CheckNotifyLevel(lev) ) {} else NotifyMsg(lev,fmt, ##__VA_ARGS__)

/*!
 Macro that tests if the current notification level set is compatible with the given one
 before calling LogMsg().
*/
#define GPVULC_LOG(lev, file_name, fmt, ...) if ( !CheckNotifyLevel(lev) ) {} else LogMsg(lev,file_name,fmt, ##__VA_ARGS__)

/*!
 Macro that tests if the current notification level set is compatible with the given one
 before calling NotifyMsg() and LogMsg().
*/
#define GPVULC_LOGNOTIFY(lev, file_name, fmt, ...) if ( !CheckNotifyLevel(lev) ) {} else {\
  NotifyMsg(lev,fmt, ##__VA_ARGS__);\
  LogMsg(lev,file_name,fmt, ##__VA_ARGS__);\
}

///@name Set/get/test the currently used notification level
//@{

//! Set the current notification level
void SetNotifyLevel(int lev);

//! Get the current notification level
int GetNotifyLevel();

//! Check if the current notification level set is compatible with the given one
bool CheckNotifyLevel(int lev);

//@}

/// @}
/// @}
/// @}

}

#endif//MSG_H_

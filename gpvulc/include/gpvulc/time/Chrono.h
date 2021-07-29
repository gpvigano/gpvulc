//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Chrono class
/// @file Chrono.h
/// @author Giovanni Paolo Vigano'

#pragma once

namespace gpvulc
{

	/// @addtogroup Time
	/// @{

	//! Chrono simulation (time expressed in seconds)
	class Chrono
	{

	public:

		Chrono();

		//! Reset the chrono and the saved interval
		void Reset();

		//! Start the chrono
		void Start();

		//! Return true if the chrono has started
		bool IsStarted() { return Started; }

		//! Return true if the chrono has paused
		bool IsPaused() { return Paused; }

		//! Pause the chrono
		double Pause();

		//! Resume the chrono
		void Resume();

		//! Get the elapsed time while the chrono is running
		double GetElapsedTime();

		//! Return the last saved interval (when Stop() was called)
		double GetLastInterval() { return RecTime; }

		//! Stop the chrono (elapsed time is saved and returned, see GetLastInterval())
		double Stop();

		//! Stop and start again the chrono (elapsed time is saved and returned, see GetLastInterval())
		double Restart();

	protected:

		double StartTime;

		double PauseTime;

		double ElapsedTime;

		double RecTime;

		double IdleTime;

		bool Started;

		bool Paused;
	};

	///@}

}//namespace gpvulc



//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//

// Chrono class implementation


#include <gpvulc/time/Chrono.h>
#include <gpvulc/time/TimeUtil.h>


namespace gpvulc
{

	//---------------------------------------------------------------------
	// Chrono class implementation

	Chrono::Chrono()
	{
		Reset();
		RecTime = 0;
	}


	void Chrono::Reset()
	{
		StartTime = ElapsedTime = 0;
		Started = false;
		Paused = false;
		IdleTime = 0;
	}


	void Chrono::Start()
	{

		if (Paused) { Resume(); return; }
		if (Started) return;
		StartTime = GetRunTimeSeconds();
		Started = true;
		Paused = false;
		IdleTime = 0;
	}


	double Chrono::Pause()
	{
		if (!Started) return -1.0;
		if (!Paused)
		{
			PauseTime = GetRunTimeSeconds();
			Paused = true;
		}
		return PauseTime - StartTime - IdleTime;
	}


	void Chrono::Resume()
	{
		if (!Started) return;
		if (!Paused) return;
		IdleTime += GetRunTimeSeconds() - PauseTime;
		//StartTime = GetRunTimeSeconds()-(PauseTime-StartTime);
		Paused = false;
	}


	double Chrono::GetElapsedTime()
	{
		if (!Started) return 0.0;
		if (Paused) return PauseTime - StartTime - IdleTime;
		return ElapsedTime = GetRunTimeSeconds() - StartTime - IdleTime;
	}


	double Chrono::Stop()
	{
		RecTime = GetElapsedTime();
		Started = false;
		IdleTime = 0;
		return RecTime;
	}


	double Chrono::Restart()
	{
		Stop();
		Start();
		return RecTime;
	}

} // namespace gpvulc

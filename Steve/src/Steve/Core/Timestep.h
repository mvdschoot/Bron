#ifndef __TIMESTEP_H__
#define __TIMESTEP_H__

namespace Steve
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: _time(time)
		{
		}

		operator float() const { return _time; }

		float getSeconds() const { return _time; }
		float getMilliseconds() const { return _time * 1000.0f; }
	private:
		float _time;
	};
}

#endif // __TIMESTEP_H__

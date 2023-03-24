#ifndef __APP_HEADER__
#define __APP_HEADER__

#include "Steve/Core/EntryPoint.h"
#include "Core.h"

#include "AppLayer.h"

namespace Steve
{
	class App : public Application
	{
		public:
			App();
			~App();

		private:
			Overlay m_overlay;
			AppLayer m_applayer;
	};
}

#endif

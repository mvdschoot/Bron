#ifndef __APP_HEADER__
#define __APP_HEADER__

#include "Steve/src/Steve.h"

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

// #ifndef _REGISTRY_HEADR__
// #define _REGISTRY_HEADR__
//
// #include "Singleton.h"
// #include "Steve/ECS/Registry.h"
//
// namespace Steve
// {
// 	// Global storage, for everyone
// 	class Storage : public Singleton<Storage>, public Registry
// 	{
// 	public:
// 		Storage() : Root(CreateEntity()) {}
// 		~Storage() = default;
//
//
// 		void Reallocate()
//
// 		// For all rest storage
// 		Entity& Root;
// 	};
// }
//
//
//
// #endif // _REGISTRY_HEADR__
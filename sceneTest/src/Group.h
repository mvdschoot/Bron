#ifndef GROUP_HEADER_
#define GROUP_HEADER_

#include "Core.h"
#include "UUID.h"
#include "Entity.h"
#include "ComponentHandle.h"

#include <tuple>
#include <typeindex>
#include <functional>
#include <vector>

namespace Cheets
{
	class IGroup {
    public:
        std::type_index p_Type;
    protected:
        IGroup() : p_Type(typeid(IGroup)) {}

        virtual void Remove(Entity &entity) = 0;
        virtual void Insert(Entity &ent) = 0;
        virtual void Update(UUID& entity) = 0;
        [[nodiscard]] virtual char *GetRaw() const = 0;
        virtual std::vector<Ref<Entity>> GetHandles() = 0;
    };

	template<class ...T>
	class Group : public IGroup
	{
		using TupleType = std::tuple<T...>;

		public:
			Group() : m_StorageSize(1024)
			{
				p_Type = std::type_index(typeid(Group<T...>));
				m_Storage = static_cast<char*>(malloc(m_StorageSize));
				m_StorageFreePointer = m_Storage;

				([&, this]() {this->m_BlockSize += sizeof(T); }(), ...);
			}
			
			Group(const Group& gr)
			{
				*this = gr;
				m_Storage = static_cast<char*>(malloc(m_StorageSize));
				memcpy(m_Storage, gr.m_Storage, m_StorageSize);
			}
			
			~Group() 
			{
				free(m_Storage);
			}

			void Insert(Entity& ent) override
			{

				if ((m_StorageFreePointer + m_BlockSize) >= (m_Storage + m_StorageSize))
				{
					IncreaseStorage();
				}

				m_Handles.push_back(Ref<Entity>(&ent));
				std::tuple<ComponentHandle<T>*...> comps = ent.GetComponents<T...>();

				auto f = [this](auto* val) {
					val->Move(this->m_StorageFreePointer);
					this->m_StorageFreePointer += val->Size();
				};


				std::apply([&](ComponentHandle<T>*... in) {
					(f(in), ...);
				}, comps);
			}

			void Remove(Entity& entity) override
			{
				std::tuple<ComponentHandle<T>*...> comps = entity.GetComponents<T...>();
				std::tuple<ComponentHandle<T>*...> to_move = m_Handles.back()->GetComponents<T...>();

				auto* t = std::get<0>(comps);
				char* loc = (char*)(&(**t));
				memset(loc, 0, m_BlockSize);
				const int offset = ((loc - m_Storage) / m_BlockSize);

				auto f = [&](auto* val) {
					val->Move(loc);
					loc += val->Size();
				};

				std::apply([&](ComponentHandle<T>*... in) {
					(f(in), ...);
				}, to_move);


				m_Handles[offset] = *(m_Handles.end() - 1);
				m_Handles.pop_back();
				m_StorageFreePointer -= m_BlockSize;
			};

            void Update(UUID& entity) override
            {
                Ref<Entity> ent;
                for (const Ref<Entity> e : m_Handles)
                {
                    if (e->p_Id == entity) {
                        ent = e;
                        break;
                    }
                }

                std::tuple<ComponentHandle<T>*...> comps = ent->GetComponents<T...>();

                auto* t = std::get<0>(comps);
                char* loc = (char*)(&(**t));

                auto f = [&, this](auto* val) {
                    val->Move(loc);
                    loc += val->Size();
                };

                std::apply([&](ComponentHandle<T>*... in) {
                    (f(in), ...);
                }, comps);
            }

			[[nodiscard]] char* GetRaw() const override
			{
				return m_Storage;
			}

			std::vector<Ref<Entity>> GetHandles() override
			{
				return m_Handles;
			}

		private:
			void IncreaseStorage()
			{
				m_StorageSize *= 2;
				char* tmp = static_cast<char*>(realloc(m_Storage, m_StorageSize));
				m_StorageFreePointer = m_StorageFreePointer - m_Storage + tmp;
				m_Storage = tmp;
			}

		public:
			UUID p_Id;
		private:

			// Component Storage
			char* m_Storage;
			size_t m_StorageSize;
			char* m_StorageFreePointer;
			size_t m_BlockSize = 0;

			std::vector<Ref<Entity>> m_Handles;
	};
}

#endif

#include "IconManagement.h"
#include <iostream>
#include <filesystem>


namespace Steve
{
    std::map<std::string, Ref<Texture>> Icons::icons;

	namespace fs = std::filesystem;

	void Icons::Load()
	{
        try {
            for (const auto& entry : fs::directory_iterator("../Assets/Icons")) {
                if (entry.is_regular_file() && entry.path().extension() == ".png") {
                    icons.emplace(entry.path().stem().string(), Texture2D::Create(entry.path().string().c_str()));
                }
            }
        }
        catch (const std::exception& e) {
            CORE_ERROR("Error with loading icons: {}", e.what());
        }
	}

	uint32_t Icons::GetIconId(const char* name)
	{
        return icons[name]->getID();
	}
}

#include "achievements.h"

#include "crc.h"
#include "extensions.h"
#include "map.h"
#include "network.h"
#include "preferences.h"

#ifdef HAVE_STEAM
#include "steamshim_child.h"
#endif

Achievements* Achievements::instance() {
	static Achievements* instance_ = nullptr;
	if (!instance_)
	{
		instance_ = new Achievements();
	}

	return instance_;
}

std::string Achievements::get_lua()
{
	std::string lua;

	uint32_t map_checksum = 0;
	if (game_is_networked)
	{
		// is this set up at the time we're called?
		map_checksum = static_cast<game_info*>(NetGetGameData())->parent_checksum;
	}
	else
	{
		map_checksum = get_current_map_checksum();
	}

	auto physics_checksum = get_physics_file_checksum();

	static constexpr uint32_t m1_map_checksum = 0x03C9;
	static constexpr uint32_t m1_physics_checksum = 0x5BC77666;

	if (map_checksum == m1_map_checksum &&
		physics_checksum == m1_physics_checksum)
	{
		lua = 
		#include "m1_achievements.lua"
		;
	}

	return lua;
}

void Achievements::set(const std::string& key)
{
#ifdef HAVE_STEAM
	STEAMSHIM_setAchievement(key.c_str(), 1);
	STEAMSHIM_storeStats();
#endif
}

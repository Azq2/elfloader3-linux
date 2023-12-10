#pragma once

#include "swi.h"
#include "gui/Font.h"
#include "BinaryStream.h"

#include <cstdint>
#include <string>
#include <map>

class Resources {
	protected:
		static constexpr uint32_t MAX_EXT_PALETTE_COLORS = 60;
		static constexpr uint32_t MAX_INT_PALETTE_COLORS = 24;
		char m_int_pallete[MAX_INT_PALETTE_COLORS][4] = {
			{ 0xff, 0xff, 0xff, 0x64 },
			{ 0x00, 0x00, 0x00, 0x64 },
			{ 0xff, 0x00, 0x00, 0x64 },
			{ 0x00, 0x00, 0xff, 0x64 },
			{ 0x00, 0xff, 0x00, 0x64 },
			{ 0xff, 0x00, 0xff, 0x64 },
			{ 0x00, 0xff, 0xff, 0x64 },
			{ 0xff, 0xff, 0x00, 0x64 },
			{ 0xff, 0x80, 0x80, 0x64 },
			{ 0x80, 0x80, 0xff, 0x64 },
			{ 0x80, 0xff, 0x80, 0x64 },
			{ 0xff, 0x80, 0xff, 0x64 },
			{ 0x80, 0xff, 0xff, 0x64 },
			{ 0xff, 0xff, 0x80, 0x64 },
			{ 0x80, 0x00, 0x00, 0x64 },
			{ 0x00, 0x00, 0x80, 0x64 },
			{ 0x00, 0x80, 0x00, 0x64 },
			{ 0x80, 0x00, 0x80, 0x64 },
			{ 0x00, 0x80, 0x80, 0x64 },
			{ 0x80, 0x80, 0x00, 0x64 },
			{ 0x4a, 0x4a, 0x4a, 0x64 },
			{ 0x80, 0x80, 0x8c, 0x64 },
			{ 0xc0, 0xc0, 0xc0, 0x64 },
			{ 0x00, 0x00, 0x00, 0x00 },
		};
		char m_ext_pallete[MAX_EXT_PALETTE_COLORS][4] = {};
		
		std::map<int, IMGHDR *> m_pit_table;
		std::map<int, Font *> m_font_table;
		
		std::string m_root;
		
		Resources(const std::string &root);
		
		void load();
		void loadColorTheme();
		void loadPictures();
		void loadFonts();
		bool loadFont(Font *font, const std::string &path);
	
	public:
		IMGHDR *getPicture(int num);
		int getPicIdByUnicode(uint32_t num);
		IMGHDR *getPicByUnicode(uint32_t num);
		Font *getFont(int id);
		
		static void init(const std::string &root);
		static Resources *instance();
		char *getColorPtr(int index);
		
		static inline std::string getPlatformName() {
			#if defined(ELKA)
				return "ELKA";
			#elif defined(NEWSGOLD)
				return "NSG";
			#else
				return "SG";
			#endif
		}
};

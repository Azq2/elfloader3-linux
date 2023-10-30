#include "charset.h"

#include <cassert>

static constexpr uint16_t cp1251_to_utf16_table[] = {
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 
	0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 
	0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F, 
	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 
	0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 
	0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, 
	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 
	0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 
	0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 
	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 
	0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F, 
	0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021, 
	0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F, 
	0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 
	0x0020, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F, 
	0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7, 
	0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407, 
	0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7, 
	0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457, 
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 
	0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 
	0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F, 
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 
	0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F, 
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 
	0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
};

static size_t _codepoint_len(uint32_t codepoint) {
	assert(codepoint <= 0x1FFFFF);
	
	if (codepoint <= 0x7F) {
		return 1;
	} else if (codepoint <= 0x7FF) {
		return 2;
	} else if (codepoint <= 0xFFFF) {
		return 3;
	} else if (codepoint <= 0x1FFFFF) {
		return 4;
	}
	return 0;
}

static size_t _encode_codepoint(char *tmp, uint32_t codepoint) {
	assert(codepoint <= 0x1FFFFF);
	
	if (codepoint <= 0x7F) {
		// 0xxxxxxx
		tmp[0] = codepoint;
		return 1;
	} else if (codepoint <= 0x7FF) {
		// 110xxxxx 10xxxxxx
		tmp[0] = ((codepoint >> 6) & 0x1F) | 0xC0;
		tmp[1] = (codepoint & 0x3F) | 0x80;
		return 2;
	} else if (codepoint <= 0xFFFF) {
		// 1110xxxx 10xxxxxx 10xxxxxx
		tmp[0] = ((codepoint >> 12) & 0xF) | 0xE0;
		tmp[1] = ((codepoint >> 6) & 0x3F) | 0x80;
		tmp[2] = (codepoint & 0x3F) | 0x80;
		return 3;
	} else if (codepoint <= 0x1FFFFF) {
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		tmp[0] = ((codepoint >> 18) & 0x7) | 0xF0;
		tmp[1] = ((codepoint >> 12) & 0x3F) | 0x80;
		tmp[2] = ((codepoint >> 6) & 0x3F) | 0x80;
		tmp[3] = (codepoint & 0x3F) | 0x80;
		return 4;
	}
	return 0;
}

static bool _check_utf8_seq(const char *buffer, size_t len) {
	for (size_t i = 1; i < len; i++) {
		if ((buffer[i] & 0xC0) != 0x80)
			return false;
	}
	return true;
}

static size_t _decode_utf8(const char *buffer, size_t max_len, uint32_t *codepoint) {
	if ((buffer[0] & 0xF8) == 0xF0 && max_len >= 4 && _check_utf8_seq(buffer, 4)) {
		// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		*codepoint = (buffer[0] & 0x7) << 18;
		*codepoint |= (buffer[1] & 0x3F) << 12;
		*codepoint |= (buffer[2] & 0x3F) << 6;
		*codepoint |= (buffer[3] & 0x3F);
		return 4;
	} else if ((buffer[0] & 0xF0) == 0xE0 && max_len >= 3 && _check_utf8_seq(buffer, 3)) {
		// 1110xxxx 10xxxxxx 10xxxxxx
		*codepoint = (buffer[0] & 0xF) << 12;
		*codepoint |= (buffer[1] & 0x3F) << 6;
		*codepoint |= (buffer[2] & 0x3F);
		return 3;
	} else if ((buffer[0] & 0xE0) == 0xC0 && max_len >= 2 && _check_utf8_seq(buffer, 2)) {
		// 110xxxxx 10xxxxxx
		*codepoint = (buffer[0] & 0x1F) << 6;
		*codepoint |= (buffer[1] & 0x3F);
		return 2;
	} else if ((buffer[0] & 0x80) == 0) {
		// 0xxxxxxx
		*codepoint = buffer[0];
		return 1;
	}
	*codepoint = 0xFFFD;
	return 1;
}

static size_t _decode_utf16(const uint16_t *buffer, size_t max_len, uint32_t *codepoint) {
	if ((buffer[0] & 0xFC00) == 0xD800 && max_len >= 2 && (buffer[1] & 0xFC00) == 0xDC00) {
		*codepoint = (buffer[0] & 0x3FF) << 10;
		*codepoint |= (buffer[1] & 0x3FF);
		*codepoint += 0x10000;
		return 2;
	} else if ((buffer[0] & 0xFC00) == 0xD800 || (buffer[0] & 0xFC00) == 0xDC00) {
		*codepoint = 0xFFFD;
	} else {
		*codepoint = buffer[0];
	}
	return 1;
}

size_t utf8_to_utf16(const std::string &input, uint16_t *buffer, size_t buffer_size) {
	assert(buffer_size > 0);
	size_t new_len = 0;
	for (size_t i = 0; i < input.size(); ) {
		uint32_t codepoint = 0;
		i += _decode_utf8(&input[i], input.size() - i, &codepoint);
		if ((codepoint >= 0x0000 && codepoint <= 0xD7FF) || (codepoint >= 0xE000 && codepoint <= 0xFFFF)) {
			if ((new_len + 1) > (buffer_size - 1))
				break;
			buffer[new_len++] = codepoint;
		} else {
			if ((new_len + 2) > (buffer_size - 1))
				break;
			uint32_t double_codepoint = codepoint - 0x10000;
			buffer[new_len++] = ((double_codepoint >> 10) & 0x3FF) | 0xD800;
			buffer[new_len++] = (double_codepoint & 0x3FF) | 0xDC00;
		}
	}
	buffer[new_len] = 0;
	return new_len;
}

size_t utf16_to_utf8(const std::vector<uint16_t> &input, char *buffer, size_t buffer_size) {
	assert(buffer_size > 0);
	size_t new_len = 0;
	for (size_t i = 0; i < input.size(); ) {
		uint32_t codepoint = 0;
		i += _decode_utf16(&input[i], input.size() - i, &codepoint);
		size_t char_len = _codepoint_len(codepoint);
		if (new_len + char_len > (buffer_size - 1))
			break;
		new_len += _encode_codepoint(&buffer[new_len], codepoint);
	}
	buffer[new_len] = 0;
	return new_len;
}

std::string cp1251_to_utf8(const std::string &input) {
	char tmp[4];
	std::string out;
	for (auto &c: input)
		out.append(tmp, _encode_codepoint(tmp, cp1251_to_utf16_table[c]));
	return out;
}

size_t cp1251_to_utf8(const std::string &input, char *buffer, size_t buffer_size) {
	assert(buffer_size > 0);
	size_t new_len = 0;
	for (size_t i = 0; i < input.size(); i++) {
		uint16_t codepoint = cp1251_to_utf16_table[input[i]];
		size_t char_len = _codepoint_len(codepoint);
		if (new_len + char_len > (buffer_size - 1))
			break;
		new_len += _encode_codepoint(&buffer[new_len], codepoint);
	}
	buffer[new_len] = 0;
	return new_len;
}

std::vector<uint16_t> cp1251_to_utf16(const std::string &input) {
	std::vector<uint16_t> out;
	out.resize(input.size());
	for (auto &c: input)
		out.push_back(cp1251_to_utf16_table[c]);
	return out;
}

size_t cp1251_to_utf16(const std::string &input, uint16_t *buffer, size_t buffer_size) {
	assert(buffer_size > 0);
	size_t new_len = std::min(buffer_size - 1, input.size());
	for (size_t i = 0; i < new_len; i++)
		buffer[i] = cp1251_to_utf16_table[input[i]];
	buffer[new_len] = 0;
	return new_len;
}
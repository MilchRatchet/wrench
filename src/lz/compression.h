/*
	wrench - A set of modding tools for the Ratchet & Clank PS2 games.
	Copyright (C) 2019-2021 chaoticgd

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef LZ_COMPRESSION_H
#define LZ_COMPRESSION_H

#include "../stream.h"

#include <map>
#include <cstring>
#include <utility>

// Decompress and recompress WAD segments used by the games to store various
// assets. Not to be confused with WAD archives.

packed_struct(WadHeader,
	char magic[3]; // "WAD"
	int32_t total_size; // Including header.
	uint8_t pad[9];
)

// Check the magic bytes.
bool validate_wad(char* magic);

struct WadBuffer {
	WadBuffer(std::vector<uint8_t>& vec) : ptr(&(*vec.begin())), end(&(*vec.end())) {}
	
	const uint8_t* ptr;
	const uint8_t* end;
};

bool decompress_wad(std::vector<uint8_t>& dest, WadBuffer src);

void compress_wad(array_stream& dest, array_stream& src, int thread_count);
void compress_wad(std::vector<uint8_t>& dest, const std::vector<uint8_t>& src, int thread_count);

#endif

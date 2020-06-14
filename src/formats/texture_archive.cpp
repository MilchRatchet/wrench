/*
	wrench - A set of modding tools for the Ratchet & Clank PS2 games.
	Copyright (C) 2019-2020 chaoticgd

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

#include "texture_archive.h"

#include "fip.h"

packed_struct(texture_table_entry,
	sector32 offset;
	uint32_t unknown_4;
)

std::vector<texture> enumerate_fip_textures(iso_stream& iso, toc_table table) {
	std::vector<texture> textures;
	
	std::size_t bad_textures = 0;
	
	for(std::size_t off = 0; off < table.data.size(); off += sizeof(texture_table_entry)) {
		auto entry = table.data.read<texture_table_entry>(off);
		std::size_t abs_offset = table.header.base_offset.bytes() + entry.offset.bytes();
		
		if(abs_offset > iso.size()) {
			return {};
		}
		
		if(entry.offset.bytes() == 0) {
			continue;
		}
		
		stream* file;
		std::size_t inner_offset;
		
		char wad_magic[3];
		iso.seek(abs_offset);
		iso.read_n(wad_magic, 3);
		if(std::memcmp(wad_magic, "WAD", 3) == 0) {
			file = iso.get_decompressed(abs_offset);
			inner_offset = 0;
		} else {
			file = &iso;
			inner_offset = abs_offset;
		}
		
		if(file == nullptr || file->size() < 0x14) {
			bad_textures++;
			continue;
		}
		
		char magic[0x14];
		file->seek(inner_offset);
		file->read_n(magic, 0x14);
		
		std::optional<std::size_t> texture_offset;
		if(validate_fip(magic)) {
			texture_offset = 0;
		}
		if(validate_fip(magic + 0x10)) {
			texture_offset = 0x10;
		}
		
		if(texture_offset) {
			std::optional<texture> tex = create_fip_texture(file, inner_offset + *texture_offset);
			if(tex) {
				textures.emplace_back(*tex);
			} else {
				bad_textures++;
				std::cerr << "Error: Failed to load 2FIP texture at "
				          << file->resource_path() << "\n";
			}
		}
	}
	
	if(bad_textures > 10) {
		return {};
	}
	
	return textures;
}

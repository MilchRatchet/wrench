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

#ifndef FORMATS_ARMOR_ARCHIVE_H
#define FORMATS_ARMOR_ARCHIVE_H

#include <memory>
#include <string>
#include <vector>

#include "../stream.h"
#include "toc.h"
#include "texture.h"
#include "game_model.h"

# /*
# 	Read ARMOR.WAD.
# */

packed_struct(armor_table_entry,
	sector32 model;
	sector32 model_size;
	sector32 texture;
	sector32 texture_size;
)

packed_struct(armor_model_header,
	uint8_t submodel_count_1;       // 0x0
	uint8_t submodel_count_2;       // 0x1
	uint8_t submodel_count_3;       // 0x2
	uint8_t unknown_3;              // 0x3
	uint32_t submodel_table_offset; // 0x4
	uint32_t unknown_8;             // 0x8
	uint32_t unknown_c;             // 0xc
)

class armor_archive {
public:
	armor_archive();
	
	bool read(stream& iso, const toc_table& table);
	
	std::vector<moby_model> models;
	std::vector<texture> textures;
};

#endif

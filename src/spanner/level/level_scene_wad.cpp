/*
	wrench - A set of modding tools for the Ratchet & Clank PS2 games.
	Copyright (C) 2019-2022 chaoticgd

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

#include "level_scene_wad.h"

#include <set>
#include <spanner/asset_packer.h>

static void pack_level_scene_wad(OutputStream& dest, std::vector<u8>* header_dest, LevelSceneWadAsset& src, Game game);

on_load([]() {
	LevelSceneWadAsset::pack_func = wrap_wad_packer_func<LevelSceneWadAsset>(pack_level_scene_wad);
})

packed_struct(SceneHeaderDL,
	/* 0x00 */ Sector32 speech_english_left;
	/* 0x04 */ Sector32 speech_english_right;
	/* 0x08 */ SectorRange subtitles;
	/* 0x10 */ Sector32 speech_french_left;
	/* 0x14 */ Sector32 speech_french_right;
	/* 0x18 */ Sector32 speech_german_left;
	/* 0x1c */ Sector32 speech_german_right;
	/* 0x20 */ Sector32 speech_spanish_left;
	/* 0x24 */ Sector32 speech_spanish_right;
	/* 0x28 */ Sector32 speech_italian_left;
	/* 0x2c */ Sector32 speech_italian_right;
	/* 0x30 */ SectorRange moby_load;
	/* 0x38 */ Sector32 chunks[69];
)

packed_struct(LevelSceneWadHeaderDL,
	/* 0x0 */ s32 header_size;
	/* 0x4 */ Sector32 sector;
	/* 0x8 */ SceneHeaderDL scenes[30];
)

static void pack_level_scene_wad(OutputStream& dest, std::vector<u8>* header_dest, LevelSceneWadAsset& src, Game game) {
	s64 base = dest.tell();
	
	LevelSceneWadHeaderDL header = {0};
	header.header_size = sizeof(LevelSceneWadHeaderDL);
	dest.write(header);
	dest.pad(SECTOR_SIZE, 0);
	
	// ...
	
	dest.write(base, header);
	if(header_dest) {
		OutBuffer(*header_dest).write(0, header);
	}
}

static SectorRange range(Sector32 offset, const std::set<s64>& end_sectors);

void unpack_level_scene_wad(LevelSceneWadAsset& dest, BinaryAsset& src) {
	auto [file, header] = open_wad_file<LevelSceneWadHeaderDL>(src);
	
	std::set<s64> end_sectors;
	for(const SceneHeaderDL& scene : header.scenes) {
		end_sectors.insert(scene.speech_english_left.sectors);
		end_sectors.insert(scene.speech_english_right.sectors);
		end_sectors.insert(scene.subtitles.offset.sectors);
		end_sectors.insert(scene.speech_french_left.sectors);
		end_sectors.insert(scene.speech_french_right.sectors);
		end_sectors.insert(scene.speech_german_left.sectors);
		end_sectors.insert(scene.speech_german_right.sectors);
		end_sectors.insert(scene.speech_spanish_left.sectors);
		end_sectors.insert(scene.speech_spanish_right.sectors);
		end_sectors.insert(scene.speech_italian_left.sectors);
		end_sectors.insert(scene.speech_italian_right.sectors);
		end_sectors.insert(scene.moby_load.offset.sectors);
		for(Sector32 chunk : scene.chunks) {
			end_sectors.insert(chunk.sectors);
		}
	}
	end_sectors.insert(Sector32::size_from_bytes(file->size()).sectors);
	
	CollectionAsset& scenes = dest.scenes();
	for(s32 i = 0; i < ARRAY_SIZE(header.scenes); i++) {
		SceneAsset& scene = scenes.child<SceneAsset>(i).switch_files();
		const SceneHeaderDL& scene_header = header.scenes[i];
		unpack_binary(scene.speech_english_left(), *file, range(scene_header.speech_english_left, end_sectors), "speech_english_left.vag");
		unpack_binary(scene.speech_english_right(), *file, range(scene_header.speech_english_right, end_sectors), "speech_english_right.vag");
		unpack_binary(scene.subtitles(), *file, scene_header.subtitles, "subtitles.vag");
		unpack_binary(scene.speech_french_left(), *file, range(scene_header.speech_french_left, end_sectors), "speech_french_left.vag");
		unpack_binary(scene.speech_french_right(), *file, range(scene_header.speech_french_right, end_sectors), "speech_french_right.vag");
		unpack_binary(scene.speech_german_left(), *file, range(scene_header.speech_german_left, end_sectors), "speech_german_left.vag");
		unpack_binary(scene.speech_german_right(), *file, range(scene_header.speech_german_right, end_sectors), "speech_german_right.vag");
		unpack_binary(scene.speech_spanish_left(), *file, range(scene_header.speech_spanish_left, end_sectors), "speech_spanish_left.vag");
		unpack_binary(scene.speech_spanish_right(), *file, range(scene_header.speech_spanish_right, end_sectors), "speech_spanish_right.vag");
		unpack_binary(scene.speech_italian_left(), *file, range(scene_header.speech_italian_left, end_sectors), "speech_italian_left.vag");
		unpack_binary(scene.speech_italian_right(), *file, range(scene_header.speech_italian_right, end_sectors), "speech_italian_right.vag");
		unpack_binary(scene.moby_load(), *file, scene_header.moby_load, "moby_load.bin");
	}
}

static SectorRange range(Sector32 offset, const std::set<s64>& end_sectors) {
	auto end_sector = end_sectors.upper_bound(offset.sectors);
	verify(end_sector != end_sectors.end(), "Header references audio beyond end of file. The WAD file may be truncated.");
	return {offset, Sector32(*end_sector - offset.sectors)};
}

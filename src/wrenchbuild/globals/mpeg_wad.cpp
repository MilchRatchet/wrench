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

#include <wrenchbuild/asset_unpacker.h>
#include <wrenchbuild/asset_packer.h>

packed_struct(MpegHeader,
	SectorByteRange subtitles;
	SectorByteRange video;
)

packed_struct(RacMpegWadHeader,
	/* 0x0 */ s32 header_size;
	/* 0x4 */ Sector32 sector;
	/* 0x8 */ SectorByteRange mpegs[88];
)

packed_struct(GcMpegWadHeader,
	/* 0x0 */ s32 header_size;
	/* 0x4 */ Sector32 sector;
	/* 0x8 */ MpegHeader mpegs[50];
)

packed_struct(UyaDlMpegWadHeader,
	/* 0x0 */ s32 header_size;
	/* 0x4 */ Sector32 sector;
	/* 0x8 */ MpegHeader mpegs[100];
)

static void unpack_rac_mpeg_wad(MpegWadAsset& dest, const RacMpegWadHeader& header, InputStream& src, Game game);
static void pack_rac_mpeg_wad(OutputStream& dest, RacMpegWadHeader& header, const MpegWadAsset& src, Game game, const char* hint);
template <typename Header>
static void unpack_gc_uya_dl_mpeg_wad(MpegWadAsset& dest, const Header& header, InputStream& src, Game game);
template <typename Header>
static void pack_gc_uya_dl_mpeg_wad(OutputStream& dest, Header& header, const MpegWadAsset& src, Game game, const char* hint);

on_load(Mpeg, []() {
	MpegWadAsset::funcs.unpack_rac1 = wrap_wad_unpacker_func<MpegWadAsset, RacMpegWadHeader>(unpack_rac_mpeg_wad);
	MpegWadAsset::funcs.unpack_rac2 = wrap_wad_unpacker_func<MpegWadAsset, GcMpegWadHeader>(unpack_gc_uya_dl_mpeg_wad<GcMpegWadHeader>);
	MpegWadAsset::funcs.unpack_rac3 = wrap_wad_unpacker_func<MpegWadAsset, UyaDlMpegWadHeader>(unpack_gc_uya_dl_mpeg_wad<UyaDlMpegWadHeader>);
	MpegWadAsset::funcs.unpack_dl = wrap_wad_unpacker_func<MpegWadAsset, UyaDlMpegWadHeader>(unpack_gc_uya_dl_mpeg_wad<UyaDlMpegWadHeader>);
	
	MpegWadAsset::funcs.pack_rac1 = wrap_wad_hint_packer_func<MpegWadAsset, RacMpegWadHeader>(pack_rac_mpeg_wad);
	MpegWadAsset::funcs.pack_rac2 = wrap_wad_hint_packer_func<MpegWadAsset, GcMpegWadHeader>(pack_gc_uya_dl_mpeg_wad<GcMpegWadHeader>);
	MpegWadAsset::funcs.pack_rac3 = wrap_wad_hint_packer_func<MpegWadAsset, UyaDlMpegWadHeader>(pack_gc_uya_dl_mpeg_wad<UyaDlMpegWadHeader>);
	MpegWadAsset::funcs.pack_dl = wrap_wad_hint_packer_func<MpegWadAsset, UyaDlMpegWadHeader>(pack_gc_uya_dl_mpeg_wad<UyaDlMpegWadHeader>);
})

static void unpack_rac_mpeg_wad(MpegWadAsset& dest, const RacMpegWadHeader& header, InputStream& src, Game game) {
	for(s32 i = 0; i < ARRAY_SIZE(header.mpegs); i++) {
		if(!header.mpegs[i].empty()) {
			BinaryAsset& video = dest.mpegs().child<BinaryAsset>(i);
			unpack_asset(video, src, header.mpegs[i], game, FMT_BINARY_PSS);
		}
	}
}

static void pack_rac_mpeg_wad(OutputStream& dest, RacMpegWadHeader& header, const MpegWadAsset& src, Game game, const char* hint) {
	if(strcmp(next_hint(&hint), "nompegs") == 0) {
		return;
	}
	
	const CollectionAsset& mpegs = src.get_mpegs();
	for(s32 i = 0; i < ARRAY_SIZE(header.mpegs); i++) {
		if(mpegs.has_child(i)) {
			const Asset& child = mpegs.get_child(i);
			if(child.type() == MpegAsset::ASSET_TYPE) {
				const MpegAsset& mpeg = child.as<MpegAsset>();
				header.mpegs[i] = pack_asset_sa<SectorByteRange>(dest, mpeg.get_video(), game);
			} else {
				header.mpegs[i] = pack_asset_sa<SectorByteRange>(dest, child, game);
			}
		}
	}
}

template <typename Header>
static void unpack_gc_uya_dl_mpeg_wad(MpegWadAsset& dest, const Header& header, InputStream& src, Game game) {
	for(s32 i = 0; i < ARRAY_SIZE(header.mpegs); i++) {
		if(!header.mpegs[i].subtitles.empty() || !header.mpegs[i].video.empty()) {
			MpegAsset& mpeg = dest.mpegs().foreign_child<MpegAsset>(i);
			BinaryAsset& video = mpeg.child<BinaryAsset>("video");
			unpack_asset(video, src, header.mpegs[i].video, game, FMT_BINARY_PSS);
			CollectionAsset& subtitles = mpeg.child<CollectionAsset>("subtitles");
			unpack_asset(subtitles, src, header.mpegs[i].subtitles, game, FMT_COLLECTION_SUBTITLES);
		}
	}
}

template <typename Header>
static void pack_gc_uya_dl_mpeg_wad(OutputStream& dest, Header& header, const MpegWadAsset& src, Game game, const char* hint) {
	if(strcmp(next_hint(&hint), "nompegs") == 0) {
		return;
	}
	
	const CollectionAsset& mpegs = src.get_mpegs();
	for(s32 i = 0; i < ARRAY_SIZE(header.mpegs); i++) {
		if(mpegs.has_child(i)) {
			const Asset& child = mpegs.get_child(i);
			if(child.type() == MpegAsset::ASSET_TYPE) {
				const MpegAsset& mpeg = child.as<MpegAsset>();
				if(mpeg.has_subtitles()) {
					header.mpegs[i].subtitles = pack_asset_sa<SectorByteRange>(dest, mpeg.get_subtitles(), game, FMT_COLLECTION_SUBTITLES);
				}
				header.mpegs[i].video = pack_asset_sa<SectorByteRange>(dest, mpeg.get_video(), game);
			} else {
				header.mpegs[i].video = pack_asset_sa<SectorByteRange>(dest, child, game);
			}
		}
	}
}

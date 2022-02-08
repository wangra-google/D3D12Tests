#include "tests.h"
#include <time.h> 
#include <cstdlib>

// GetCopyableFootprints Implementation
namespace
{
	static uint32_t RoundUp(uint32_t value, uint32_t multiple)
	{
		assert(multiple && ((multiple & (multiple - 1)) == 0));
		return (value + multiple - 1) & ~(multiple - 1);
	}

	static bool IsBCFormat(DXGI_FORMAT format)
	{
		bool ret = false;

		switch (format)
		{
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			ret = true;
			break;
		default:
			break;
		}

		return ret;
	}

	static bool IsPairedFormat(DXGI_FORMAT format)
	{
		bool ret = false;

		switch (format)
		{
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
			ret = true;
			break;
		default:
			break;
		}

		return ret;
	}

	static bool IsDepthFormat(DXGI_FORMAT format)
	{
		bool ret = false;

		switch (format)
		{
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			ret = true;
			break;
		default:
			break;
		}

		return ret;
	}

	static bool IsUnSupportedFormat(DXGI_FORMAT format)
	{
		bool ret = false;

		switch (format)
		{
		case DXGI_FORMAT_UNKNOWN:
		case DXGI_FORMAT_R1_UNORM:
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		case DXGI_FORMAT_P8:
		case DXGI_FORMAT_A8P8:
		case DXGI_FORMAT_AYUV:
		case DXGI_FORMAT_Y410:
		case DXGI_FORMAT_Y416:
		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
		case DXGI_FORMAT_420_OPAQUE:
		case DXGI_FORMAT_YUY2:
		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
		case DXGI_FORMAT_NV11:
		case DXGI_FORMAT_AI44:
		case DXGI_FORMAT_IA44:
		case DXGI_FORMAT_P208:
		case DXGI_FORMAT_V208:
		case DXGI_FORMAT_V408:
		case DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE:
		case DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE:
		case DXGI_FORMAT_FORCE_UINT:
			ret = true;
			break;
		default:
			break;
		}

		return ret;
	}

	static DXGI_FORMAT GetAliasedFormat(DXGI_FORMAT format)
	{
		DXGI_FORMAT ret = format;

		switch (format)
		{
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			ret = DXGI_FORMAT_R32_TYPELESS;
			break;

		default:
			break;
		}

		return ret;
	}

	static void GetRowPitchAndBlockHeightInByte(DXGI_FORMAT format, uint32_t width, uint32_t height, uint32_t& row_pitch, uint32_t& block_height)
	{
		row_pitch = width;
		block_height = height;
		const uint32_t block_width = RoundUp(width, 4) / 4;
		switch (format)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			row_pitch *= 16;
			break;
		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			row_pitch *= 12;
			break;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
			row_pitch *= 8;
			break;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			row_pitch *= 4;
			break;
		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
		case DXGI_FORMAT_B4G4R4A4_UNORM:
			row_pitch *= 2;
			break;
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
			row_pitch *= 2;
			row_pitch = RoundUp(row_pitch, 4);
			break;
		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
			break;
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			row_pitch = block_width * 8;
			block_height = RoundUp(height, 4) / 4;
			break;
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			row_pitch = block_width * 16;
			block_height = RoundUp(height, 4) / 4;
			break;
		default:
			assert(false && "Unsupported format!");
			break;
		}
	}

	void STDMETHODCALLTYPE GetCopyableFootprints(
		const D3D12_RESOURCE_DESC* pResourceDesc,
		UINT                                FirstSubresource,
		UINT                                NumSubresources,
		UINT64                              BaseOffset,
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
		UINT* pNumRows,
		UINT64* pRowSizeInBytes,
		UINT64* pTotalBytes)
	{
		if (pResourceDesc->MipLevels == 0)
		{
			return;
		}
		const DXGI_FORMAT format = GetAliasedFormat(pResourceDesc->Format);
		const bool is_aliased = (format != pResourceDesc->Format);
		const bool is_bc_format = IsBCFormat(format);
		const bool is_depth_format = IsDepthFormat(format);
		const bool is_1d_texture = (pResourceDesc->Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D);
		const bool is_2d_texture = (pResourceDesc->Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D);
		const bool is_3d_texture = (pResourceDesc->Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D);
		const bool is_paired_format = IsPairedFormat(format);
		uint32_t w = static_cast<uint32_t>(pResourceDesc->Width);
		uint32_t h = static_cast<uint32_t>(pResourceDesc->Height);
		uint32_t d = is_3d_texture ? pResourceDesc->DepthOrArraySize : 1;
		constexpr uint32_t limit_2d_texture = 16384;
		constexpr uint32_t limit_3d_texture = 2048;
		const bool over_limit_2d_texture = ((w > limit_2d_texture) || (h > limit_2d_texture) || (pResourceDesc->DepthOrArraySize > limit_3d_texture));
		const bool over_limit_3d_texture = ((w > limit_3d_texture) || (h > limit_3d_texture) || (d > limit_3d_texture));
		if ((NumSubresources > pResourceDesc->MipLevels)
			|| (pResourceDesc->Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
			|| (is_1d_texture)
			|| (is_2d_texture && over_limit_2d_texture)
			|| (is_3d_texture && (over_limit_3d_texture || is_aliased || is_depth_format))
			|| (pResourceDesc->SampleDesc.Count != 1)
			|| (pResourceDesc->Alignment != 0)
			|| (pResourceDesc->Layout != D3D12_TEXTURE_LAYOUT_UNKNOWN)
			|| (pResourceDesc->Flags != D3D12_RESOURCE_FLAG_NONE)
			|| (is_bc_format && (((w & 3) != 0) || ((h & 3) != 0)))
			|| (is_paired_format && ((w & 1) != 0))
			|| (IsUnSupportedFormat(format))
			)
		{
			if (pLayouts != nullptr)
			{
				memset(pLayouts, -1, sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) * pResourceDesc->MipLevels);
			}
			if (pNumRows != nullptr)
			{
				memset(pNumRows, -1, sizeof(UINT) * pResourceDesc->MipLevels);
			}
			if (pRowSizeInBytes != nullptr)
			{
				memset(pRowSizeInBytes, -1, sizeof(UINT64) * pResourceDesc->MipLevels);
			}
			if (pTotalBytes != nullptr)
			{
				memset(pTotalBytes, -1, sizeof(UINT64));
			}
			return;
		}

		/*********************************************************************/
		// TODO(wangra): there are some weird behaviors in D3D12 when width or 
		// height is not dividable 16, sometimes the value is round up,  
		// sometimes it is round down, e.g: 420
		// Do not support those values for now
		if (is_bc_format)
		{
			assert((w & 15) == 0);
			assert((h & 15) == 0);
		}

		if (is_paired_format)
		{
			assert((w & 31) == 0);
		}
		/*********************************************************************/

		uint32_t          offset = 0;
		for (uint32_t i = 0; i < NumSubresources; ++i)
		{
			uint32_t row_pitch = 0;
			uint32_t block_height = 0;
			GetRowPitchAndBlockHeightInByte(format, w, h, row_pitch, block_height);
			const uint32_t padded_row_pitch = RoundUp(row_pitch, is_aliased ? 512 : 256);
			w = is_bc_format ? RoundUp(w, 4) : (is_paired_format ? RoundUp(w, 2) : w);
			h = is_bc_format ? RoundUp(h, 4) : h;
			if (pLayouts != nullptr)
			{
				pLayouts[i].Offset = offset;
				pLayouts[i].Footprint.Format = format;
				pLayouts[i].Footprint.Width = w;
				pLayouts[i].Footprint.Height = h;
				pLayouts[i].Footprint.Depth = d;
				pLayouts[i].Footprint.RowPitch = padded_row_pitch;
			}

			if (pNumRows != nullptr)
			{
				pNumRows[i] = block_height;
			}

			if (pRowSizeInBytes != nullptr)
			{
				pRowSizeInBytes[i] = row_pitch;
			}

			const uint32_t min_w = is_bc_format ? 4 : (is_paired_format ? 2 : 1);
			const uint32_t min_h = is_bc_format ? 4 : 1;

			const bool is_last_sub_resource = (i == (NumSubresources - 1));

			if (is_last_sub_resource)
			{
				uint32_t size = padded_row_pitch * block_height * (d - 1)
					+ padded_row_pitch * (block_height - 1) + row_pitch;
				offset += size;
			}
			else
			{
				offset += padded_row_pitch * block_height * d;
				offset = RoundUp(offset, 512);
			}

			if (w > min_w) w = w >> 1;
			if (h > min_h) h = h >> 1;
			if (d > 1) d = d >> 1;
		}

		if (pTotalBytes != nullptr)
		{
			*pTotalBytes = offset;
		}
	}
}

void D3D12UnitTests::TestGetCopyableFootprints()
{
	constexpr uint32_t format_count = uint32_t(DXGI_FORMAT_B4G4R4A4_UNORM) + 1;
	constexpr uint32_t random_size_count = 5000;
	srand(uint32_t(time(nullptr)));

	for (uint32_t f = 0; f < format_count; ++f)
	{
		const DXGI_FORMAT format = static_cast<DXGI_FORMAT>(f);
		for (int r = 0; r < random_size_count; ++r)
		{
			uint32_t width = rand() % 16384;
			uint32_t height = rand() % 16384;
			uint32_t depth = rand() % 16384;
			D3D12_RESOURCE_DIMENSION dims[2] = { D3D12_RESOURCE_DIMENSION_TEXTURE2D , D3D12_RESOURCE_DIMENSION_TEXTURE3D };
			uint32_t dim_idx = rand() % 2;
			D3D12_RESOURCE_DIMENSION dim = dims[dim_idx];
			const bool is_3d_texture = (dim == D3D12_RESOURCE_DIMENSION_TEXTURE3D);
			
			if (is_3d_texture)
			{
				width = rand() % 2048;
				height = rand() % 2048;
				depth = rand() % 2048;
			}

			if ((IsBCFormat(format) || IsPairedFormat(format)))
			{
				width = RoundUp(width, 16);
			}

			if (IsBCFormat(format) )
			{
				height = RoundUp(height, 16);
			}


			// desc
			constexpr uint32_t sub_resource_count = 10;
			D3D12_RESOURCE_DESC desc;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Alignment = 0;
			desc.Width = width;
			desc.Height = height;
			desc.DepthOrArraySize = depth;
			desc.MipLevels = sub_resource_count;
			desc.Format = format;
			desc.SampleDesc = { 1, 0 };
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts[sub_resource_count];
			uint32_t num_rows[sub_resource_count];
			uint64_t row_sizes_in_bytes[sub_resource_count];
			uint64_t total_bytes;
			m_device->GetCopyableFootprints(&desc, 0, sub_resource_count, 0, layouts,
				num_rows, row_sizes_in_bytes, &total_bytes);

			D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts_test[sub_resource_count];
			uint32_t num_rows_test[sub_resource_count];
			uint64_t row_sizes_in_bytes_test[sub_resource_count];
			uint64_t total_bytes_test;
			GetCopyableFootprints(&desc, 0, sub_resource_count, 0, layouts_test,
				num_rows_test, row_sizes_in_bytes_test, &total_bytes_test);

			// Validation
			for (uint32_t i = 0; i < sub_resource_count; ++i)
			{
				if (layouts[i].Offset != layouts_test[i].Offset) assert(false);
				if (layouts[i].Footprint.Format != layouts_test[i].Footprint.Format) assert(false);
				if (layouts[i].Footprint.Width != layouts_test[i].Footprint.Width) assert(false);
				if (layouts[i].Footprint.Height != layouts_test[i].Footprint.Height) assert(false);
				if (layouts[i].Footprint.Depth != layouts_test[i].Footprint.Depth) assert(false);
				if (layouts[i].Footprint.RowPitch != layouts_test[i].Footprint.RowPitch) assert(false);

				if (num_rows[i] != num_rows_test[i]) assert(false);
				if (row_sizes_in_bytes[i] != row_sizes_in_bytes_test[i]) assert(false);
			}
			if (total_bytes != total_bytes_test) assert(false);
		}
	}
}
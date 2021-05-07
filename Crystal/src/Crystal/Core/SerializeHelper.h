#pragma once
#include <d3d12.h>

#define SERIALIZE_PROPERTIES friend class boost::serialization::access;\
template <class Archive>\
void serialize(Archive& ar, const int version)

namespace boost {
	namespace serialization {
		template <class Archive>
		void serialize(Archive& ar, DirectX::XMFLOAT4X4& g, const unsigned int version)
		{
			ar & g.m;
		}

		template <class Archive>
		void serialize(Archive& ar, DirectX::XMFLOAT2& g, const unsigned int version)
		{
			ar & g.x;
			ar & g.y;
		}

		template <class Archive>
		void serialize(Archive& ar, DirectX::XMFLOAT3& g, const unsigned int version)
		{
			ar & g.x;
			ar & g.y;
			ar & g.z;
		}

		template <class Archive>
		void serialize(Archive& ar, DirectX::XMFLOAT4& g, const unsigned int version)
		{
			ar & g.x;
			ar & g.y;
			ar & g.z;
			ar & g.w;
		}

		template <class Archive>
		void serialize(Archive& ar, tagPOINT& g, const unsigned int version)
		{
			ar& g.x;
			ar& g.y;
			
		}

		


		template <class Archive>
		void serialize(Archive& ar, D3D12_VIEWPORT& g, const unsigned int version)
		{
			ar & g.TopLeftX;
			ar & g.TopLeftY;
			ar & g.Width;
			ar & g.Height;
			ar & g.MinDepth;
			ar & g.MaxDepth;
		}


		template <class Archive>
		void serialize(Archive& ar, D3D12_RECT& g, const unsigned int version)
		{
			ar & g.left;
			ar & g.top;
			ar & g.right;
			ar & g.bottom;
		}


	}
}

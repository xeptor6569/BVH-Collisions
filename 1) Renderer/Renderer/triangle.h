#pragma once
#include "math_types.h"
#include <vector>

namespace end
{
	struct aabb_t { float3 center; float3 extents; }; //Alternative: aabb_t { float3 min; float3 max; };

	struct triangle
	{
		std::vector<float3> indices;
		aabb_t aabb;
		float3 centroid;
		float3 min;
		float3 max;
		float4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
	};
}

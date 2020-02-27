#pragma once
#include "math_types.h"
#include "renderer.h"
#include <array>
//#include <DirectXMath.h>
//#include <dxgi1_2.h>
//#include <d3d11_2.h>
//
//#pragma comment(lib, "d2d1.lib")
//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "DXGI.lib")


// Note: You are free to make adjustments/additions to the declarations provided here.

namespace end
{
	//using namespace DirectX;

	struct sphere_t { float3 center; float radius; }; //Alterative: using sphere_t = float4;

	struct aabb_t { float3 min; float3 max; float3 center; float3 extents; float4 color = { 0.0f, 1.0f, 1.0f, 1.0f }; }; //Alternative: aabb_t { float3 min; float3 max; };

	struct aabb_box { float3 min; float3 max;};

	struct plane_t { float3 normal; float offset; };  //Alterative: using plane_t = float4;

	using frustum_t = std::array<plane_t, 6>;

	// Calculates the plane of a triangle from three points.
	plane_t calculate_plane(float3 a, float3 b, float3 c)
	{
		plane_t result;
		//result.normal = normalize(cross(c - b, b - a));
		result.normal = normalize(cross(b - a, c - b));
		result.offset = dot(result.normal, a);
		return result;
	};

	// Calculates a frustum (6 planes) from the input view parameter.
	//
	// Calculate the eight corner points of the frustum. 
	// Use your debug renderer to draw the edges.
	// 
	// Calculate the frustum planes.
	// Use your debug renderer to draw the plane normals as line segments.
	void calculate_frustum(frustum_t& frustum, float fov, float viewRatio, float nearDist, float farDist, float4x4 camXform)
	{
		// Center Point Calculation
		float3 nD = { nearDist, nearDist, nearDist };
		float3 fD = { farDist, farDist, farDist };
		float3 near_center = camXform[3].xyz + camXform[2].xyz * nD;
		float3 far_center = camXform[3].xyz + camXform[2].xyz * fD;

		// Height & Width of Far Plane
		float height_near = 2 * tan(fov / 2) * nearDist;
		float height_far = 2 * tan(fov / 2) * farDist;
		// Height & Width of Near Plane
		float width_near = height_near * viewRatio;
		float width_far = height_far * viewRatio;

		// Make float3's with scalar multiply
		float3 Hfar = { height_far * 0.5f, height_far * 0.5f, height_far * 0.5f };
		float3 Wfar = { width_far * 0.5f ,width_far * 0.5f ,width_far * 0.5f };
		float3 HNear = { height_near * 0.5f, height_near * 0.5f, height_near * 0.5f };
		float3 WNear = { width_near * 0.5f ,width_near * 0.5f ,width_near * 0.5f };

		// Calculate 8 points of Near/Far Plane
		float3 far_top_left = far_center + camXform[1].xyz * Hfar - camXform[0].xyz * Wfar;
		float3 far_top_right = far_center + camXform[1].xyz * Hfar + camXform[0].xyz * Wfar;
		float3 far_bottom_left = far_center - camXform[1].xyz * Hfar - camXform[0].xyz * Wfar;
		float3 far_bottom_right = far_center - camXform[1].xyz * Hfar + camXform[0].xyz * Wfar;
		float3 near_top_left = near_center + camXform[1].xyz * HNear - camXform[0].xyz * WNear;
		float3 near_top_right = near_center + camXform[1].xyz * HNear + camXform[0].xyz * WNear;
		float3 near_bottom_left = near_center - camXform[1].xyz * HNear - camXform[0].xyz * WNear;
		float3 near_bottom_right = near_center - camXform[1].xyz * HNear + camXform[0].xyz * WNear;

		// Construct frustum
		frustum[0] = calculate_plane(near_bottom_left, far_top_left, far_bottom_left); // Left
		frustum[1] = calculate_plane(near_bottom_right, far_bottom_right, far_top_right); // Right
		frustum[2] = calculate_plane(near_bottom_left, near_bottom_right, near_top_right); // Near
		frustum[3] = calculate_plane(far_bottom_right, far_bottom_left, far_top_right); // Far
		frustum[4] = calculate_plane(near_top_left, far_top_right, far_top_left); // Top
		frustum[5] = calculate_plane(near_bottom_left, far_bottom_left, far_bottom_right); // Bottom
		
		// Calculte Center points of the sides
		float3 four = { 4, 4, 4 };
		float3 dir_scalar = { 0.5 ,0.5 ,0.5 };
		float3 left_center = (near_bottom_left + far_bottom_left + far_top_left + near_top_left) / four;
		float3 right_center = (near_bottom_right + far_bottom_right + far_top_right + near_top_right) / four;
		float3 top_center = (near_top_left + near_top_right + far_top_left + far_top_right) / four;
		float3 bottom_center = (near_bottom_left + near_bottom_right + far_bottom_left + far_bottom_right) / four;

		// Draw Far
		end::debug_renderer::add_line(far_top_left,    far_top_right,    { 1.0f, 0.0f, 0.0f, 1.0f });
		end::debug_renderer::add_line(far_top_left,    far_bottom_left,  { 1.0f, 0.0f, 0.0f, 1.0f });
		end::debug_renderer::add_line(far_top_right,   far_bottom_right, { 1.0f, 0.0f, 0.0f, 1.0f });
		end::debug_renderer::add_line(far_bottom_left, far_bottom_right, { 1.0f, 0.0f, 0.0f, 1.0f });

		// Draw Near
		end::debug_renderer::add_line(near_top_left,    near_top_right,    { 1.0f, 0.0f, 0.0f, 1.0f });
		end::debug_renderer::add_line(near_top_left,    near_bottom_left,  { 1.0f, 0.0f, 0.0f, 1.0f });
		end::debug_renderer::add_line(near_top_right,   near_bottom_right, { 1.0f, 0.0f, 0.0f, 1.0f });
		end::debug_renderer::add_line(near_bottom_left, near_bottom_right, { 1.0f, 0.0f, 0.0f, 1.0f });

		// Draw Connection
		end::debug_renderer::add_line(near_top_left,     far_top_left,     { 1.0f, 0.0f, 0.0f, 1.0f });
		end::debug_renderer::add_line(near_top_right,    far_top_right,    { 1.0f, 0.0f, 0.0f, 1.0f });
		end::debug_renderer::add_line(near_bottom_left,  far_bottom_left,  { 1.0f, 0.0f, 0.0f, 1.0f });
		end::debug_renderer::add_line(near_bottom_right, far_bottom_right, { 1.0f, 0.0f, 0.0f, 1.0f });

		// Draw Normal pointing in (Blue to white lerp)
		end::debug_renderer::add_line(left_center, (left_center + (frustum[0].normal * dir_scalar)),     { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
		end::debug_renderer::add_line(right_center, (right_center + (frustum[1].normal * dir_scalar)),   { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
		end::debug_renderer::add_line(near_center, (near_center + (frustum[2].normal * dir_scalar)),     { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
		end::debug_renderer::add_line(far_center, (far_center + (frustum[3].normal * dir_scalar)),       { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
		end::debug_renderer::add_line(top_center, (top_center + (frustum[4].normal * dir_scalar)),       { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
		end::debug_renderer::add_line(bottom_center, (bottom_center + (frustum[5].normal * dir_scalar)), { 0.0f,0.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f,1.0f });

	}

	// Calculates which side of a plane the sphere is on.
	//
	// Returns -1 if the sphere is completely behind the plane.
	// Returns 1 if the sphere is completely in front of the plane.
	// Otherwise returns 0 (Sphere overlaps the plane)
	int classify_sphere_to_plane(const sphere_t& sphere, const plane_t& plane);     // NOT USED

	// Calculates which side of a plane the aabb is on.
	//
	// Returns -1 if the aabb is completely behind the plane.
	// Returns 1 if the aabb is completely in front of the plane.
	// Otherwise returns 0 (aabb overlaps the plane)
	// MUST BE IMPLEMENTED UsING THE PROJECTED RADIUS TEST
	int classify_aabb_to_plane(const aabb_t& aabb, const plane_t& plane);          // NOT USED

	// Determines if the aabb is inside the frustum.
	//
	// Returns false if the aabb is completely behind any plane.
	// Otherwise returns true. 
	bool aabb_to_frustum(const aabb_t& aabb, const frustum_t& frustum)            // USED
	{
		for (int i = 0; i < 6; i++)
		{
			float projected_radius = aabb.extents.x * abs(frustum[i].normal.x)
				+ aabb.extents.y * abs(frustum[i].normal.y)
				+ aabb.extents.z * abs(frustum[i].normal.z);

			float center_distance = aabb.center.x * frustum[i].normal.x
				+ aabb.center.y * frustum[i].normal.y
				+ aabb.center.z * frustum[i].normal.z;

			float result = center_distance - frustum[i].offset;


			if (result < -projected_radius)
			{
				return false;
			}

		}

		return true;
	};
}


// dot the extent with the absulte value of the planes normal
//dot center of cube with plnaes normal
// subtract that number from the offset of the plane, then compare the first number with that. whether its larger or not is whether its in the frustum
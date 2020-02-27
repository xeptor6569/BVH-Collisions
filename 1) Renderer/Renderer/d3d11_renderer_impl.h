#pragma once

#define AABB_MIN 0.5
#define AABB_MAX 4
#define POS_MIN -5
#define POS_MAX 5

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DXGI.lib")

#include "renderer.h"
#include "view.h"
#include "blob.h"
#include "../Renderer/shaders/mvp.hlsli"
#include "debug_renderer.h"
#include <ctime>
#include "pools.h"
#include "XTime.h"
#include "frustum_culling.h"

#include "Camera.h"
//#include "bvh.h"

//#include "Mouse.h"
// NOTE: This header file must *ONLY* be included by renderer.cpp

namespace
{
	template<typename T>
	void safe_release(T* t)
	{
		if (t)
			t->Release();
	}
}


namespace end
{
	using namespace DirectX;

	// WVP Matrices
	//float4x4 targets[0].position = (float4x4&)XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(30.0f, 15.f, 0.0f)); // 0,0,0 //z -4.f
	float4x4 w_matrixTurn = (float4x4&)XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(5.0f,5.f,35.0f)); // 5,5,35 //z 5.f
	float4x4 w_matrixLook = (float4x4&)XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(-4.0f, 5.f, 25.0f)); //-4,5,25 // z-4.f
	XMMATRIX w_matrixLookPos = (XMMATRIX&)w_matrixLook;

	XMMATRIX matRot;
	XMMATRIX matTrans;

	frustum_t frustum;
	float viewRatio;

	int rasterFillType = 3;


	UINT size;

	struct SimpleVertex
	{
		float3 pos;
		float3 Normal;
		float2 Tex;
	};
	// Binary File
	uint32_t vert_count;         //Number of vertices
	std::vector<float3> pos;      // Array of vert_count vertex positions
	std::vector<float3> norms;   // Array of vert_count normals
	std::vector<float2> uvs;     // Array of vert_count texture coordinates

	std::vector<SimpleVertex> surface;

	struct Terrain
	{
		float3 centroid;
		aabb_t aabb;
	};
	std::vector<Terrain> terrain;


	struct Target
	{
		float4x4 position = (float4x4&)XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, 0.0f, -30.0f));//(float4x4&)XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(45.0f, 15.8f, 25.0f));//(float4x4&)XMMatrixIdentity();////    // Defualt Values
		aabb_t aabb;
		float3 centroid;
		float width = 1.f;     // Defualt Values
		float height = 2.0f;	// Defualt Values
		float depth = 1.f;		// Defualt Values
		float4 color = { 0.0f, 1.0f, 1.0f, 1.0f };
		//triangle tri;
	}target;


	

	struct Node
	{
		Terrain element;
		Node* left;
		Node* right;

		Node(Terrain _element)
		{
			left = right = nullptr;
			element = _element;
		}
	};

	Node* root = nullptr;

	//std::vector<triangle> triangles;
	//std::vector<float3> indices;
	std::vector<uint32_t> indices;


	struct renderer_t::impl_t
	{
		// platform/api specific members, functions, etc.
		// Device, swapchain, resource views, states, etc. can be members here
		HWND hwnd;

		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		IDXGISwapChain* swapchain = nullptr;

		ID3D11RenderTargetView* render_target[VIEW_RENDER_TARGET::COUNT]{};

		ID3D11DepthStencilView* depthStencilView[VIEW_DEPTH_STENCIL::COUNT]{};

		ID3D11DepthStencilState* depthStencilState[STATE_DEPTH_STENCIL::COUNT]{};

		ID3D11RasterizerState* rasterState[STATE_RASTERIZER::COUNT]{};

		ID3D11Buffer* vertex_buffer[VERTEX_BUFFER::COUNT]{};

		ID3D11Buffer* index_buffer[INDEX_BUFFER::COUNT]{};

		ID3D11InputLayout* input_layout[INPUT_LAYOUT::COUNT]{};

		ID3D11VertexShader* vertex_shader[VERTEX_SHADER::COUNT]{};

		ID3D11PixelShader* pixel_shader[PIXEL_SHADER::COUNT]{};

		ID3D11Buffer* constant_buffer[CONSTANT_BUFFER::COUNT]{};

		D3D11_VIEWPORT				view_port[VIEWPORT::COUNT]{};

		ID3D11InputLayout* input_layout_wire;


		/* Add more as needed...
		ID3D11SamplerState*			sampler_state[STATE_SAMPLER::COUNT]{};

		ID3D11BlendState*			blend_state[STATE_BLEND::COUNT]{};
		*/
		// Timer init
		XTime xtimer;
		std::clock_t start;
		float duration;
		double delta = 0.0f;
		double delta2 = 0.0f;
		double delta_time = 0.0f;

		// Red
		bool cieling = false;
		float color_dec = 0.5f;
		float jump = 0.03f;

		Camera cam;

		// Constructor for renderer implementation
		// 
		impl_t(native_handle_type window_handle, view_t& default_view)
		{
			srand(time(0));

			hwnd = (HWND)window_handle;

			create_device_and_swapchain();

			create_main_render_target();

			setup_depth_stencil();

			setup_rasterizer();

			create_shaders();

			create_constant_buffers();

			// BVH Stuff
			load_terrain();

			std::random_shuffle(terrain.begin(), terrain.end());

			update_target();

			construct_tree();

			// Camera
			cam.SetEyePosVector(XMVectorSet(0.0f, 30.0f, -50.0f, 1.0f));
			//cam.SetEyePosVector(XMVectorSet(30.0f, 40.0f, -17.0f, 1.0f));
			cam.SetFocusVector(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
			cam.SetUpVector(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			float aspect = view_port[VIEWPORT::DEFAULT].Width / view_port[VIEWPORT::DEFAULT].Height;
			viewRatio = aspect;

			cam.SetProjectionMatrix(XMMatrixPerspectiveFovLH(3.1415926f / 4.0f, aspect, 0.01f, 100.0f));

			default_view.view_mat = (float4x4_a&)cam.GetViewMatrix();  //XMMatrixInverse(nullptr, XMMatrixLookAtLH(eyepos, focus, up));
			default_view.proj_mat = (float4x4_a&)cam.GetProjectionMatrix(); //XMMatrixPerspectiveFovLH(3.1415926f / 4.0f, aspect, 0.01f, 100.0f);
		}

		void draw_view(view_t& view)
		{
			const float4 black{ 0.0f, 0.0f, 0.0f, 1.0f };

			context->OMSetDepthStencilState(depthStencilState[STATE_DEPTH_STENCIL::DEFAULT], 1);
			context->OMSetRenderTargets(1, &render_target[VIEW_RENDER_TARGET::DEFAULT], depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);

			context->ClearRenderTargetView(render_target[VIEW_RENDER_TARGET::DEFAULT], black.data());
			context->ClearDepthStencilView(depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			context->RSSetState(rasterState[STATE_RASTERIZER::DEFAULT]);
			context->RSSetViewports(1, &view_port[VIEWPORT::DEFAULT]);

			context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);

			MVP_t mvp;

			mvp.modeling = XMMatrixIdentity(); //XMMatrixTranspose(XMMatrixIdentity());
			mvp.projection = (XMMATRIX&)view.proj_mat; // XMMatrixTranspose((XMMATRIX&)view.proj_mat);
			mvp.view = (XMMATRIX&)view.view_mat; // XMMatrixTranspose(XMMatrixInverse(nullptr, (XMMATRIX&)view.view_mat));

			context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::MVP], 0, NULL, &mvp, 0, 0);
			////////////////////

			//VERTEX BUFFER
			create_vertex_buffer();

			////////////
			// set vb, vs, ps, il, topology, cb, etc.
			UINT strides[] = { sizeof(colored_vertex) };
			UINT offsets[] = { 0 };
			context->IASetVertexBuffers(0, 1, &vertex_buffer[VERTEX_BUFFER::COLORED_VERTEX], strides, offsets);

			context->IASetInputLayout(input_layout[INPUT_LAYOUT::COLORED_VERTEX]);
			// Set Shaders
			context->VSSetShader(vertex_shader[VERTEX_SHADER::COLORED_VERTEX], nullptr, 0);
			context->PSSetShader(pixel_shader[PIXEL_SHADER::COLORED_VERTEX], nullptr, 0);

			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

			context->UpdateSubresource(vertex_buffer[VERTEX_BUFFER::COLORED_VERTEX], 0, NULL, end::debug_renderer::get_line_verts(), 0, 0);

			context->Draw(end::debug_renderer::get_line_vert_count(), 0);
			end::debug_renderer::clear_lines();

			/////////////////////////////

			create_vertex_buffer_terrain();
			UINT strides2[] = { sizeof(SimpleVertex) };
			context->IASetVertexBuffers(0, 1, &vertex_buffer[VERTEX_BUFFER::WIRE_FRAME], strides2, offsets);

			context->IASetInputLayout(input_layout_wire);
			// Set Shaders
			context->VSSetShader(vertex_shader[VERTEX_SHADER::WIRE_FRAME], nullptr, 0);
			context->PSSetShader(pixel_shader[PIXEL_SHADER::WIRE_FRAME], nullptr, 0);

			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//XMMATRIX temp = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(-30.f, -15.0f, 0.f));
			//mvp.modeling = temp;

			//context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::MVP], 0, NULL, &mvp, 0, 0);

			context->UpdateSubresource(vertex_buffer[VERTEX_BUFFER::WIRE_FRAME], 0, NULL, surface.data(), 0, 0);

			context->Draw(size, 0);
			////////////////////////

			swapchain->Present(0, 0);
		}



		~impl_t()
		{
			// TODO:
			//Clean-up
			//
			// In general, release objects in reverse order of creation

			for (auto& ptr : constant_buffer)
				safe_release(ptr);

			for (auto& ptr : pixel_shader)
				safe_release(ptr);

			for (auto& ptr : vertex_shader)
				safe_release(ptr);

			for (auto& ptr : input_layout)
				safe_release(ptr);

			for (auto& ptr : index_buffer)
				safe_release(ptr);

			for (auto& ptr : vertex_buffer)
				safe_release(ptr);

			for (auto& ptr : rasterState)
				safe_release(ptr);

			for (auto& ptr : depthStencilState)
				safe_release(ptr);

			for (auto& ptr : depthStencilView)
				safe_release(ptr);

			for (auto& ptr : render_target)
				safe_release(ptr);

			safe_release(context);
			safe_release(swapchain);
			safe_release(device);

			clear_bvh_recursive(root);
		}

		void create_device_and_swapchain()
		{
			RECT crect;
			GetClientRect(hwnd, &crect);

			// Setup the viewport
			D3D11_VIEWPORT& vp = view_port[VIEWPORT::DEFAULT];

			vp.Width = (float)crect.right;
			vp.Height = (float)crect.bottom;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;

			// Setup swapchain
			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.BufferCount = 1;
			sd.BufferDesc.Width = crect.right;
			sd.BufferDesc.Height = crect.bottom;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.RefreshRate.Numerator = 60;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = hwnd;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = TRUE;

			D3D_FEATURE_LEVEL  FeatureLevelsSupported;

			const D3D_FEATURE_LEVEL lvl[] =
			{
				D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
			};

			UINT createDeviceFlags = 0;

#ifdef _DEBUG
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, lvl, _countof(lvl), D3D11_SDK_VERSION, &sd, &swapchain, &device, &FeatureLevelsSupported, &context);

			if (hr == E_INVALIDARG)
			{
				hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &lvl[1], _countof(lvl) - 1, D3D11_SDK_VERSION, &sd, &swapchain, &device, &FeatureLevelsSupported, &context);
			}

			assert(!FAILED(hr));
		}

		void create_main_render_target()
		{
			ID3D11Texture2D* pBackBuffer;
			// Get a pointer to the back buffer
			HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
				(LPVOID*)&pBackBuffer);

			assert(!FAILED(hr));

			// Create a render-target view
			device->CreateRenderTargetView(pBackBuffer, NULL,
				&render_target[VIEW_RENDER_TARGET::DEFAULT]);

			pBackBuffer->Release();
		}

		void setup_depth_stencil()
		{
			/* DEPTH_BUFFER */
			D3D11_TEXTURE2D_DESC depthBufferDesc;
			ID3D11Texture2D* depthStencilBuffer;

			ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

			depthBufferDesc.Width = (UINT)view_port[VIEWPORT::DEFAULT].Width;
			depthBufferDesc.Height = (UINT)view_port[VIEWPORT::DEFAULT].Height;
			depthBufferDesc.MipLevels = 1;
			depthBufferDesc.ArraySize = 1;
			depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthBufferDesc.SampleDesc.Count = 1;
			depthBufferDesc.SampleDesc.Quality = 0;
			depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthBufferDesc.CPUAccessFlags = 0;
			depthBufferDesc.MiscFlags = 0;

			HRESULT hr = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);

			assert(!FAILED(hr));

			/* DEPTH_STENCIL */
			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;

			hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);

			assert(!FAILED(hr));

			depthStencilBuffer->Release();

			/* DEPTH_STENCIL_DESC */
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState[STATE_DEPTH_STENCIL::DEFAULT]);

			assert(!FAILED(hr));
		}

		void setup_rasterizer()
		{
			D3D11_RASTERIZER_DESC rasterDesc;

			ZeroMemory(&rasterDesc, sizeof(rasterDesc));

			rasterDesc.AntialiasedLineEnable = true;
			rasterDesc.CullMode = D3D11_CULL_BACK;
			rasterDesc.DepthBias = 0;
			rasterDesc.DepthBiasClamp = 0.0f;
			rasterDesc.DepthClipEnable = false;
			rasterDesc.FillMode = (D3D11_FILL_MODE)rasterFillType;//D3D11_FILL_SOLID;
			rasterDesc.FrontCounterClockwise = false;
			rasterDesc.MultisampleEnable = false;
			rasterDesc.ScissorEnable = false;
			rasterDesc.SlopeScaledDepthBias = 0.0f;

			HRESULT hr = device->CreateRasterizerState(&rasterDesc, &rasterState[STATE_RASTERIZER::DEFAULT]);

			assert(!FAILED(hr));
		}

		void create_shaders()
		{
			binary_blob_t vs_blob = load_binary_blob("vs_cube.cso");
			binary_blob_t ps_blob = load_binary_blob("ps_cube.cso");

			HRESULT hr = device->CreateVertexShader(vs_blob.data(), vs_blob.size(), NULL, &vertex_shader[VERTEX_SHADER::BUFFERLESS_CUBE]);

			assert(!FAILED(hr));

			hr = device->CreatePixelShader(ps_blob.data(), ps_blob.size(), NULL, &pixel_shader[PIXEL_SHADER::BUFFERLESS_CUBE]);

			assert(!FAILED(hr));

			vs_blob = load_binary_blob("vs_line.cso");
			ps_blob = load_binary_blob("ps_line.cso");

			hr = device->CreateVertexShader(vs_blob.data(), vs_blob.size(), NULL, &vertex_shader[VERTEX_SHADER::COLORED_VERTEX]);

			assert(!FAILED(hr));

			hr = device->CreatePixelShader(ps_blob.data(), ps_blob.size(), NULL, &pixel_shader[PIXEL_SHADER::COLORED_VERTEX]);

			assert(!FAILED(hr));

			binary_blob_t vs_blob2 = load_binary_blob("vs_terrain.cso");
			binary_blob_t ps_blob2 = load_binary_blob("ps_terrain.cso");

			hr = device->CreateVertexShader(vs_blob2.data(), vs_blob2.size(), NULL, &vertex_shader[VERTEX_SHADER::WIRE_FRAME]);

			assert(!FAILED(hr));

			hr = device->CreatePixelShader(ps_blob2.data(), ps_blob2.size(), NULL, &pixel_shader[PIXEL_SHADER::WIRE_FRAME]);


			///////
			// create input layout
						// Input Layout
			// describe the vertex to D3D11
			D3D11_INPUT_ELEMENT_DESC ieDesc[] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			hr = device->CreateInputLayout(ieDesc, ARRAYSIZE(ieDesc), vs_blob.data(), vs_blob.size(), input_layout);

			D3D11_INPUT_ELEMENT_DESC inputDesc[] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },    // APPEND_ALIGNED_ELEMENT will automatically calculate the byte offset
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,   D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vs_blob2.data(), vs_blob2.size(), &input_layout_wire);
		}

		void create_vertex_buffer()
		{
			D3D11_BUFFER_DESC bDesc;
			D3D11_SUBRESOURCE_DATA subData;
			ZeroMemory(&bDesc, sizeof(bDesc));
			ZeroMemory(&subData, sizeof(subData));

			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(end::colored_vertex) * end::debug_renderer::get_line_vert_count();
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = end::debug_renderer::get_line_verts();

			HRESULT hr = device->CreateBuffer(&bDesc, &subData, &vertex_buffer[VERTEX_BUFFER::COLORED_VERTEX]);
		}

		void create_vertex_buffer_terrain()
		{

			D3D11_BUFFER_DESC bDesc;
			D3D11_SUBRESOURCE_DATA subData;
			ZeroMemory(&bDesc, sizeof(bDesc));
			ZeroMemory(&subData, sizeof(subData));

			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(SimpleVertex) * vert_count;
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = surface.data();//end::debug_renderer::get_line_verts();

			HRESULT hr = device->CreateBuffer(&bDesc, &subData, &vertex_buffer[VERTEX_BUFFER::WIRE_FRAME]);

		}

		void create_constant_buffers()
		{
			D3D11_BUFFER_DESC mvp_bd;
			ZeroMemory(&mvp_bd, sizeof(mvp_bd));

			mvp_bd.Usage = D3D11_USAGE_DEFAULT;
			mvp_bd.ByteWidth = sizeof(MVP_t);
			mvp_bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			mvp_bd.CPUAccessFlags = 0;

			HRESULT hr = device->CreateBuffer(&mvp_bd, NULL, &constant_buffer[CONSTANT_BUFFER::MVP]);
		}

		void update(view_t& view)
		{

			xtimer.Signal();
			delta_time = xtimer.Delta();

#pragma region "Controls"
			if (GetAsyncKeyState(0x57) & 0x1) //w
			{
				cam.AdjustPosition(cam.GetForwardVector() * 0.3f);
			}
			//A
			if (GetAsyncKeyState(0x41) & 0x1) //a
			{
				cam.AdjustPosition(cam.GetLeftVector() * 0.3f);
			}
			//S
			if (GetAsyncKeyState(0x53) & 0x1) // s
			{
				cam.AdjustPosition(cam.GetBackVector() * 0.3f);
			}
			//D
			if (GetAsyncKeyState(0x44) & 0x1) // d
			{
				cam.AdjustPosition(cam.GetRightVector() * 0.3f);
			}
			//Q
			if (GetAsyncKeyState(0x51) & 0x1) // Q
			{
				cam.AdjustPosition(cam.GetUpVector() * 0.3f);
			}
			//E
			if (GetAsyncKeyState(0x45) & 0x1) // E
			{
				cam.AdjustPosition(cam.GetDownVector() * 0.3f);
			}

			cam.UpdateCamera();
			view.view_mat = (float4x4_a&)cam.GetViewMatrix();

#pragma region "Matrix Controls"
			if (GetAsyncKeyState(VK_LEFT) & 0x1) //<
			{
				target.position = (float4x4&)XMMatrixMultiply(XMMatrixRotationY(-delta_time * 15),
					(XMMATRIX&)target.position);
			}
			if (GetAsyncKeyState(VK_RIGHT) & 0x1) //<
			{

				target.position = (float4x4&)XMMatrixMultiply(XMMatrixRotationY(delta_time * 15),
					(XMMATRIX&)target.position);
			}
			if (GetAsyncKeyState(VK_UP) & 0x1) //<
			{
				target.position = (float4x4&)XMMatrixMultiply(XMMatrixTranslation(0.0f, 0.0f, 0.1f),
					(XMMATRIX&)target.position);
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x1) //<
			{
				target.position = (float4x4&)XMMatrixMultiply(XMMatrixTranslation(0.0f, 0.0f, -0.1f),
					(XMMATRIX&)target.position);
			}
			if (GetAsyncKeyState(VK_OEM_2) & 0x1) //?/
			{
				target.position = (float4x4&)XMMatrixMultiply(XMMatrixRotationX(-delta_time * 15),
					(XMMATRIX&)target.position);
			}
			if (GetAsyncKeyState(VK_RSHIFT) & 0x1) // Right Shift
			{
				target.position = (float4x4&)XMMatrixMultiply(XMMatrixRotationX(delta_time * 15),
					(XMMATRIX&)target.position);
			}
#pragma endregion


			if (GetAsyncKeyState(0x31) & 0x01)
			{
				rasterFillType = 2;
				setup_rasterizer();
			}
			if (GetAsyncKeyState(0x32) & 0x01)
			{
				rasterFillType = 3;
				setup_rasterizer();
			}
#pragma endregion

			// Seconjd Matrix Rotating
			float4x4 look = look_at((float3&)w_matrixLook[3], (float3&)target.position[3], { 0,1,0 });

			w_matrixLook = look;

			float4x4 turn = turn_to(w_matrixTurn, (float3&)target.position[3], delta_time);

			w_matrixTurn = turn;

			// Re-calculate player and Draw
			update_target();

			// Draw all colliding nodes in the tree
			draw_tree(root);

			if (aabb_abbb_collision(target.aabb, root->element.aabb))
			{
				draw_aabb(root->element.aabb);
			}

			// Frustum on player
			calculate_frustum(frustum, 3.1415926f / 4.0f, viewRatio, 1.0f, 7.0f, target.position);

			// Draw player and matrices
			create_axis();
		}

		void create_axis()
		{
			end::debug_renderer::add_line(target.position[3].xyz, target.position[3].xyz + target.position[0].xyz, { 1, 0, 0, 1 });
			end::debug_renderer::add_line(target.position[3].xyz, target.position[3].xyz + target.position[1].xyz, { 0, 1, 0, 1 });
			end::debug_renderer::add_line(target.position[3].xyz, target.position[3].xyz + target.position[2].xyz, { 0, 0, 1, 1 });

			end::debug_renderer::add_line(w_matrixTurn[3].xyz, w_matrixTurn[3].xyz + w_matrixTurn[0].xyz, { 1, 0, 0, 1 });
			end::debug_renderer::add_line(w_matrixTurn[3].xyz, w_matrixTurn[3].xyz + w_matrixTurn[1].xyz, { 0, 1, 0, 1 });
			end::debug_renderer::add_line(w_matrixTurn[3].xyz, w_matrixTurn[3].xyz + w_matrixTurn[2].xyz, { 0, 0, 1, 1 });


			end::debug_renderer::add_line(w_matrixLook[3].xyz, w_matrixLook[3].xyz + w_matrixLook[0].xyz, { 1, 0, 0, 1 });
			end::debug_renderer::add_line(w_matrixLook[3].xyz, w_matrixLook[3].xyz + w_matrixLook[1].xyz, { 0, 1, 0, 1 });
			end::debug_renderer::add_line(w_matrixLook[3].xyz, w_matrixLook[3].xyz + w_matrixLook[2].xyz, { 0, 0, 1, 1 });

			// matrix.pos + (matrix.xaxis * scalar)
		}

		void mouse_look(float x, float y)
		{
			cam.MouseRotation(x, y);
		}

		float random_number(float min, float max)
		{
			return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
		}

		float4x4 look_at(float3 eyePos, float3 target, float3 upDirection)
		{
			float3 zAxis = normalize(target - eyePos);
			float3 xAxis = normalize(cross(upDirection, zAxis));
			float3 yAxis = normalize(cross(zAxis, xAxis));

			float4x4 result =
			{
				xAxis.x, xAxis.y, xAxis.z, 0,
				yAxis.x, yAxis.y, yAxis.z, 0,
				zAxis.x, zAxis.y, zAxis.z, 0,
				eyePos.x, eyePos.y, eyePos.z, 1,

				//xAxis.x, yAxis.x, zAxis.x, 0,
				//xAxis.y, yAxis.y, zAxis.y, 0,
				//xAxis.z, yAxis.z, zAxis.z, 0,
			};

			return result;
		}

		float4x4 turn_to(float4x4 looker, float3 target, float delta)
		{
			float3 v = normalize(target - looker[3].xyz);
			float dotY = dot(v, looker[0].xyz);
			float dotX = dot(v, looker[1].xyz);
			XMMATRIX rotX = XMMatrixRotationX(-dotX * delta);
			XMMATRIX rotY = XMMatrixRotationY(dotY * delta);
			looker = (float4x4&)XMMatrixMultiply(rotX, (XMMATRIX&)looker);
			looker = (float4x4&)XMMatrixMultiply(rotY, (XMMATRIX&)looker);

			return orthonormalize(looker);
		}

		float4x4 orthonormalize(float4x4 looker)
		{
			float3 worldUp = { 0,1,0 };
			float3 zAxis = normalize(looker[2].xyz);
			float3 xAxis = normalize(cross(worldUp, zAxis));
			float3 yAxis = normalize(cross(zAxis, xAxis));

			float4x4 result =
			{
				xAxis.x, xAxis.y, xAxis.z, 0,
				yAxis.x, yAxis.y, yAxis.z, 0,
				zAxis.x, zAxis.y, zAxis.z, 0,
				looker[3].x, looker[3].y, looker[3].z, 1,
			};

			return result;
		}


		// BVH FUNCTIONS

		void update_target()
		{
			target.aabb.max = { (target.width + target.position[3].x), (target.height + target.position[3].y), (target.depth + target.position[3].z) };
			target.aabb.min = { (-target.width + target.position[3].x), (-target.height + target.position[3].y), (-target.depth + target.position[3].z) };
			target.aabb.center = { target.position[3].x, target.position[3].y , target.position[3].z };
			target.aabb.extents = { target.width, target.height, target.depth };
			target.aabb.color = { 0.0f, 0.0f, 0.75f, 1.0f };
			target.centroid = target.aabb.center;

			draw_aabb(target.aabb);
		}
		
		void load_terrain()
		{
			std::fstream in("terrain.bin", std::ios_base::binary | std::ios_base::in);

			if (in.is_open())
			{
				in.read((char*)&size, sizeof(UINT));

				pos.resize(size);
				norms.resize(size);
				uvs.resize(size);

				vert_count = size;

				in.read((char*)pos.data(), sizeof(float3) * size);
				in.read((char*)norms.data(), sizeof(float3) * size);
				in.read((char*)uvs.data(), sizeof(float2) * size);


				in.close();
			}

			for (int i = 0; i < vert_count; i++)
			{
				SimpleVertex v;
				v.pos = { pos[i].x - 30.0f, pos[i].y - 15.0f, pos[i].z - 30.0f };
				v.Normal = norms[i];
				v.Tex = uvs[i];
				surface.push_back(v);
			}

			for (size_t i = 0; i < vert_count; i+=3)
			{
				float3 centroid;
				centroid.x = (surface[i].pos.x + surface[i + 1].pos.x + surface[i + 2].pos.x) / 3;
				centroid.y = (surface[i].pos.y + surface[i + 1].pos.y + surface[i + 2].pos.y) / 3;
				centroid.z = (surface[i].pos.z + surface[i + 1].pos.z + surface[i + 2].pos.z) / 3;

				terrain.push_back({ centroid, calculate_aabb_from_triangle(surface[i].pos, surface[i + 1].pos, surface[i + 2].pos) });
			}
		}

		// Recursive function to draw all nodes
		void draw_tree(Node* _node)
		{
			if (_node->left != nullptr && _node->right != nullptr && aabb_abbb_collision(target.aabb, root->element.aabb)) //reminder make the player targets[0]
			{
				if (aabb_abbb_collision(target.aabb, _node->left->element.aabb))
				{
					draw_aabb(_node->left->element.aabb);
					draw_tree(_node->left);
				}

				if (aabb_abbb_collision(target.aabb, _node->right->element.aabb))
				{
					draw_aabb(_node->right->element.aabb);
					draw_tree(_node->right);
				}
			}
		}

		// Build BVH Tree
		void construct_tree()
		{
			root = new Node(terrain[0]);

			for (size_t i = 1; i < terrain.size(); i++)
			{
				Node* current = root;
				Node* new_node = new Node(terrain[i]);
				int debug = 0;
				while (current->left != nullptr && current->right != nullptr)
				{
					current->element.aabb.max = get_max(current->element.aabb.max, new_node->element.aabb.max);
					current->element.aabb.min = get_min(current->element.aabb.min, new_node->element.aabb.min);
					current->element.centroid.x = (current->element.aabb.min.x + current->element.aabb.max.x) / 2;
					current->element.centroid.y = (current->element.aabb.min.y + current->element.aabb.max.y) / 2;
					current->element.centroid.z = (current->element.aabb.min.z + current->element.aabb.max.z) / 2;
					current = manhattan_distance(new_node->element.centroid, current->left->element.centroid) <
						manhattan_distance(new_node->element.centroid, current->right->element.centroid) ?
						current->left : current->right;
				}

				current->left = new Node(current->element);
				current->right = new_node;
				current->element.aabb.max = get_max(current->element.aabb.max, new_node->element.aabb.max);
				current->element.aabb.min = get_min(current->element.aabb.min, new_node->element.aabb.min);
				current->element.centroid.x = (current->element.aabb.min.x + current->element.aabb.max.x) / 2;
				current->element.centroid.y = (current->element.aabb.min.y + current->element.aabb.max.y) / 2;
				current->element.centroid.z = (current->element.aabb.min.z + current->element.aabb.max.z) / 2;

			}
		}

		void clear_bvh_recursive(Node* root)
		{
			if (root != nullptr)
			{
				clear_bvh_recursive(root->left);
				clear_bvh_recursive(root->right);
				delete root;
			}
		}

		bool aabb_abbb_collision(aabb_t a, aabb_t b)
		{
			if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
			if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
			if (a.max.z < b.min.z || a.min.z >  b.max.z) return false;

			return true;
		}

		aabb_t calculate_aabb_from_triangle(float3 a, float3 b, float3 c)
		{
			aabb_t _aabb;
			_aabb.max = { get_max(a.x, b.x, c.x), get_max(a.y, b.y, c.y), get_max(a.z, b.z, c.z) };
			_aabb.min = { get_min(a.x, b.x, c.x), get_min(a.y, b.y, c.y), get_min(a.z, b.z, c.z) };
			_aabb.color = { 1.0f, 0.0f, 0.0f, 1.0f };
			return _aabb;
		}

		void draw_aabb(aabb_t& aabb)
		{

			float3 FTR = aabb.max;
			float3 FTL = { aabb.min.x, aabb.max.y, aabb.max.z };
			float3 FBR = { aabb.max.x, aabb.min.y, aabb.max.z };
			float3 FBL = { aabb.min.x, aabb.min.y, aabb.max.z };
			float3 NTR = { aabb.max.x, aabb.max.y, aabb.min.z };
			float3 NTL = { aabb.min.x, aabb.max.y, aabb.min.z };
			float3 NBR = { aabb.max.x, aabb.min.y, aabb.min.z };
			float3 NBL = aabb.min;



			debug_renderer::add_line(NTR, NTL, aabb.color);
			debug_renderer::add_line(NBR, NBL, aabb.color);
			debug_renderer::add_line(NTL, NBL, aabb.color);
			debug_renderer::add_line(NTR, NBR, aabb.color);
			debug_renderer::add_line(FTR, FTL, aabb.color);
			debug_renderer::add_line(FBR, FBL, aabb.color);
			debug_renderer::add_line(FTL, FBL, aabb.color);
			debug_renderer::add_line(FTR, FBR, aabb.color);
			debug_renderer::add_line(FTR, NTR, aabb.color);
			debug_renderer::add_line(FTL, NTL, aabb.color);
			debug_renderer::add_line(FBR, NBR, aabb.color);
			debug_renderer::add_line(FBL, NBL, aabb.color);

		}

		float3 get_max(float3 a, float3 b)
		{
			float3 max;
			if (a.x > b.x)
				max.x = a.x;
			else
				max.x = b.x;

			if (a.y > b.y)
				max.y = a.y;
			else
				max.y = b.y;

			if (a.z > b.z)
				max.z = a.z;
			else
				max.z = b.z;

			return max;
		}

		float3 get_min(float3 a, float3 b)
		{
			float3 min;
			if (a.x < b.x)
				min.x = a.x;
			else
				min.x = b.x;

			if (a.y < b.y)
				min.y = a.y;
			else
				min.y = b.y;

			if (a.z < b.z)
				min.z = a.z;
			else
				min.z = b.z;

			return min;
		}

		float get_max(float a, float b, float c)
		{
			float max;
			if (a > b)
				max = a;
			else
				max = b;
			if (max < c)
				max = c;
			return max;
		}

		float get_min(float a, float b, float c)
		{
			float min;
			if (a < b)
				min = a;
			else
				min = b;
			if (min > c)
				min = c;
			return min;
		}

		float manhattan_distance(float3 a, float3 b)
		{
			return fabsf(a.x - b.x) + fabsf(a.y - b.y) + fabsf(a.z - b.z);
		}

	};
}
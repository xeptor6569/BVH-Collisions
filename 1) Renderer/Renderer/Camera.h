#pragma once

#include <d3d11.h>
#include <DirectXMath.h>  // Directs  (gateware also has its own)
#include "math_types.h"

using namespace DirectX;
using namespace end;

class Camera
{
	//XMVECTOR eyePosVector = { 0.0f, 25.0f, -17.0f, 1.0f };
	XMVECTOR eyePosVector = { 30.0f, 40.0f, -17.0f, 1.0f };
	XMFLOAT3 eyePos = { 30.0f, 40.0f, -17.0f };
	XMFLOAT3 eyeRot = { 0.65,0,0 };
	XMVECTOR eyeRotVector;
	XMVECTOR focusVector = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMVECTOR upVector = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMFLOAT3 focus = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 up = { 0.0f, 1.0f, 0.0f};


	XMVECTOR vUp = { 0,1,0 };
	XMVECTOR vDown = { 0,-1,0 };
	XMVECTOR vForward = { 0,0,1 };
	XMVECTOR vLeft = { -1,0,0 };
	XMVECTOR vRight = { 1, 0, 0 };
	XMVECTOR vBack = { 0,0,-1 };

	XMVECTOR vUpD = { 0,1,0 };
	XMVECTOR vDownD = { 0,-1,0 };
	XMVECTOR vForwardD = { 0,0,1 };
	XMVECTOR vLeftD = { -1,0,0 };
	XMVECTOR vRightD = { 1, 0, 0 };
	XMVECTOR vBackD = { 0,0,-1 };

	XMMATRIX viewMatrix = XMMatrixLookAtLH(eyePosVector, focusVector, upVector);//{ 0,0,0 }, { 0,1,0 });;
	XMMATRIX projectionMatrix;

	float x = 0.0f;
	float y = 0.0f;
	float outState = 0.0f;


public:

	void UpdateCamera();

	void MouseRotation(float _x, float _y);
	void RotateCamera(float _x, float _y);
	void SetEyePos(XMFLOAT3 _eyePos); 
	void SetProjectionMatrix(XMMATRIX _projMatrix);
	void SetEyeRot(XMFLOAT3 _eyeRot);
	void SetEyeRotVector(XMVECTOR _eyeRot);
	void SetEyePosVector(XMVECTOR _eyePosVector);
	void SetFocusVector(XMVECTOR _focusVector);
	void SetUpVector(XMVECTOR _upVector);


	XMFLOAT3 GetEyePos();
	XMVECTOR GetEyePosVector(); 

	XMFLOAT3 GetEyeRot();
	XMVECTOR GetEyeRotVector();

	XMMATRIX GetViewMatrix();     
	XMMATRIX GetProjectionMatrix(); 

	XMVECTOR GetForwardVector();
	XMVECTOR GetBackVector();
	XMVECTOR GetLeftVector();
	XMVECTOR GetRightVector();
	XMVECTOR GetUpVector();
	XMVECTOR GetDownVector();

	XMFLOAT3 GetCamTarget();
	XMFLOAT3 GetUpDirection();


	void AdjustPosition(const XMVECTOR& pos);
};


#pragma region "Camera Get/Set"
// Everything Camera
void Camera::SetProjectionMatrix(XMMATRIX _projMatrix)
{
	this->projectionMatrix = _projMatrix;
}

void Camera::SetEyePos(XMFLOAT3 _eyePos)
{
	this->eyePos = _eyePos;
}

void Camera::SetEyePosVector(XMVECTOR _eyePosVector)
{
	this->eyePosVector = _eyePosVector;
	XMStoreFloat3(&eyePos, eyePosVector);
}

void Camera::SetEyeRot(XMFLOAT3 _eyeRot)
{
	this->eyeRot = _eyeRot;
}

void Camera::SetEyeRotVector(XMVECTOR _eyeRotVector)
{
	this->eyeRotVector = _eyeRotVector;
	XMStoreFloat3(&eyeRot, eyeRotVector);
}

void Camera::SetFocusVector(XMVECTOR _focusVector)
{
	this->focusVector = _focusVector;
	XMStoreFloat3(&focus, eyeRotVector);
}

void Camera::SetUpVector(XMVECTOR _upVector)
{
	this->upVector = _upVector;
	XMStoreFloat3(&up, upVector);
}


XMFLOAT3 Camera::GetEyePos()
{
	return this->eyePos;
}
XMVECTOR Camera::GetEyePosVector()
{
	return this->eyePosVector;
}
XMFLOAT3 Camera::GetEyeRot()
{
	return this->eyeRot;
}
XMVECTOR Camera::GetEyeRotVector()
{
	return this->eyeRotVector;
}
XMVECTOR Camera::GetForwardVector()
{
	return this->vForward;
}
XMVECTOR Camera::GetRightVector()
{
	return this->vRight;
}
XMVECTOR Camera::GetLeftVector()
{
	return this->vLeft;
}
XMVECTOR Camera::GetBackVector()
{
	return this->vBack;
}
XMVECTOR Camera::GetUpVector()
{
	return this->vUp;
}
XMVECTOR Camera::GetDownVector()
{
	return this->vDown;
}
XMMATRIX Camera::GetViewMatrix()
{
	return this->viewMatrix;
}
XMMATRIX Camera::GetProjectionMatrix()
{
	return this->projectionMatrix;
}

XMFLOAT3 Camera::GetCamTarget()
{
	return this->focus;
}
XMFLOAT3 Camera::GetUpDirection()
{
	return this->up;
}

#pragma endregion



void Camera::AdjustPosition(const XMVECTOR& pos)
{
	this->eyePosVector += pos;
	XMStoreFloat3(&this->eyePos, this->eyePosVector);
	//this->UpdateCamera();
}

void Camera::MouseRotation(float _y, float _x)
{
		this->eyeRot.x += _x * 0.0015f;
		this->eyeRot.y += _y * 0.0015f;
		this->eyeRot.z += 0;
		eyeRotVector = XMLoadFloat3(&eyeRot);
}

void Camera::RotateCamera(float _x, float _y)
{
	this->eyeRot.x += _x*1.2;
	this->eyeRot.y += _y*1.2;
	this->eyeRot.z += 0;
	eyeRotVector = XMLoadFloat3(&eyeRot);
}

void Camera::UpdateCamera()
{
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(this->eyeRot.x, this->eyeRot.y, 0.0f);
	XMVECTOR camTarget = XMVector3TransformCoord(this->vForwardD, rotMatrix);

	camTarget += this->eyePosVector;
	//XMStoreFloat3(&eyePos, eyePosVector);

	XMVECTOR upDir = XMVector3TransformCoord(this->vUpD, rotMatrix);

	this->viewMatrix = XMMatrixLookAtLH(eyePosVector, camTarget, upDir);

	XMMATRIX rotMatrix2 = XMMatrixRotationRollPitchYaw(eyeRot.x, eyeRot.y, 0.0f);
	vForward = XMVector3TransformCoord(vForwardD, rotMatrix2);
	vLeft = XMVector3TransformCoord(vLeftD, rotMatrix2);
	vRight = XMVector3TransformCoord(vRightD, rotMatrix2);
	vBack = XMVector3TransformCoord(vBackD, rotMatrix2);
	vUp = XMVector3TransformCoord(vUpD, rotMatrix2);
	vDown = XMVector3TransformCoord(vDownD, rotMatrix2);
}


////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_position.x = 0.0f;
	m_position.y = 0.0f;
	m_position.z = 0.0f;

	m_rotation.x = 0.0f;
	m_rotation.y = 0.0f;
	m_rotation.z = 0.0f;



	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// 빌보딩 기반 카메라 이동 관련 초기화
	m_defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	m_camForward = m_defaultForward;
	m_camRight = m_defaultRight;

	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;

	m_camYaw = 0.0f;
	m_camPitch = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}


XMFLOAT3 CameraClass::GetPosition()
{
	return m_position;
}


XMFLOAT3 CameraClass::GetRotation()
{
	return m_rotation;
}

// This uses the position and rotation of the camera to build and to update the view matrix.
void CameraClass::Render()
{
	XMVECTOR up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	position = XMLoadFloat3(&m_position);
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// 여기서 m_camYaw, m_camPitch를 사용!
	pitch = m_camPitch;
	yaw = m_camYaw;
	roll = 0.0f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	lookAt = position + lookAt;

	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);

	return;
}


void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void CameraClass::UpdateCamera()
{

	if (m_camPitch > XM_PIDIV2 - 0.1f) m_camPitch = XM_PIDIV2 - 0.1f;
	if (m_camPitch < -XM_PIDIV2 + 0.1f) m_camPitch = -XM_PIDIV2 + 0.1f;


	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_camPitch, m_camYaw, 0);

	XMVECTOR camTarget = XMVector3TransformCoord(m_defaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	XMMATRIX rotateYTempMatrix = XMMatrixRotationY(m_camYaw);

	m_camRight = XMVector3TransformCoord(m_defaultRight, rotateYTempMatrix);
	m_camForward = XMVector3TransformCoord(m_defaultForward, rotateYTempMatrix);

	XMVECTOR pos = XMLoadFloat3(&m_position);
	pos += m_moveLeftRight * m_camRight;
	pos += m_moveBackForward * m_camForward;

	XMStoreFloat3(&m_position, pos);

	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;

	camTarget = pos + camTarget;
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_viewMatrix = XMMatrixLookAtLH(pos, camTarget, up);
}

void CameraClass::ResetMovement()
{
	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;
}


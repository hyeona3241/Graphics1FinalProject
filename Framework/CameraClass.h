////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <directxmath.h>

#include "AlignedAllocationPolicy.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;


public:
	XMVECTOR m_defaultForward;
	XMVECTOR m_defaultRight;
	XMVECTOR m_camForward;
	XMVECTOR m_camRight;

	float m_moveLeftRight;
	float m_moveBackForward;

	float m_camYaw;
	float m_camPitch;

	// ī�޶� �̵� ������Ʈ �Լ�
	void UpdateCamera();

	// ī�޶� �̵� ���� �ʱ�ȭ (����)
	void ResetMovement();
};

#endif
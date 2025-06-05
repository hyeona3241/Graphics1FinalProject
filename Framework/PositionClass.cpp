#include "stdafx.h"
#include "PositionClass.h"


PositionClass::PositionClass()
{
}


PositionClass::PositionClass(const PositionClass& other)
{
}


PositionClass::~PositionClass()
{
}


void PositionClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}


void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT3(x, y, z);
}


void PositionClass::GetPosition(XMFLOAT3& p)
{
	p = m_position;
}


void PositionClass::GetRotation(XMFLOAT3& p)
{
	p = m_rotation;
}


void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
}


void PositionClass::MoveLeft(bool keydown)
{
	// Ű�� ������ ī�޶� �������� ���ƴ� �ӵ��� �����մϴ�. ȸ�� �ӵ��� ������ ������.
	if (keydown)
	{
		m_leftSpeed += m_frameTime * 0.001f;

		if(m_leftSpeed > (m_frameTime * 0.03f))
		{
			m_leftSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_leftSpeed -= m_frameTime * 0.0007f;

		if(m_leftSpeed < 0.0f)
		{
			m_leftSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	float radians = m_rotation.y * 0.0174532925f;

	// Update the position.
	m_position.x -= cosf(radians) * m_leftSpeed;
	m_position.z -= sinf(radians) * m_leftSpeed;
}


void PositionClass::MoveRight(bool keydown)
{
	// Ű�� ������ ī�޶� ���������� ȸ���ϴ� �ӵ��� �����մϴ�. ȸ�� �ӵ��� ������ ������.
	if (keydown)
	{
		m_rightSpeed += m_frameTime * 0.001f;

		if(m_rightSpeed > (m_frameTime * 0.03f))
		{
			m_rightSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_rightSpeed -= m_frameTime * 0.0007f;
		
		if(m_rightSpeed < 0.0f)
		{
			m_rightSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	float radians = m_rotation.y * 0.0174532925f;

	// Update the position.
	m_position.x += cosf(radians) * m_rightSpeed;
	m_position.z += sinf(radians) * m_rightSpeed;
}
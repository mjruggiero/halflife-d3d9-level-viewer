
#include "Camera.h"

CCamera::CCamera() : m_vEye(0.0, 0.0, 0.0), m_vDirection(0.0, 0.0, 1.0), 
	m_vUp(0.0, 1.0, 0.0), m_vRight(1.0, 0.0, 0.0), m_fPitch(0.0), m_fYaw(0.0)
{

}

CCamera::~CCamera()
{

}

void CCamera::Move(float speed)
{
	m_vEye += speed * m_vDirection;
}

void CCamera::Strafe(float speed)
{
	m_vEye += speed * m_vRight;
}

void CCamera::Yaw(float angle)
{
	//m_vEye += speed * m_vDirection;
	D3DXMATRIX matrix;
	D3DXMatrixRotationAxis(&matrix, &m_vUp, D3DXToRadian(angle));
	D3DXVec3TransformCoord(&m_vDirection, &m_vDirection, &matrix);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &matrix);
}

void CCamera::Pitch(float angle)
{
	//m_vEye += speed * m_vDirection;
	D3DXMATRIX matrix;
	D3DXMatrixRotationAxis(&matrix, &m_vRight, D3DXToRadian(angle));
	D3DXVec3TransformCoord(&m_vDirection, &m_vDirection, &matrix);
	//D3DXVec3TransformCoord(&m_vUp, &m_vUp, &matrix);
}

void CCamera::Update(LPDIRECT3DDEVICE9 &pDevice)
{
	D3DXMATRIX view;
	D3DXVECTOR3 vLook = m_vEye + m_vDirection;
	//D3DXVec3Normalize(&vLook, &vLook);
	D3DXMatrixLookAtRH(&view, &m_vEye, &vLook, &m_vUp);
	pDevice->SetTransform(D3DTS_VIEW, &view);
}

void CCamera::PositionCamera(float x, float y, float z, float lookx, float looky,
							 float lookz, float upx, float upy, float upz)
{
	m_vEye.x = x;
	m_vEye.y = y;
	m_vEye.z = z;
	m_vDirection.x = lookx;
	m_vDirection.y = looky;
	m_vDirection.z = lookz;
	m_vUp.x = upx;
	m_vUp.y = upy;
	m_vUp.z = upz;
}

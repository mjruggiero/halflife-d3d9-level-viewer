
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <d3dx9.h>

class CCamera
{
public:
	CCamera();
	~CCamera();
	void Move(float speed);
	void Strafe(float speed);
	void Yaw(float angle);
	void Pitch(float angle);
	void Update(LPDIRECT3DDEVICE9 &pDevice);
	void PositionCamera(float x, float y, float z,
		float lookx, float looky, float lookz,
		float upx, float upy, float upz);
	D3DXVECTOR3 Position() { return m_vEye; }

private:
	D3DXVECTOR3 m_vEye;
	D3DXVECTOR3 m_vDirection;
	D3DXVECTOR3 m_vUp;
	D3DXVECTOR3 m_vRight;
	float m_fPitch;
	float m_fYaw;

};

#endif // _CAMERA_H_

////////////////////////////////////////////////////////////////////////////////
//
// HLViewer.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include <d3d9.h>
#include <fstream>
#include <mmsystem.h>
#include "hlbsp.h"
#include "Camera.h"

using namespace std;

LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pD3DDevice = NULL;
HWND g_hWnd = NULL;

CCamera *g_pCamera = NULL;
CHLBsp *g_pLevel = NULL;

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 768;

bool bWireframe = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT InitD3D(HWND hWnd);
bool Init();
void Shutdown();
void Render();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wcl;

	wcl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wcl.lpfnWndProc = WndProc;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hInstance = hInstance;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = NULL;
	wcl.lpszMenuName = NULL;
	wcl.lpszClassName = "HalfLifeViewer";

	if(!RegisterClass(&wcl))
		return 0;

	hWnd = CreateWindow("HalfLifeViewer",
		"Half-Life Level Viewer",
		WS_OVERLAPPEDWINDOW,
		0, 0,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	if(!hWnd)
		return 0;

	g_hWnd = hWnd;
	
	ShowWindow(hWnd, nCmdShow);

	if(FAILED(InitD3D(hWnd)))
	{
		MessageBox(hWnd, "Couldn't initialize Direct3D", "Error", MB_OK);
		return 0;
	}
	
	if(!Init())
		return 0;
	
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_pCamera->Update(g_pD3DDevice);
			Render();
		}
	}

	Shutdown();

	return msg.lParam;
}

HRESULT InitD3D(HWND hWnd)
{
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(!g_pD3D)
		return E_FAIL;

	D3DDISPLAYMODE d3ddm;
	
	if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	
	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_pD3DDevice)))
		return E_FAIL;

	return D3D_OK;
}

bool Init()
{
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	g_pD3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	//g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	g_pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	g_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x000000DF);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	// read config file
	char strTexturePath[120];
	char strPakFilename[120];
	char strMapName[120];
	ifstream configFile;
	
	configFile.open("config.txt");

	configFile.getline(strTexturePath, 120);
	configFile.getline(strPakFilename, 120);
	configFile.getline(strMapName, 120);
	
	configFile.close();

	g_pLevel = new CHLBsp();
	g_pLevel->Init(g_pD3DDevice, strTexturePath);
	//if(!g_pLevel->LoadFromPak(strPakFilename, strMapName))
	if(!g_pLevel->Load(strMapName))
	{
		MessageBox(NULL, "Couldn't load BSP file", "Error", MB_OK);
		return false;
	}

	cout << *g_pLevel;

	g_pCamera = new CCamera;

	// 1392 -2210 24 angle 90
	//g_pCamera->PositionCamera(1392.0, -2210.0, 24.0,	// position vector
	//camera.SetPosition(60.0, 220.0, -173.0, 0.0);
	g_pCamera->PositionCamera(60.0, 220.0, -173.0,	// position vector
		0.0, 1.0, 0.0,						// direction vector
		0.0, 0.0, 1.0);						// up vector

	// setup RH perspective matrix
	D3DXMATRIX matrix;
	D3DXMatrixPerspectiveFovRH(&matrix, D3DX_PI/4, SCREEN_WIDTH/(float)SCREEN_HEIGHT, 1.0, 10000.0);
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matrix);

	return true;
}

void Shutdown()
{
	if(g_pLevel)
	{
		g_pLevel->Shutdown();
		delete g_pLevel;
		g_pLevel = NULL;
	}

	if(g_pCamera)
	{
		delete g_pCamera;
		g_pCamera = NULL;
	}
	
	if(g_pD3DDevice)
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if(g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}

void Render()
{
	static int start = GetTickCount();
	int end = GetTickCount();
	if(end - start > 100)
	{
		g_pLevel->Update();	
		start = end;
	}
	
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);
	
	if(FAILED(g_pD3DDevice->BeginScene()))
		MessageBox(NULL, "device->BeginScene() failed", "Error", MB_OK);
	
	// update the camera
	g_pCamera->Update(g_pD3DDevice);
	
	// render the level
	g_pLevel->Render(g_pCamera->Position());
	
	// draw wireframe if set
	if(bWireframe)
	{
		g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		g_pD3DDevice->SetRenderState(D3DRS_DEPTHBIAS, 5);
		g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pLevel->Render(g_pCamera->Position());
		g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		g_pD3DDevice->SetRenderState(D3DRS_DEPTHBIAS, 0);
		g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	}

	if(FAILED(g_pD3DDevice->EndScene()))
		MessageBox(NULL, "device->BeginScene() failed", "Error", MB_OK);

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	static bool buttondown = false;
	int xPos, yPos;
	static int anchorx, anchory;
	
	switch(uMsg)
	{
	case WM_ACTIVATE:
		return 0;

	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_LBUTTONDOWN:
		anchorx = LOWORD(lParam);
		anchory = HIWORD(lParam);
		return 0;
	
	case WM_LBUTTONUP:
		return 0;

	case WM_MOUSEMOVE:
		//buttondown = false;
		if(wParam & MK_LBUTTON)
		{
			xPos = LOWORD(lParam);
			yPos = HIWORD(lParam);
			int diffx = anchorx - xPos;
			int diffy = anchory - yPos;
			anchorx = xPos;
			anchory = yPos;
			g_pCamera->Pitch(diffy * 0.1);
			g_pCamera->Yaw(diffx * 0.1);
		}
		return 0;
	
	case WM_KEYDOWN:
		if(wParam == 'W')
			g_pCamera->Move(20.0);
		if(wParam == 'S')
			g_pCamera->Move(-20.0);
		if(wParam == 'A')
			g_pCamera->Strafe(-20.0);
		if(wParam == 'D')
			g_pCamera->Strafe(20.0);
		if(wParam == VK_ESCAPE)
			PostQuitMessage(0);
		if(wParam == 'O')
			g_pLevel->Switch();	
		if(wParam == VK_SPACE)
		{
			bWireframe = !bWireframe;
			if(bWireframe)
				g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			else
				g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
		if(wParam == 'T')
		{
			g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			g_pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		}
		if(wParam == 'L')
		{
			g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
			g_pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		}
		if(wParam == 'M')
		{
			g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			g_pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		}
		return 0;
	
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

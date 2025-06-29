#include "ShowPixelShader8.h"

//------------------------------------------------------------------------------------------
CShowPixelShader8::CShowPixelShader8()
{
	pD3DDevice = nullptr; 
	pImmediateContext = nullptr;
	pNewVertexBuffer = nullptr;
	pPixelShader = nullptr;
	pRenderTargetView = nullptr;
	ZeroMemory(pNewVertices, 6 * sizeof(TVertex8));
	ZeroMemory(m_DefaultVertices, 4 * sizeof(TVertex8));
	DirectX_On = false;
	m_Width = 0;
	m_Height = 0;
	WidthOriginalVideo = 0;
	HeightOriginalVideo = 0;
	m_VertexCount = 0;
	m_VertexStride = 0;
	m_VertexOffset = 0;
	SliderValue = 0.0f;
	alpha = 1.0f;
}
//------------------------------------------------------------------------------------------
CShowPixelShader8::~CShowPixelShader8()
{

}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CShowPixelShader8::OnLoad()
{
	HRESULT hr = S_FALSE;

	DeclareParameterSlider(&SliderValue, ID_SLIDER_1, "Wet/Dry", "W/D", 1.0f);
	
	OnParameter(ID_INIT);
	return S_OK;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CShowPixelShader8::OnGetPluginInfo(TVdjPluginInfo8 *info)
{
	info->Author = "djcel";
	info->PluginName = "ShowPixelShader8";
	info->Description = "Use of pixel shader.";
	info->Flags = 0x00; // VDJFLAG_VIDEO_OUTPUTRESOLUTION | VDJFLAG_VIDEO_OUTPUTASPECTRATIO;
	info->Version = "1.0 (64-bit)";

	return S_OK;
}
//------------------------------------------------------------------------------------------
ULONG VDJ_API CShowPixelShader8::Release()
{
	delete this;
	return 0;
}
//------------------------------------------------------------------------------------------
HRESULT VDJ_API CShowPixelShader8::OnParameter(int id)
{
	if (id == ID_INIT)
	{
		for (int i = ID_SLIDER_1; i <= ID_SLIDER_1; i++) OnSlider(i);
	}

	OnSlider(id);

	return S_OK;
}

//------------------------------------------------------------------------------------------
void CShowPixelShader8::OnSlider(int id)
{
	switch (id)
	{
	case ID_SLIDER_1:
		alpha = SliderValue;
		break;
	}

}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CShowPixelShader8::OnGetParameterString(int id, char* outParam, int outParamSize)
{
	switch (id)
	{
		case ID_SLIDER_1:
			sprintf_s(outParam, outParamSize, "%.0f%%", SliderValue * 100);
			break;
	}

	return S_OK;
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CShowPixelShader8::OnDeviceInit()
{
	HRESULT hr = S_FALSE;

	DirectX_On = true;
	WidthOriginalVideo = 0;
	HeightOriginalVideo = 0;
	m_Width = width;
	m_Height = height;

	hr = GetDevice(VDJVIDEOENGINE, (void**)  &pD3DDevice);
	if(hr!=S_OK || pD3DDevice==NULL) return E_FAIL;

	hr = Initialize_D3D11(pD3DDevice);

	return S_OK;
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CShowPixelShader8::OnDeviceClose()
{
	SAFE_RELEASE(pNewVertexBuffer);
	SAFE_RELEASE(pPixelShader);
	pD3DDevice = nullptr;
	DirectX_On = false;
	
	return S_OK;
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CShowPixelShader8::OnStart() 
{
	return S_OK;
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CShowPixelShader8::OnStop() 
{
	return S_OK;
}
//-------------------------------------------------------------------------------------------
HRESULT VDJ_API CShowPixelShader8::OnDraw()
{
	HRESULT hr = S_FALSE;
	VDJVIDEOTEXTURE *pTexture;
	TVertex8* vertices;

	if (width != m_Width || height != m_Height)
	{
		OnResizeVideo();
	}

	// We get current texture and vertices
	hr = GetTexture(VDJVIDEOENGINE, (void**) &pTexture, &vertices);

	// Copy current vertices
	memset(m_DefaultVertices, 0, 4 * sizeof(TVertex8));
	memcpy(m_DefaultVertices, vertices, 4 * sizeof(TVertex8));

	WidthOriginalVideo = m_DefaultVertices[1].position.x - m_DefaultVertices[0].position.x;
	HeightOriginalVideo = m_DefaultVertices[3].position.y - m_DefaultVertices[0].position.y;

	
	hr = Rendering_D3D11(pD3DDevice, pTexture, vertices);


	return S_OK;
}
//-----------------------------------------------------------------------
void CShowPixelShader8::OnResizeVideo()
{
	HRESULT hr = S_FALSE;

	m_Width = width;
	m_Height = height;
}
//-----------------------------------------------------------------------
HRESULT CShowPixelShader8::Initialize_D3D11(ID3D11Device* pDevice)
{
	HRESULT hr1 = Create_VertexBufferDynamic_D3D11(pDevice);
	HRESULT hr2 = Create_PixelShader_D3D11(pDevice);

	return S_OK;
}
// -----------------------------------------------------------------------
HRESULT CShowPixelShader8::Rendering_D3D11(ID3D11Device* pDevice, ID3D11ShaderResourceView* pTextureView, TVertex8* pVertices)
{
	HRESULT hr = S_FALSE;

	if (!pDevice) return E_FAIL;
	if (!pTextureView) return E_FAIL;
	if (!pNewVertexBuffer)
	{
		hr = Create_VertexBufferDynamic_D3D11(pDevice);
		if (hr != S_OK || !pNewVertexBuffer) return E_FAIL;
	}
	if (!pPixelShader)
	{

		hr = Create_PixelShader_D3D11(pDevice);
		if (hr != S_OK || !pPixelShader) return E_FAIL;
	}

	if (!pImmediateContext)
	{
		pDevice->GetImmediateContext(&pImmediateContext);
		if (!pImmediateContext) return E_FAIL;
	}

	Update_VertexBufferDynamic_D3D11(pImmediateContext);

	//pImmediateContext->OMGetRenderTargets(1, &pRenderTargetView, nullptr);
	//if (!pRenderTargetView) return S_FALSE;
	
	hr = DrawDeck();

	//FLOAT backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//FLOAT backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
	//pImmediateContext->ClearRenderTargetView(pRenderTargetView, backgroundColor);

	//pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

	pImmediateContext->IASetVertexBuffers(0, 1, &pNewVertexBuffer, &m_VertexStride, &m_VertexOffset);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	pImmediateContext->PSSetShader(pPixelShader, nullptr, 0);

	if (pTextureView)
	{
		pImmediateContext->PSSetShaderResources(0, 1, &pTextureView);
	}

	//UINT sampleMask = 0xffffffff;
	//pImmediateContext->OMSetBlendState(nullptr, nullptr, sampleMask);

	pImmediateContext->Draw(m_VertexCount, 0);


	return S_OK;
}
// ---------------------------------------------------------------------- -
HRESULT CShowPixelShader8::Create_VertexBufferDynamic_D3D11(ID3D11Device* pDevice)
{
	HRESULT hr = S_FALSE;

	if (!pDevice) return E_FAIL;

	// Set the number of vertices in the vertex array.
	m_VertexCount = 6; // = ARRAYSIZE(pNewVertices);
	m_VertexStride = sizeof(TLVERTEX);
	m_VertexOffset = 0;

	// Fill in a buffer description.
	D3D11_BUFFER_DESC VertexBufferDesc;
	ZeroMemory(&VertexBufferDesc, sizeof(VertexBufferDesc));
	VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;   // CPU_Access=Write_Only & GPU_Access=Read_Only
	VertexBufferDesc.ByteWidth = sizeof(TLVERTEX) * m_VertexCount;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Use as vertex buffer  // or D3D11_BIND_INDEX_BUFFER
	VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Allow CPU to write in buffer
	VertexBufferDesc.MiscFlags = 0;

	hr = pDevice->CreateBuffer(&VertexBufferDesc, NULL, &pNewVertexBuffer);
	if (hr != S_OK || !pNewVertexBuffer) return S_FALSE;

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CShowPixelShader8::Update_VertexBufferDynamic_D3D11(ID3D11DeviceContext* ctx)
{
	HRESULT hr = S_FALSE;

	if (!ctx) return S_FALSE;
	if (!pNewVertexBuffer) return S_FALSE;

	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));


	hr = ctx->Map(pNewVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	if (hr != S_OK) return S_FALSE;

	hr = Update_Vertices_D3D11();
	//hr = Update_VerticesV2_D3D11();

	memcpy(MappedSubResource.pData, pNewVertices, m_VertexCount * sizeof(TLVERTEX));

	ctx->Unmap(pNewVertexBuffer, NULL);

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CShowPixelShader8::Update_Vertices_D3D11()
{
	float frameWidth = (float) m_Width;
	float frameHeight = (float) m_Height;

	D3DPOSITION P1 = { 0.0f, 0.0f, 0.0f }, // Top Left
		P2 = { 0.0f, frameHeight, 0.0f }, // Bottom Left
		P3 = { frameWidth, 0.0f, 0.0f }, // Top Right
		P4 = { frameWidth, frameHeight, 0.0f }; // Bottom Right
	D3DXCOLOR color_vertex = D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha); // White color with alpha layer
	D3DTEXCOORD T1 = { 0.0f , 0.0f }, T2 = { 0.0f , 1.0f }, T3 = { 1.0f , 0.0f }, T4 = { 1.0f , 1.0f };

	// Triangle n°1 (Bottom Right)
	pNewVertices[0] = { P3 , color_vertex , T3 };
	pNewVertices[1] = { P4 , color_vertex , T4 };
	pNewVertices[2] = { P2 , color_vertex , T2 };

	// Triangle n°2 (Top Left)
	pNewVertices[3] = { P2 , color_vertex , T2 };
	pNewVertices[4] = { P1 , color_vertex , T1 };
	pNewVertices[5] = { P3 , color_vertex , T3 };


	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CShowPixelShader8::Update_VerticesV2_D3D11()
{
	HRESULT hr = S_FALSE;

	int curTextureW = width;
	int curTextureH = height;
	float curTextureAR = 1.0f;

	int srcX = 0, srcY = 0, srcWidth = curTextureW, srcHeight = curTextureH;
	int dstX = 0, dstY = 0, dstWidth = width, dstHeight = height;

	initImageSize(&srcX, &srcY, &srcWidth, &srcHeight, curTextureAR, 0, width, height, &dstX, &dstY, &dstWidth, &dstHeight);

	setVertexDst((float)dstX, (float)dstY, (float)dstWidth, (float)dstHeight);
	setVertexSrc((float)srcX, (float)srcY, (float)srcWidth, (float)srcHeight, (float)curTextureW, (float)curTextureH);

	return S_OK;
}
//-----------------------------------------------------------------------
HRESULT CShowPixelShader8::Create_PixelShader_D3D11(ID3D11Device* pDevice)
{
	HRESULT hr = S_FALSE;
	const WCHAR* pShaderHLSLFilepath = L"PixelShader.hlsl";
	const WCHAR* pShaderCSOFilepath = L"PixelShader.cso";
	const WCHAR* resourceType = RT_RCDATA;
	const WCHAR* resourceName = L"PIXELSHADER.CSO";

	SAFE_RELEASE(pPixelShader);

	hr = Create_PixelShaderFromResourceCSOFile_D3D11(pDevice, resourceType, resourceName);
	//hr = Create_PixelShaderFromCSOFile_D3D11(pDevice, pShaderCSOFilepath);
	//hr = Create_PixelShaderFromHLSLFile_D3D11(pDevice, pShaderHLSLFilepath);
	//hr = Create_PixelShaderFromHeaderFile_D3D11(pDevice);

	return hr;
}
//-----------------------------------------------------------------------
HRESULT CShowPixelShader8::Create_PixelShaderFromCSOFile_D3D11(ID3D11Device* pDevice,pShaderFilepath)
{
	HRESULT hr = S_FALSE;
	ID3DBlob* pPixelShaderBlob = nullptr;

	hr = D3DReadFileToBlob(pShaderFilepath, &pPixelShaderBlob);
	if (hr != S_OK || !pPixelShaderBlob) return S_FALSE;

	LPVOID PixelShaderBytecode = pPixelShaderBlob->GetBufferPointer();
	SIZE_T PixelShaderBytecodeLength = pPixelShaderBlob->GetBufferSize();

	SAFE_RELEASE(pPixelShaderBlob);

	hr = pDevice->CreatePixelShader(PixelShaderBytecode, PixelShaderBytecodeLength, nullptr, &pPixelShader);

	return hr;
}
//-----------------------------------------------------------------------
HRESULT CShowPixelShader8::Create_PixelShaderFromHLSLFile_D3D11(ID3D11Device* pDevice, const WCHAR* pShaderFilepath)
{
	HRESULT hr = S_FALSE;
	ID3DBlob* pPixelShaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	hr = D3DCompileFromFile(pShaderFilepath, nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &pPixelShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		const char* errorString = NULL;
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			errorString = "Could not compile Pixel-Shader. HLSL file not found.";
		}
		else if (errorBlob)
		{
			errorString = (const char*) errorBlob->GetBufferPointer();
			errorBlob->Release();
		}
		MessageBoxA(NULL, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
		return hr;
	}

	LPVOID PixelShaderBytecode = pPixelShaderBlob->GetBufferPointer();
	SIZE_T PixelShaderBytecodeLength = pPixelShaderBlob->GetBufferSize();

	SAFE_RELEASE(pPixelShaderBlob);

	hr = pDevice->CreatePixelShader(PixelShaderBytecode, PixelShaderBytecodeLength, nullptr, &pPixelShader);

	return hr;
}
//-----------------------------------------------------------------------
HRESULT CShowPixelShader8::Create_PixelShaderFromHeaderFile_D3D11(ID3D11Device* pDevice)
{
	HRESULT hr = S_FALSE;
	ID3DBlob* pPixelShaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	const char* PixelShaderSrcData = R"(
		Texture2D g_Texture : register(t0);
		sampler g_TextureSampler : register(s0);

		struct PS_INPUT
		{
			float4 Position : SV_Position;
			float4 Color : COLOR;
			float2 TexCoord : TEXCOORD0;
		};

		struct PS_OUTPUT
		{
			float4 Color : SV_TARGET;
		};

		PS_OUTPUT ps_main(PS_INPUT input)
		{
			PS_OUTPUT output;
			output.Color = g_Texture.Sample(g_TextureSampler, input.TexCoord);
			output.Color = output.Color * input.Color;
			return output;
		}

		)";

	SIZE_T PixelShaderSrcDataSize = strlen(PixelShaderSrcData);

	hr = D3DCompile(PixelShaderSrcData, PixelShaderSrcDataSize, nullptr, nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &pPixelShaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		const char* errorString = NULL;
		if (errorBlob) 
		{
			errorString = (const char*) errorBlob->GetBufferPointer();
			errorBlob->Release();
		}
		MessageBoxA(NULL, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
		return hr;
	}

	LPVOID PixelShaderBytecode = pPixelShaderBlob->GetBufferPointer();
	SIZE_T PixelShaderBytecodeLength = pPixelShaderBlob->GetBufferSize();
	
	SAFE_RELEASE(pPixelShaderBlob);

	hr = pDevice->CreatePixelShader(PixelShaderBytecode, PixelShaderBytecodeLength, nullptr, &pPixelShader);
	
	return hr;
}
//-----------------------------------------------------------------------
HRESULT CShowPixelShader8::Create_PixelShaderFromResourceCSOFile_D3D11(ID3D11Device* pDevice, const WCHAR* resourceType, const WCHAR* resourceName)
{
	HRESULT hr = S_FALSE;
	ID3DBlob* pPixelShaderBlob = nullptr;
	
	hr = D3DReadResourceToBlob(resourceType, resourceName, &pPixelShaderBlob);
	if (hr != S_OK || !pPixelShaderBlob) return S_FALSE;
	
	LPVOID PixelShaderBytecode = pPixelShaderBlob->GetBufferPointer();
	SIZE_T PixelShaderBytecodeLength = pPixelShaderBlob->GetBufferSize();
	
	SAFE_RELEASE(pPixelShaderBlob);
	
	hr = pDevice->CreatePixelShader(PixelShaderBytecode, PixelShaderBytecodeLength, nullptr, &pPixelShader);

	return hr;
}
//-----------------------------------------------------------------------
HRESULT CShowPixelShader8::D3DReadResourceToBlob(const WCHAR* resourceType, const WCHAR* resourceName, ID3DBlob** ppContents)
{
	HRESULT hr = S_FALSE;

	std::string_view ShaderData = getResource(resourceType, resourceName);

	const char* ShaderBytecode = ShaderData.data();
	SIZE_T ShaderBytecodeLength = ShaderData.length();

	hr = D3DCreateBlob(ShaderBytecodeLength, ppContents);
	if (hr != S_OK || !*ppContents)
	{
		return hr;
	}

	memcpy((*ppContents)->GetBufferPointer(), ShaderBytecode, ShaderBytecodeLength);

	return S_OK;
}
//-----------------------------------------------------------------------
std::string_view CShowPixelShader8::getResource(const WCHAR* resourceType, const WCHAR* resourceName)
{
	HRSRC rc = FindResource(hInstance, resourceName, resourceType);
	if (!rc)
		return std::string_view("");

	HGLOBAL rcData = LoadResource(hInstance, rc);
	if (!rcData)
		return std::string_view("");

	DWORD size = SizeofResource(hInstance, rc);

	char* data = (char*)LockResource(rcData);
	if (!data)
		return std::string_view("");

	return std::string_view(data, size);
}
//-----------------------------------------------------------------------
void CShowPixelShader8::initImageSize(int* srcX, int* srcY, int* srcWidth, int* srcHeight, float srcAr, int srcOrientation, int width, int height, int* dstX, int* dstY, int* dstWidth, int* dstHeight)
{
	int letterBoxing = 0;
	*dstWidth = width;
	*dstHeight = height;
	*dstX = 0;
	*dstY = 0;

	if (letterBoxing == 2) //zoom (stretch)
		return;

	if (srcWidth == 0 || srcHeight == 0)
		return;

	if (srcOrientation >= 6 && srcOrientation <= 8)
	{
		*srcWidth = *srcHeight;
		*srcHeight = *srcWidth;
	}

	if (letterBoxing == 0) //borders (scale)
	{
		*dstWidth = (int)(height * *srcWidth * srcAr / *srcHeight);
		*dstHeight = height;
		if (*dstWidth > width)
		{
			*dstWidth = width;
			*dstHeight = (int)(width * *srcHeight / (*srcWidth * srcAr));
		}
		*dstX = (width - *dstWidth) / 2;
		*dstY = (height - *dstHeight) / 2;
	}
	else if (letterBoxing == 1) //crop
	{
		*dstWidth = (int)(height * *srcWidth * srcAr / *srcHeight);
		*dstHeight = height;
		if (*dstWidth < width)
		{
			*dstWidth = width;
			*dstHeight = (int)(width * *srcHeight / (*srcWidth * srcAr));
		}
		*dstX = (width - *dstWidth) / 2;
		*dstY = (height - *dstHeight) / 2;
		//TODO: Is this correct in combination with rotation?
		if (*dstX < 0)
		{
			*srcX = -(*dstX * *srcWidth / *dstWidth);
			*srcWidth -= 2 * *srcX;
			*dstWidth -= *dstX * 2;
			*dstX = 0;
		}
		if (*dstY < 0)
		{
			*srcY = -(*dstY * *srcHeight / *dstHeight);
			*srcHeight -= 2 * *srcY;
			*dstHeight -= *dstY * 2;
			*dstY = 0;
		}
	}
}

//-----------------------------------------------------------------------
void CShowPixelShader8::setVertexDst(float dstX, float dstY, float width, float height)
{
	const D3DXCOLOR color = D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha); // white color with alpha

	pNewVertices[0].position.x = dstX + width;
	pNewVertices[0].position.y = dstY;
	pNewVertices[0].position.z = 0.0f;
	pNewVertices[0].color = color;

	pNewVertices[1].position.x = dstX + width;
	pNewVertices[1].position.y = dstY + height;
	pNewVertices[1].position.z = 0.0f;
	pNewVertices[1].color = color;

	pNewVertices[2].position.x = dstX;
	pNewVertices[2].position.y = dstY + height;
	pNewVertices[2].position.z = 0.0f;
	pNewVertices[2].color = color;

	pNewVertices[3].position.x = dstX;
	pNewVertices[3].position.y = dstY + height;
	pNewVertices[3].position.z = 0.0f;
	pNewVertices[3].color = color;

	pNewVertices[4].position.x = dstX;
	pNewVertices[4].position.y = dstY;
	pNewVertices[4].position.z = 0.0f;
	pNewVertices[4].color = color;

	pNewVertices[5].position.x = dstX + width;
	pNewVertices[5].position.y = dstY;
	pNewVertices[5].position.z = 0.0f;
	pNewVertices[5].color = color;
}
//-----------------------------------------------------------------------
void CShowPixelShader8::setVertexSrc(float srcX, float srcY, float srcWidth, float srcHeight, float textureWidth, float textureHeight)
{
	pNewVertices[0].texture.tu = (srcX + srcWidth) / textureWidth;
	pNewVertices[0].texture.tv = srcY / textureHeight;

	pNewVertices[1].texture.tu = (srcX + srcWidth) / textureWidth;
	pNewVertices[1].texture.tv = (srcY + srcHeight) / textureHeight;

	pNewVertices[2].texture.tu = srcX / textureWidth;
	pNewVertices[2].texture.tv = (srcY + srcHeight) / textureHeight;

	pNewVertices[3].texture.tu = srcX / textureWidth;
	pNewVertices[3].texture.tv = (srcY + srcHeight) / textureHeight;

	pNewVertices[4].texture.tu = srcX / textureWidth;
	pNewVertices[4].texture.tv = srcY / textureHeight;

	pNewVertices[5].texture.tu = (srcX + srcWidth) / textureWidth;
	pNewVertices[5].texture.tv = srcY / textureHeight;
}

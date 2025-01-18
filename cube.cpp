#include "cube.h"

Cube::Cube(ID3D11Device *dxdevice, ID3D11DeviceContext *dxdevice_context)
	: Model(dxdevice, dxdevice_context)
{
	Init();
	CreateCubeBuffers(dxdevice);

}

void Cube::Init()
{
	//A -0.5,-0.5,0
	//B 0.5, -0.5, 0
	//C 0.5,0.5,0
	//D -0.5, 0.5, 0
	//E 0.5,0.5,-1.0
	//F 0.5,-0.5,-1.0
	//G -0.5,-0.5,-1.0
	//H -0.5,0.5,-1.0

	//Front Face ABCD   TRIS ABC CDA   0,1,2,2,3,0
	//Left Face BCEF    TRIS BFE ECB   4,5,6,6,7,4
	//Right Face ADGH   TRIS ADH HGA   8,9,10,10,11,8
	//Back Face EFGH    TRIS EFG GHE   12,13,14,14,15,12
	//Top Face CDEH     TRIS HDC CEH   16,17,18,18,19,16
	//Bottom Face ABGF  TRIS GFB BAG   20,21,22,22,23,20

#pragma region Front Vertices
	cube_vertices[0].Position = {-0.5f, -0.5f, 0.0f};
	cube_vertices[0].Normal = {0, 0, 1};
	cube_vertices[0].TexCoord = {0, 0};
	cube_vertices[1].Position = {0.5f, -0.5f, 0.0f};
	cube_vertices[1].Normal = {0, 0, 1};
	cube_vertices[1].TexCoord = {0, 1};
	cube_vertices[2].Position = {0.5f, 0.5f, 0.0f};
	cube_vertices[2].Normal = {0, 0, 1};
	cube_vertices[2].TexCoord = {1, 1};
	cube_vertices[3].Position = {-0.5f, 0.5f, 0.0f};
	cube_vertices[3].Normal = {0, 0, 1};
	cube_vertices[3].TexCoord = {1, 0};
#pragma endregion
#pragma region Left Vertices
	cube_vertices[4].Position = {0.5f, 0.5f, 0.0f};
	cube_vertices[4].Normal = {1, 0, 0};
	cube_vertices[4].TexCoord = {0, 0};
	cube_vertices[5].Position = {0.5f, -0.5f, 0.0f};
	cube_vertices[5].Normal = {1, 0, 0};
	cube_vertices[5].TexCoord = {0, 1};
	cube_vertices[6].Position = {0.5f, -0.5f, -1.0f};
	cube_vertices[6].Normal = {1, 0, 0};
	cube_vertices[6].TexCoord = {1, 1};
	cube_vertices[7].Position = {0.5f, 0.5f, -1.0f};
	cube_vertices[7].Normal = {1, 0, 0};
	cube_vertices[7].TexCoord = {1, 0};
#pragma endregion
#pragma region Right Vertices
	cube_vertices[8].Position = {-0.5f, -0.5f, 0.0f};
	cube_vertices[8].Normal = {1, 0, 0};
	cube_vertices[8].TexCoord = {0, 0};
	cube_vertices[9].Position = {-0.5f, 0.5f, 0.0f};
	cube_vertices[9].Normal = {1, 0, 0};
	cube_vertices[9].TexCoord = {0, 1};
	cube_vertices[10].Position = {-0.5f, 0.5f, -1.0f};
	cube_vertices[10].Normal = {1, 0, 0};
	cube_vertices[10].TexCoord = {1, 1};
	cube_vertices[11].Position = {-0.5f, -0.5f, -1.0f};
	cube_vertices[11].Normal = {1, 0, 0};
	cube_vertices[11].TexCoord = {1, 0};
#pragma endregion
#pragma region Back Vertices
	cube_vertices[12].Position = {0.5f, 0.5f, -1.0f};
	cube_vertices[12].Normal = {0, 0, 1};
	cube_vertices[12].TexCoord = {0, 0};
	cube_vertices[13].Position = {0.5f, -0.5f, -1.0f};
	cube_vertices[13].Normal = {0, 0, 1};
	cube_vertices[13].TexCoord = {0, 1};
	cube_vertices[14].Position = {-0.5f, -0.5f, -1.0f};
	cube_vertices[14].Normal = {0, 0, 1};
	cube_vertices[14].TexCoord = {1, 1};
	cube_vertices[15].Position = {-0.5f, 0.5f, -1.0f};
	cube_vertices[15].Normal = {0, 0, 1};
	cube_vertices[15].TexCoord = {1, 0};
#pragma endregion
#pragma region Top Vertices
	cube_vertices[16].Position = {-0.5f, 0.5f, -1.0f};
	cube_vertices[16].Normal = {0, 1, 0};
	cube_vertices[16].TexCoord = {0, 0};
	cube_vertices[17].Position = {-0.5f, 0.5f, 0.0f};
	cube_vertices[17].Normal = {0, 1, 0};
	cube_vertices[17].TexCoord = {0, 1};
	cube_vertices[18].Position = {0.5f, 0.5f, 0.0f};
	cube_vertices[18].Normal = {0, 1, 0};
	cube_vertices[18].TexCoord = {1, 1};
	cube_vertices[19].Position = {0.5f, 0.5f, -1.0f};
	cube_vertices[19].Normal = {0, 1, 0};
	cube_vertices[19].TexCoord = {1, 0};
#pragma endregion
#pragma region Bottom Vertices
	cube_vertices[20].Position = {-0.5f, -0.5f, -1.0f};
	cube_vertices[20].Normal = {0, 1, 0};
	cube_vertices[20].TexCoord = {0, 0};
	cube_vertices[21].Position = {0.5f, -0.5f, -1.0f};
	cube_vertices[21].Normal = {0, 1, 0};
	cube_vertices[21].TexCoord = {0, 1};
	cube_vertices[22].Position = {0.5f, -0.5f, 0.0f};
	cube_vertices[22].Normal = {0, 1, 0};
	cube_vertices[22].TexCoord = {1, 1};
	cube_vertices[23].Position = {-0.5f, -0.5f, 0.0f};
	cube_vertices[23].Normal = {0, 1, 0};
	cube_vertices[23].TexCoord = {1, 0};
#pragma endregion

	cube_indices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

}

void Cube::CreateCubeBuffers(ID3D11Device *dxdevice)
{
	// Vertex array descriptor
	D3D11_BUFFER_DESC vertexbufferDesc{0};
	vertexbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferDesc.CPUAccessFlags = 0;
	vertexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferDesc.MiscFlags = 0;
	vertexbufferDesc.ByteWidth = (UINT)(cube_vertices.size() * sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vertexData = {0};
	vertexData.pSysMem = &cube_vertices[0];
	// Create vertex buffer on device using descriptor & data
	dxdevice->CreateBuffer(&vertexbufferDesc, &vertexData, &m_vertex_buffer);
	SETNAME(m_vertex_buffer, "VertexBuffer");

	//  Index array descriptor
	D3D11_BUFFER_DESC indexbufferDesc = {0};
	indexbufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferDesc.CPUAccessFlags = 0;
	indexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferDesc.MiscFlags = 0;
	indexbufferDesc.ByteWidth = (UINT)(cube_indices.size() * sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA indexData{0};
	indexData.pSysMem = &cube_indices[0];
	// Create index buffer on device using descriptor & data
	dxdevice->CreateBuffer(&indexbufferDesc, &indexData, &m_index_buffer);
	SETNAME(m_index_buffer, "IndexBuffer");

	m_number_of_indices = (unsigned int)cube_indices.size();
}

void Cube::Render() const
{
	const UINT32 stride = sizeof(Vertex); //  sizeof(float) * 8;
	const UINT32 offset = 0;
	m_dxdevice_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	// Bind our index buffer
	m_dxdevice_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Make the drawcall
	m_dxdevice_context->DrawIndexed(m_number_of_indices, 0, 0);
}

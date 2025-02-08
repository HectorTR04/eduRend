#include "OBJModel.h"
#include <buffers.h>

OBJModel::OBJModel(
	const std::string& objfile,
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context)
	: Model(dxdevice, dxdevice_context)
{
	// Load the OBJ
	OBJLoader* mesh = new OBJLoader();
	mesh->Load(objfile);

	// Load and organize indices in ranges per drawcall (material)

	std::vector<unsigned> indices;
	unsigned int indexOffset = 0;

	for (auto& dc : mesh->Drawcalls)
	{
		// Append the drawcall indices
		for (auto& tri : dc.Triangles)
			indices.insert(indices.end(), tri.VertexIndices, tri.VertexIndices + 3);

		// Create a range
		unsigned int indexSize = (unsigned int)dc.Triangles.size() * 3;
		int materialIndex = dc.MaterialIndex > -1 ? dc.MaterialIndex : -1;
		m_index_ranges.push_back({ indexOffset, indexSize, 0, materialIndex });

		indexOffset = (unsigned int)indices.size();
	}

	for (int i = 0; i < indices.size(); i += 3) 
		ComputeTB(mesh->Vertices[indices[i + 0]], 
			mesh->Vertices[indices[i + 1]], 
			mesh->Vertices[indices[i + 2]]);

	// Vertex array descriptor
	D3D11_BUFFER_DESC vertexbufferDesc = { 0 };
	vertexbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferDesc.CPUAccessFlags = 0;
	vertexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferDesc.MiscFlags = 0;
	vertexbufferDesc.ByteWidth = (UINT)(mesh->Vertices.size() * sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = &(mesh->Vertices)[0];
	// Create vertex buffer on device using descriptor & data
	dxdevice->CreateBuffer(&vertexbufferDesc, &vertexData, &m_vertex_buffer);
	SETNAME(m_vertex_buffer, "VertexBuffer");

	// Index array descriptor
	D3D11_BUFFER_DESC indexbufferDesc = { 0 };
	indexbufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferDesc.CPUAccessFlags = 0;
	indexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferDesc.MiscFlags = 0;
	indexbufferDesc.ByteWidth = (UINT)(indices.size() * sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	dxdevice->CreateBuffer(&indexbufferDesc, &indexData, &m_index_buffer);
	SETNAME(m_index_buffer, "IndexBuffer");

	// Copy materials from mesh
	append_materials(mesh->Materials);

	// Go through materials and load textures (if any) to device
	std::cout << "Loading textures..." << std::endl;
	for (auto& material : m_materials)
	{
		HRESULT hr;

		// Load Diffuse texture
		//
		if (material.DiffuseTextureFilename.size()) {

			hr = LoadTextureFromFile(
				dxdevice,
				material.DiffuseTextureFilename.c_str(),
				&material.DiffuseTexture);
			std::cout << "\t" << material.DiffuseTextureFilename
				<< (SUCCEEDED(hr) ? " - OK" : "- FAILED") << std::endl;
		}

		// + other texture types here - see Material class
		if (material.NormalTextureFilename.size()) {

			hr = LoadTextureFromFile(
				dxdevice,
				material.NormalTextureFilename.c_str(),
				&material.NormalTexture);
			std::cout << "\t" << material.NormalTextureFilename
				<< (SUCCEEDED(hr) ? " - OK" : "- FAILED") << std::endl;
		}
	}
	std::cout << "Done." << std::endl;

	InitMaterialBuffer();

	SAFE_DELETE(mesh);
}

void OBJModel::InitMaterialBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixBufferDesc = {0};
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MaterialBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_material_buffer));
}

void OBJModel::UpdateMaterialBuffer(Material material) const
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_material_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	MaterialBuffer *material_buffer = (MaterialBuffer*)resource.pData;
	material_buffer->ambient = float4(material.AmbientColour.x, material.AmbientColour.y, material.AmbientColour.z, 1.0f);
	material_buffer->diffuse = float4(material.DiffuseColour.x, material.DiffuseColour.y, material.DiffuseColour.z, 1.0f);
	material_buffer->specular = float4(material.SpecularColour.x, material.SpecularColour.y, material.SpecularColour.z, 1.0f);
	m_dxdevice_context->Unmap(m_material_buffer, 0);
}

void OBJModel::ComputeTB(Vertex &v0, Vertex &v1, Vertex &v2)
{
	vec3f tangent, binormal;

	vec3f D = v1.Position - v0.Position;
	vec3f E = v2.Position - v0.Position;
	vec2f F = v1.TexCoord - v0.TexCoord;
	vec2f G = v2.TexCoord - v0.TexCoord;

	//GF is a 2x2 and DE is a 2x3 matrix both written as 3x3 to allow multiplication
	mat3f GF = {G.y, -F.y, 0, -G.x, F.x, 0, 0, 0, 0};
	mat3f DE = {D.x, D.y, D.z, E.x, E.y, E.z, 0, 0, 0};
	
	float determinant = 1 / ((F.x * G.y) - (F.y * G.x));
	mat3f TB = GF * DE;

	TB.m11 *= determinant;
	TB.m12 *= determinant;
	TB.m13 *= determinant;
	TB.m21 *= determinant;
	TB.m22 *= determinant;
	TB.m23 *= determinant;

	tangent.x = TB.m11;
	tangent.y = TB.m12;
	tangent.z = TB.m13;
	binormal.x = TB.m21;
	binormal.y = TB.m22;
	binormal.z = TB.m23;

	// Now assign the newly computed vectors to the vertices
	v0.Tangent = v1.Tangent = v2.Tangent = normalize(tangent);
	v0.Binormal = v1.Binormal = v2.Binormal = normalize(binormal);
}

void OBJModel::Render() const
{
	// Bind vertex buffer
	const UINT32 stride = sizeof(Vertex);
	const UINT32 offset = 0;
	m_dxdevice_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	// Bind index buffer
	m_dxdevice_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//Bind material buffer
	m_dxdevice_context->PSSetConstantBuffers(1, 1, &m_material_buffer);

	// Iterate Drawcalls
	for (auto& indexRange : m_index_ranges)
	{
		// Fetch material
		const Material& material = m_materials[indexRange.MaterialIndex];

		UpdateMaterialBuffer(material);

		// Bind diffuse texture to slot t0 of the PS
		m_dxdevice_context->PSSetShaderResources(0, 1, &material.DiffuseTexture.TextureView);
		// + bind other textures here, e.g. a normal map, to appropriate slots
		m_dxdevice_context->PSSetShaderResources(1, 1, &material.NormalTexture.TextureView);

		// Make the drawcall
		m_dxdevice_context->DrawIndexed(indexRange.Size, indexRange.Start, 0);
	}
}

OBJModel::~OBJModel()
{
	for (auto& material : m_materials)
	{
		SAFE_RELEASE(material.DiffuseTexture.TextureView);
		SAFE_RELEASE(material.NormalTexture.TextureView);
		// Release other used textures ...
	}
}
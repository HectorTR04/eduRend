#pragma once
#include "Model.h"

class Cube : public Model
{
	unsigned m_number_of_indices = 0;

public:
	Cube(ID3D11Device *dxdevice, ID3D11DeviceContext *dxdevice_context);

	void Init();
	void CreateCubeBuffers(ID3D11Device *dxdevice);

	void InitMaterialBuffer();

	void UpdateMaterialBuffer(Material material) const;

	virtual void Render() const;

	~Cube() {};

private:
	std::vector<unsigned> cube_indices;
	std::vector<Vertex> cube_vertices{24};
	Material material;
};


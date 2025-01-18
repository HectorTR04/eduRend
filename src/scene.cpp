#include "Scene.h"
#include "QuadModel.h"
#include "OBJModel.h"
#include "cube.h"

Scene::Scene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	m_dxdevice(dxdevice),
	m_dxdevice_context(dxdevice_context),
	m_window_width(window_width),
	m_window_height(window_height)
{ }

void Scene::OnWindowResized(
	int new_width,
	int new_height)
{
	m_window_width = new_width;
	m_window_height = new_height;
}

OurTestScene::OurTestScene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	Scene(dxdevice, dxdevice_context, window_width, window_height)
{ 
	InitTransformationBuffer();
	// + init other CBuffers
}

//
// Called once at initialization
//
void OurTestScene::Init()
{
	m_camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)m_window_width / m_window_height,	// aspect ratio
		1.0f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)

	// Move camera to (0,0,5)
	m_camera->MoveTo({ 0, 0, 5 });

	// Create objects
	//m_quad = new QuadModel(m_dxdevice, m_dxdevice_context);
	//m_sponza = new OBJModel("assets/crytek-sponza/sponza.obj", m_dxdevice, m_dxdevice_context);
	m_homestead = new OBJModel("assets/lars-homestead/larshomestead.obj", m_dxdevice, m_dxdevice_context);
	m_cube = new Cube(m_dxdevice, m_dxdevice_context);
	m_cube2 = new Cube(m_dxdevice, m_dxdevice_context);
	m_cube3 = new Cube(m_dxdevice, m_dxdevice_context);
}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(
	float dt,
	const InputHandler& input_handler)
{
	// Basic camera control
	if (input_handler.IsKeyPressed(Keys::Up) || input_handler.IsKeyPressed(Keys::W))
		m_camera->Move({ 0.0f, 0.0f, -m_camera_velocity * dt });
	if (input_handler.IsKeyPressed(Keys::Down) || input_handler.IsKeyPressed(Keys::S))
		m_camera->Move({ 0.0f, 0.0f, m_camera_velocity * dt });
	if (input_handler.IsKeyPressed(Keys::Right) || input_handler.IsKeyPressed(Keys::D))
		m_camera->Move({ m_camera_velocity * dt, 0.0f, 0.0f });
	if (input_handler.IsKeyPressed(Keys::Left) || input_handler.IsKeyPressed(Keys::A))
		m_camera->Move({ -m_camera_velocity * dt, 0.0f, 0.0f });

	//Camera Rotation
	float mousedx = input_handler.GetMouseDeltaX();
	float mousedy = input_handler.GetMouseDeltaY();
	m_camera->Rotate(0, mousedx, mousedy);


	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

#pragma region Default Object Transforms
	// Quad model-to-world transformation
	m_quad_transform = mat4f::translation(0, 0, 0) *			// No translation
		mat4f::rotation(-m_angle_y, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	// Sponza model-to-world transformation
	m_sponza_transform = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(0.05f);						 // The scene is quite large so scale it down to 5%
#pragma endregion

	m_cube_transform = mat4f::translation(0, 0, 0) *
		mat4f::rotation(-m_angle_x, 1.0f, 0.0f, 0.0f) * // Rotate around X-axis
		mat4f::rotation(-m_angle_y, 0.0f, 1.0f, 0.0f) * // Rotate around Y-axis
		mat4f::scaling(0.5, 0.5, 0.5);

	m_homestead_transform = mat4f::translation(0, -5, 0) *
		mat4f::rotation(0, 0.0f, 0.0f, 0.0f) * 
		mat4f::scaling(0.1);

	// Increment the rotation angle.
	m_angle_x += m_angular_velocity * dt;
	m_angle_y += m_angular_velocity * dt;

	// Print fps
	m_fps_cooldown -= dt;
	if (m_fps_cooldown < 0.0)
	{
		std::cout << "fps " << (int)(1.0f / dt) << std::endl;
//		printf("fps %i\n", (int)(1.0f / dt));
		m_fps_cooldown = 2.0;
	}
}

//
// Called every frame, after update
//
void OurTestScene::Render()
{
	// Bind transformation_buffer to slot b0 of the VS
	m_dxdevice_context->VSSetConstantBuffers(0, 1, &m_transformation_buffer);

	// Obtain the matrices needed for rendering from the camera
	m_view_matrix = m_camera->WorldToViewMatrix();
	m_projection_matrix = m_camera->ProjectionMatrix();

	// Load matrices + the Quad's transformation to the device and render it
	//UpdateTransformationBuffer(m_quad_transform, m_view_matrix, m_projection_matrix);
	//m_quad->Render();

	// Load matrices + Sponza's transformation to the device and render it
	//UpdateTransformationBuffer(m_sponza_transform, m_view_matrix, m_projection_matrix);
	//m_sponza->Render();

	/*UpdateTransformationBuffer(m_cube_transform, m_view_matrix, m_projection_matrix);
	m_cube->Render();*/

	UpdateTransformationBuffer(m_homestead_transform, m_view_matrix, m_projection_matrix);
	m_homestead->Render();


	
}

void OurTestScene::Release()
{
	SAFE_DELETE(m_quad);
	SAFE_DELETE(m_sponza);
	SAFE_DELETE(m_camera);
	SAFE_DELETE(m_cube);

	SAFE_RELEASE(m_transformation_buffer);
	// + release other CBuffers
}

void OurTestScene::OnWindowResized(
	int new_width,
	int new_height)
{
	if (m_camera)
		m_camera->SetAspect(float(new_width) / new_height);

	Scene::OnWindowResized(new_width, new_height);
}

void OurTestScene::InitTransformationBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(TransformationBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_transformation_buffer));
}

void OurTestScene::UpdateTransformationBuffer(
	mat4f ModelToWorldMatrix,
	mat4f WorldToViewMatrix,
	mat4f ProjectionMatrix)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformationBuffer* matrixBuffer = (TransformationBuffer*)resource.pData;
	matrixBuffer->ModelToWorldMatrix = ModelToWorldMatrix;
	matrixBuffer->WorldToViewMatrix = WorldToViewMatrix;
	matrixBuffer->ProjectionMatrix = ProjectionMatrix;
	m_dxdevice_context->Unmap(m_transformation_buffer, 0);
}
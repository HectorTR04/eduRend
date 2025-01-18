#include "Camera.h"

using namespace linalg;

void Camera::MoveTo(const vec3f& position) noexcept
{
	m_position = position;
}

void Camera::Move(const vec3f& direction) noexcept
{
	m_position += direction;
}

void Camera::Rotate(const float &roll, const float &yaw, const float &pitch) noexcept
{	
	m_rotation.z += roll / camera_sensitivity;
	m_rotation.y += yaw / camera_sensitivity;
	m_rotation.x += pitch / camera_sensitivity;
	rotation_matrix = mat4f::rotation(-m_rotation.z, -m_rotation.y, -m_rotation.x);
}

mat4f Camera::WorldToViewMatrix() const noexcept
{
	// Assuming a camera's position and rotation is defined by matrices T(p) and R,
	// the View-to-World transform is T(p)*R (for a first-person style camera).
	//
	// World-to-View then is the inverse of T(p)*R;
	//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
	// Since now there is no rotation, this matrix is simply T(-p)

	return transpose(rotation_matrix) * mat4f::translation(-m_position);
}

mat4f Camera::ProjectionMatrix() const noexcept
{
	return mat4f::projection(m_vertical_fov, m_aspect_ratio, m_near_plane, m_far_plane);
}
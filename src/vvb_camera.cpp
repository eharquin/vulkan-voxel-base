#include "vvb_camera.hpp"

// std
#include <iostream>
#include <cassert>
#include <limits>

// Set the orthographic projection matrix of the camera
//
// The projection matrix is a 4x4 matrix that determines how 3D points are
// projected onto the 2D image plane. In this case, we are setting an
// orthographic projection, which means that all objects will appear the same
// size regardless of their distance from the camera.
//
// The input parameters define the dimensions of the viewing volume:
//   left:   left edge of the viewing volume
//   right:  right edge of the viewing volume
//   top:    top edge of the viewing volume
//   bottom: bottom edge of the viewing volume
//   near:   distance to the near plane of the viewing volume
//   far:    distance to the far plane of the viewing volume
//
// The resulting projection matrix has the following form:
//
//   2/(r-l)    0          0           -(r+l)/(r-l)
//   0          2/(t-b)    0           -(t+b)/(t-b)
//   0          0          1/(f-n)     -n/(f-n)
//   0          0          0           1
//
// where (l, t, n) and (r, b, f) are the left, top, and near coordinates and
// the right, bottom, and far coordinates of the viewing volume, respectively.
void VvbCamera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far)
{
	proj = glm::mat4{ 1.0f };
	proj[0][0] = 2.f / (right - left);
	proj[1][1] = 2.f / (bottom - top);
	proj[2][2] = 1.f / (far - near);
	proj[3][0] = -(right + left) / (right - left);
	proj[3][1] = -(bottom + top) / (bottom - top);
	proj[3][2] = -near / (far - near);
}

// Set the perspective projection matrix of the camera
//
// The projection matrix is a 4x4 matrix that determines how 3D points are
// projected onto the 2D image plane. In this case, we are setting a
// perspective projection, which means that objects farther from the camera
// will appear smaller than objects close to the camera.
//
// The input parameters define the shape of the viewing frustum:
//   fovy:   field of view angle (in radians) in the y direction
//   aspect: aspect ratio (width / height) of the viewport
//   near:   distance to the near plane of the viewing frustum
//   far:    distance to the far plane of the viewing frustum
//
// The resulting projection matrix has the following form:
//
//   1/(a*t)   0          0                   0
//   0          1/t        0                   0
//   0          0          f/(f-n)             1
//   0          0          -(f*n)/(f-n)        0
//
// where (f, n) and (t) are the far distance, near distance, and tangent of
// half the field of view angle in the y direction, respectively. The aspect
// ratio (a) is the width of the viewport divided by the height.
void VvbCamera::setPerspectiveProjection(float fovy, float aspect, float near, float far)
{
	assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
	const float tanHalfFovy = tan(fovy / 2.f);
	proj = glm::mat4{ 0.0f };
	proj[0][0] = 1.f / (aspect * tanHalfFovy);
	proj[1][1] = 1.f / (tanHalfFovy);
	proj[2][2] = far / (far - near);
	proj[2][3] = 1.f;
	proj[3][2] = -(far * near) / (far - near);
}

// Set the view matrix of the camera
//
// The view matrix is a 4x4 matrix that determines the position and orientation
// of the camera in world space.
//
// The input parameters define the position and orientation of the camera:
//   position: position of the camera in world space
//   direction: direction that the camera is facing (should be normalized)
//   up:       up direction of the camera (should be normalized and perpendicular to direction)
//
// The resulting view matrix has the following form:
//
//   ux  vx  wx  -dot(u,position)
//   uy  vy  wy  -dot(v,position)
//   uz  vz  wz  -dot(w,position)
//   0   0   0   1
//
// where (u, v, w) are the local x, y, and z axes of the camera, respectively,
// and (position) is the position of the camera in world space. The local x axis
// is determined by the cross product of the up direction and the direction that
// the camera is facing, and the local y and z axes are determined by the cross
// product of the direction that the camera is facing and the local x axis.
void VvbCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
	const glm::vec3 w{ glm::normalize(direction) };
	const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
	const glm::vec3 v{ glm::cross(w, u) };

	view = glm::mat4{ 1.f };
	view[0][0] = u.x;
	view[1][0] = u.y;
	view[2][0] = u.z;
	view[0][1] = v.x;
	view[1][1] = v.y;
	view[2][1] = v.z;
	view[0][2] = w.x;
	view[1][2] = w.y;
	view[2][2] = w.z;
	view[3][0] = -glm::dot(u, position);
	view[3][1] = -glm::dot(v, position);
	view[3][2] = -glm::dot(w, position);
}

// Set the view matrix of the camera using a target position
//
// This function is a convenience function that sets the view matrix of the
// camera using a target position rather than a direction. The target position
// is used to compute the direction that the camera is facing, which is then
// passed to the setViewDirection function along with the camera position and
// up direction.
//
// The input parameters are the same as for the setViewDirection function,
// except that the direction parameter is replaced by a target position:
//   position: position of the camera in world space
//   target:   target position in world space
//   up:       up direction of the camera (should be normalized and perpendicular to direction)
void VvbCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
	setViewDirection(position, target - position, up);
}

// Set the view matrix of the camera using yaw, pitch, and roll angles
//
// This function is a convenience function that sets the view matrix of the
// camera using yaw, pitch, and roll angles to specify the orientation of the
// camera in world space. The yaw angle specifies the rotation around the y
// axis, the pitch angle specifies the rotation around the x axis, and the
// roll angle specifies the rotation around the z axis.
//
// The input parameters are the camera position and yaw, pitch, and roll angles:
//   position: position of the camera in world space
//   rotation: yaw, pitch, and roll angles (in radians)
//
// The resulting view matrix has the following form:
//
//   ux  vx  wx  -dot(u,position)
//   uy  vy  wy  -dot(v,position)
//   uz  vz  wz  -dot(w,position)
//   0   0   0   1
//
// where (u, v, w) are the local x, y, and z axes of the camera, respectively,
// and (position) is the position of the camera in world space. The local x axis
// is determined by the yaw, pitch, and roll angles, and the local y and z axes
// are determined by the cross product of the local x axis and the direction that
// the camera is facing (which is the opposite of the local z axis).
void VvbCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation)
{
	const float c3 = glm::cos(rotation.z);
	const float s3 = glm::sin(rotation.z);
	const float c2 = glm::cos(rotation.x);
	const float s2 = glm::sin(rotation.x);
	const float c1 = glm::cos(rotation.y);
	const float s1 = glm::sin(rotation.y);
	const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
	const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
	const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
	view = glm::mat4{ 1.f };
	view[0][0] = u.x;
	view[1][0] = u.y;
	view[2][0] = u.z;
	view[0][1] = v.x;
	view[1][1] = v.y;
	view[2][1] = v.z;
	view[0][2] = w.x;
	view[1][2] = w.y;
	view[2][2] = w.z;
	view[3][0] = -glm::dot(u, position);
	view[3][1] = -glm::dot(v, position);
	view[3][2] = -glm::dot(w, position);
}

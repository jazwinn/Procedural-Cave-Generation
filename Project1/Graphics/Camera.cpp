/************************************************************************/
/*!
FILENAME:		Camera.cpp
AUTHOR(S):		Yu Heng (100%)
BRIEF:			Camera class that holds the functions for the
				camera handling which includes, movements, updating
				views, and projection matrices, and converting screen
				space coordinates to world space coordinates.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/************************************************************************/
#include "Camera.h"

#include <iostream>

Camera::Camera(float width, float height) : viewportWidth{ width }, viewportHeight{ height } {
	position = glm::vec3{0.f, 0.f, 20.f};
	zoom = 1.0f;
	aspectRatio = viewportWidth / viewportHeight;
	nearClipPlane = 0.1f;
	farClipPlane = 1000.0f;
	isFollowing = false;
	followSpeed = 0.01f;
	toUpdate = true;
	fovDegree = 70.f;
	direction = glm::vec3{ -1,0,-15 };
	UpdateProjection();
}

void Camera::SetPosition(const glm::vec3& pos) { position = pos; }

void Camera::SetZoom(float z) { zoom = glm::clamp(z, 0.1f, 2.f); }

glm::vec3 Camera::GetPosition() const { return position; }

float Camera::GetZoom() const { return zoom; }

glm::mat4 Camera::GetProjectionMatrix() const { return projectionMatrix; }

glm::mat4 Camera::GetViewMatrix() const { return viewMatrix; }

glm::mat4 Camera::GetViewProjectionMatrix() const { return cameraMatrix; }

void Camera::UpdateProjection() {
	/*float halfWidth = (viewportWidth / zoom) * 0.5f;
	float halfHeight = (viewportHeight / zoom) * 0.5f;
	projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearClipPlane, farClipPlane);*/
	projectionMatrix = glm::perspective(glm::radians(fovDegree), aspectRatio , nearClipPlane, farClipPlane);
}

void Camera::UpdateView() {
	//glm::mat4 result = glm::mat4{ 1.0f };
	//result = glm::scale(result, glm::vec3(zoom, zoom, 1.0f));
	//result = glm::translate(result, glm::vec3(-position.x, -position.y, 0.0f));	
	//viewMatrix = result;

	viewMatrix = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
}

void Camera::UpdateViewProjection() { cameraMatrix = GetProjectionMatrix() * GetViewMatrix(); }

void Camera::Move(const glm::vec3& offset, float spd) { position += offset * spd; toUpdate = true; }

void Camera::Resize(float newWidth, float newHeight) {
	viewportWidth = newWidth;
	viewportHeight = newHeight;
	aspectRatio = viewportWidth / viewportHeight;
}

void Camera::Follow(const glm::vec3& tgt, float spd) {
	target = tgt;
	isFollowing = true;
	followSpeed = spd;
}

void Camera::CameraUpdate() {
	if (toUpdate) {
	//	toUpdate = false;
		UpdateView();
		UpdateViewProjection();
	}
	if (isFollowing) {
		Move(target - position, followSpeed);
		//toUpdate = true;
	}
}

glm::vec3 Camera::ScreenToWorldCoordinates(const glm::vec3& screenCoord) const {
	glm::vec3 norm = glm::vec3{ screenCoord.x / viewportWidth * 2 - 1, 1 - screenCoord.y / viewportHeight * 2 , screenCoord.z};
	glm::vec4 worldCoord = glm::inverse(cameraMatrix) * glm::vec4{ norm.x, norm.y, 0.0f, 1.0f };
	return glm::vec3{ worldCoord.x, worldCoord.y , worldCoord.z};
}
#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/************************************************************************/
/*!
	\brief
	Abstract base class representing a generic 2D camera.
*/
/************************************************************************/
class Camera {
public:
	/************************************************************************/
	/*!
		\brief
		Constructor to initialize a Camera with viewport dimensions.
		\param float
		The initial width of the viewport.
		\param float
		The initial height of the viewport.
	*/
	/************************************************************************/
	Camera(float, float);

	/************************************************************************/
	/*!
		\brief
		Sets the camera's position.
		\param const Vector2D&
		The new position for the camera.
	*/
	/************************************************************************/
	void SetPosition(const glm::vec3&);

	/************************************************************************/
	/*!
		\brief
		Gets the current position of the camera.
		\return Vector2D
		The current position of the camera.
	*/
	/************************************************************************/
	glm::vec3 GetPosition() const;

	/************************************************************************/
	/*!
		\brief
		Sets the zoom level of the camera.
		\param float
		The new zoom level for the camera.
	*/
	/************************************************************************/
	void SetZoom(float);

	/************************************************************************/
	/*!
		\brief
		Gets the current zoom level of the camera.
		\return float
		The current zoom level of the camera.
	*/
	/************************************************************************/
	float GetZoom() const;

	/************************************************************************/
	/*!
		\brief
		Gets the projection matrix of the camera.
		\return glm::mat4
		The camera's current projection matrix.
	*/
	/************************************************************************/
	glm::mat4 GetProjectionMatrix() const;

	/************************************************************************/
	/*!
		\brief
		Gets the view matrix of the camera.
		\return glm::mat4
		The camera's current view matrix.
	*/
	/************************************************************************/
	glm::mat4 GetViewMatrix() const;

	/************************************************************************/
	/*!
		\brief
		Gets the combined view-projection matrix of the camera.
		\return glm::mat4
		The camera's current view-projection matrix.
	*/
	/************************************************************************/
	glm::mat4 GetViewProjectionMatrix() const;

	/************************************************************************/
	/*!
		\brief
		Updates the camera's projection matrix based on current settings.
	*/
	/************************************************************************/
	void UpdateProjection();

	/************************************************************************/
	/*!
		\brief
		Updates the camera's view matrix based on the current position and zoom.
	*/
	/************************************************************************/
	void UpdateView();

	/************************************************************************/
	/*!
		\brief
		Updates the combined view-projection matrix for the camera.
	*/
	/************************************************************************/
	void UpdateViewProjection();

	/************************************************************************/
	/*!
		\brief
		Moves the camera by a specified offset at a given speed.
		\param const Vector2D&
		The offset by which to move the camera.
		\param float
		The speed at which to move the camera.
	*/
	/************************************************************************/
	void Move(const glm::vec3&, float);

	/************************************************************************/
	/*!
		\brief
		Resizes the camera's viewport to new dimensions.
		\param float
		The new width of the viewport.
		\param float
		The new height of the viewport.
	*/
	/************************************************************************/
	void Resize(float, float);

	/************************************************************************/
	/*!
		\brief
		Enables the camera to follow a specified target at a given speed.
		\param const Vector2D&
		The target position for the camera to follow.
		\param float
		The speed at which the camera follows the target.
	*/
	/************************************************************************/
	void Follow(const glm::vec3&, float);

	/************************************************************************/
	/*!
		\brief
		Updates the camera's matrices and internal state.
	*/
	/************************************************************************/
	void CameraUpdate();

	/************************************************************************/
	/*!
		\brief
		Transforms screen coordinates to world coordinates based on the camera's view.
		\param const Vector2D&
		The screen coordinates to transform.
		\return Vector2D
		The transformed world coordinates.
	*/
	/************************************************************************/
	glm::vec3 ScreenToWorldCoordinates(const glm::vec3&) const;

public:
	glm::vec3 direction;
	glm::vec2 prevMouseCursor;

private:
	glm::vec3 position{0,0,0};             /*!< The current position of the camera. */
	float zoom;                    /*!< The current zoom level of the camera. */
	float aspectRatio;             /*!< The current aspect ratio of the viewport. */
	float viewportWidth;           /*!< The width of the viewport. */
	float viewportHeight;          /*!< The height of the viewport. */
	float nearClipPlane;           /*!< The near clipping plane for projection. */
	float farClipPlane;            /*!< The far clipping plane for projection. */
	glm::mat4 projectionMatrix;    /*!< The projection matrix of the camera. */
	glm::mat4 viewMatrix;          /*!< The view matrix of the camera. */
	glm::mat4 cameraMatrix;        /*!< The combined view-projection matrix. */
	glm::vec3 target;               /*!< The target position for the camera to follow. */		
	bool isFollowing;              /*!< Flag indicating if the camera is following a target. */
	float followSpeed;             /*!< Speed of following the target. */
	bool toUpdate{true};                 /*!< Flag indicating if matrices need updating. */
	float fovDegree;

};

#endif
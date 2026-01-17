#include "Ankka/tools/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getViewMatrix(OGLRenderData& renderData)
{

	float azimRad = glm::radians(renderData.rdViewAzimuth);
	float elevRad = glm::radians(renderData.rdViewElevation);

	float sinAzim = glm::sin(azimRad);
	float cosAzim = glm::cos(azimRad);

	float sinElev = glm::sin(elevRad);
	float cosElev = glm::sin(elevRad);

	mViewDirection = glm::normalize(
		glm::vec3(
			sinAzim * cosElev,
			sinElev,
			-cosAzim * cosElev));

	return glm::lookAt(mWorldPos,
		mWorldPos + mViewDirection,
		mWroldUpVector);
}
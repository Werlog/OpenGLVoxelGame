#include <glm/glm.hpp>

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

constexpr float nearPlane = 0.1f;
constexpr float farPlane = 1000.0f;

class Camera {
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;
	float fov;

	float movementSpeed;
	float mouseSensitivity;

	Camera(glm::vec3 position, float fov, float aspectRatio, float movementSpeed);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	void ProccessKeyboard(CameraMovement movement, float deltaTime);
	void ProccessMouse(float mouseX, float mouseY, bool constrainPitch = true);
	void updateProjectionMatrix(float aspectRatio);
private:
	float lastMouseX, lastMouseY;
	bool firstMouse;
	glm::mat4 projectionMatrix;

	void updateCameraVectors();
};
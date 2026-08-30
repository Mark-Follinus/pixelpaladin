#include "input_windows.hpp"

// Standard library includes
#include <algorithm>

// Engine includes
#include "core/engine.hpp"
#include "device/device.hpp"
#include "input/input.hpp"

// ------------------------
// Public Methods
// ------------------------

bool Input::GetKeyboardKey(KeyboardKey key) const
{
	const int k = static_cast<int>(key);
	assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
	return m_inputData.KeysDown[k];
}

bool Input::GetKeyboardKeyDown(KeyboardKey key) const
{
	const int k = static_cast<int>(key);
	assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
	return m_inputData.KeysDown[k] && !m_inputData.KeysDownPrevious[k];
}

bool Input::GetKeyboardKeyUp(KeyboardKey key) const
{
	const int k = static_cast<int>(key);
	assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
	return !m_inputData.KeysDown[k] && m_inputData.KeysDownPrevious[k];
}

bool Input::GetMouseButton(MouseButton button) const
{
	const int b = static_cast<int>(button);
	assert(b >= GLFW_MOUSE_BUTTON_1 && b <= GLFW_MOUSE_BUTTON_LAST);
	return m_inputData.MouseButtonsDown[b];
}

bool Input::GetMouseButtonDown(MouseButton button) const
{
	const int b = static_cast<int>(button);
	assert(b >= GLFW_MOUSE_BUTTON_1 && b <= GLFW_MOUSE_BUTTON_LAST);
	return m_inputData.MouseButtonsDown[b] && !m_inputData.MouseButtonsDownPrevious[b];
}

bool Input::GetMouseButtonUp(MouseButton button) const
{
	const int b = static_cast<int>(button);
	assert(b >= GLFW_MOUSE_BUTTON_1 && b <= GLFW_MOUSE_BUTTON_LAST);
	return !m_inputData.MouseButtonsDown[b] && m_inputData.MouseButtonsDownPrevious[b];
}

float Input::GetMouseWheel() const
{
	return m_inputData.MouseWheel;
}

float Input::GetMouseWheelMax() const
{
	return MOUSE_WHEEL_MAXIMUM;
}

float Input::GetMouseWheelMin() const
{
	return MOUSE_WHEEL_MINIMUM;
}

glm::vec2 Input::GetRawMousePosition() const
{
	return m_inputData.RawMousePosition;
}

//float Input::GetGamepadAxis(GamepadAxis axis) const
//{
//	if (!m_inputData.GamepadConnected)
//	{
//		return 0.0;
//	} 
//	
//	const int a = static_cast<int>(axis);
//	assert(a >= 0 && a <= GLFW_GAMEPAD_AXIS_LAST);
//	return m_inputData.GamepadState.axes[a];
//}

//float Input::GetGamepadAxisPrevious(GamepadAxis axis) const
//{
//	if (!m_inputData.GamepadConnected)
//	{
//		return 0.0;
//	}
//
//	const int a = static_cast<int>(axis);
//	assert(a >= 0 && a <= GLFW_GAMEPAD_AXIS_LAST);
//	return m_inputData.GamepadStatePrevious.axes[a];
//}

//bool Input::GetGamepadButton(GamepadButton button) const
//{
//	if (!m_inputData.GamepadConnected)
//	{
//		return 0.0;
//	}
//
//	const int b = static_cast<int>(button);
//	assert(b >= 0 && b <= GLFW_GAMEPAD_BUTTON_LAST);
//	return m_inputData.GamepadState.buttons[b] == GLFW_PRESS;
//}

//bool Input::GetGamepadButtonDown(GamepadButton button) const
//{
//	if (!m_inputData.GamepadConnected)
//	{
//		return 0.0;
//	}
//
//	const int b = static_cast<int>(button);
//	assert(b >= 0 && b <= GLFW_GAMEPAD_BUTTON_LAST);
//	return m_inputData.GamepadState.buttons[b] == GLFW_PRESS && m_inputData.GamepadStatePrevious.buttons[b] == GLFW_RELEASE;
//}

//bool Input::GetGamepadButtonUp(GamepadButton button) const
//{
//	if (!m_inputData.GamepadConnected)
//	{
//		return 0.0;
//	}
//
//	const int b = static_cast<int>(button);
//	assert(b >= 0 && b <= GLFW_GAMEPAD_BUTTON_LAST);
//	return !m_inputData.GamepadState.buttons[b] == GLFW_RELEASE && m_inputData.GamepadStatePrevious.buttons[b] == GLFW_PRESS;
//}

// ------------------------
// Private Constructors
// ------------------------ 

Input::Input(Device* device) : m_device(device)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(m_device->GetWindow());
	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, MouseWheelCallback);
	glfwSetCursorPosCallback(window, MousePositionCallback);

	Update();
}

Input::~Input()
{
	GLFWwindow* window = static_cast<GLFWwindow*>(m_device->GetWindow()); // Owning device pointer gets deleted last, so we can do this. We live dangerously.

	glfwSetKeyCallback(window, NULL);
	glfwSetMouseButtonCallback(window, NULL);
	glfwSetScrollCallback(window, NULL);
	glfwSetCursorPosCallback(window, NULL);
}

// ------------------------
// Private Static Methods
// ------------------------ 

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
	if (!input) 
	{
		return;
	}

	if (key >= 0 && key < NUMBER_OF_KEYS)
	{
		if (action == GLFW_PRESS || action == GLFW_RELEASE)
		{
			input->m_inputData.KeysAction[key] = static_cast<Input::InputAction>(action);
		}
	}
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
	if (!input)
	{
		return;
	}

	if (button >= 0 && button < NUMBER_OF_MOUSE_BUTTONS)
	{
		if (action == GLFW_PRESS || action == GLFW_RELEASE)
		{
			input->m_inputData.MouseButtonsAction[button] = static_cast<Input::InputAction>(action);
		}
	}
}

void Input::MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
	if (!input)
	{
		return;
	}

	input->m_inputData.MouseWheel += static_cast<float>(yoffset);
	input->m_inputData.MouseWheel = std::clamp(input->m_inputData.MouseWheel, MOUSE_WHEEL_MINIMUM, MOUSE_WHEEL_MAXIMUM);
}

void Input::MousePositionCallback(GLFWwindow* window, double xposition, double yposition)
{
	Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
	if (!input)
	{
		return;
	}

	input->m_inputData.RawMousePosition.x = static_cast<float>(xposition);
	input->m_inputData.RawMousePosition.y = static_cast<float>(yposition);
}

// ------------------------
// Private Methods
// ------------------------

void Input::Update()
{
	// Update keyboard key states.
	for (int i = 0; i < NUMBER_OF_KEYS; ++i)
	{
		m_inputData.KeysDownPrevious[i] = m_inputData.KeysDown[i];

		if (m_inputData.KeysAction[i] == InputAction::Press)
		{
			m_inputData.KeysDown[i] = true;
		}
		else if (m_inputData.KeysAction[i] == InputAction::Release)
		{
			m_inputData.KeysDown[i] = false;
		}

		m_inputData.KeysAction[i] = InputAction::None;
	}

	// Update mouse button states.
	for (int i = 0; i < NUMBER_OF_MOUSE_BUTTONS; ++i)
	{
		m_inputData.MouseButtonsDownPrevious[i] = m_inputData.MouseButtonsDown[i];

		if (m_inputData.MouseButtonsAction[i] == InputAction::Press)
		{
			m_inputData.MouseButtonsDown[i] = true;
		}

		else if (m_inputData.MouseButtonsAction[i] == InputAction::Release)
		{
			m_inputData.MouseButtonsDown[i] = false;
		}
			
		m_inputData.MouseButtonsAction[i] = InputAction::None;
	}

	// Update gamepad states.
	//if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
	//{
	//	m_inputData.GamepadStatePrevious = m_inputData.GamepadState;
	//	m_inputData.GamepadConnected = static_cast<bool>(glfwGetGamepadState(GLFW_JOYSTICK_1, &m_inputData.GamepadState));
	//}
	//else 
	//{
	//	m_inputData.GamepadConnected = false;
	//}
}
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
	const i32 k = static_cast<i32>(key);
	assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
	return m_inputData.KeysDown[k];
}

bool Input::GetKeyboardKeyDown(KeyboardKey key) const
{
	const i32 k = static_cast<i32>(key);
	assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
	return m_inputData.KeysDown[k] && !m_inputData.KeysDownPrevious[k];
}

bool Input::GetKeyboardKeyUp(KeyboardKey key) const
{
	const i32 k = static_cast<i32>(key);
	assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
	return !m_inputData.KeysDown[k] && m_inputData.KeysDownPrevious[k];
}

bool Input::GetMouseButton(MouseButton button) const
{
	const i32 b = static_cast<i32>(button);
	assert(b >= GLFW_MOUSE_BUTTON_1 && b <= GLFW_MOUSE_BUTTON_LAST);
	return m_inputData.MouseButtonsDown[b];
}

bool Input::GetMouseButtonDown(MouseButton button) const
{
	const i32 b = static_cast<i32>(button);
	assert(b >= GLFW_MOUSE_BUTTON_1 && b <= GLFW_MOUSE_BUTTON_LAST);
	return m_inputData.MouseButtonsDown[b] && !m_inputData.MouseButtonsDownPrevious[b];
}

bool Input::GetMouseButtonUp(MouseButton button) const
{
	const i32 b = static_cast<i32>(button);
	assert(b >= GLFW_MOUSE_BUTTON_1 && b <= GLFW_MOUSE_BUTTON_LAST);
	return !m_inputData.MouseButtonsDown[b] && m_inputData.MouseButtonsDownPrevious[b];
}

f32 Input::GetMouseWheel() const
{
	return m_inputData.MouseWheel;
}

f32 Input::GetMouseWheelMax() const
{
	return MOUSE_WHEEL_MAXIMUM;
}

f32 Input::GetMouseWheelMin() const
{
	return MOUSE_WHEEL_MINIMUM;
}

glm::vec2 Input::GetRawMousePosition() const
{
	return m_inputData.RawMousePosition;
}

f32 Input::GetGamepadAxis(GamepadAxis axis, i32 gamepadID) const
{
	if (!m_inputData.GamepadsConnected[gamepadID])
	{
		return 0.0;
	} 
	
	const GLFWgamepadstate& gamePad = m_inputData.GamepadState[gamepadID];
	const i32 a = static_cast<int>(axis);
	assert(a >= 0 && a <= GLFW_GAMEPAD_AXIS_LAST);
	return gamePad.axes[a];
}

f32 Input::GetGamepadAxisPrevious(GamepadAxis axis, i32 gamepadID) const
{
	if (!m_inputData.GamepadsConnected[gamepadID])
	{
		return 0.0;
	}

	const GLFWgamepadstate& gamePad = m_inputData.GamepadStatePrevious[gamepadID];
	const i32 a = static_cast<int>(axis);
	assert(a >= 0 && a <= GLFW_GAMEPAD_AXIS_LAST);
	return gamePad.axes[a];
}

bool Input::GetGamepadButton(GamepadButton button, i32 gamepadID) const
{
	if (!m_inputData.GamepadsConnected[gamepadID])
	{
		return 0.0;
	}

	const GLFWgamepadstate& gamePad = m_inputData.GamepadState[gamepadID];

	const i32 b = static_cast<int>(button);
	assert(b >= 0 && b <= GLFW_GAMEPAD_AXIS_LAST);
	return gamePad.buttons[b];
}

bool Input::GetGamepadButtonDown(GamepadButton button, i32 gamepadID) const
{
	if (!m_inputData.GamepadsConnected[gamepadID])
	{
		return 0.0;
	}

	const GLFWgamepadstate& gamePad = m_inputData.GamepadState[gamepadID];
	const GLFWgamepadstate& gamePadPrevious = m_inputData.GamepadStatePrevious[gamepadID];

	const i32 b = static_cast<int>(button);
	assert(b >= 0 && b <= GLFW_GAMEPAD_AXIS_LAST);
	return gamePad.buttons[b] && !gamePadPrevious.buttons[b];
}

bool Input::GetGamepadButtonUp(GamepadButton button, i32 gamepadID) const
{
	if (!m_inputData.GamepadsConnected[gamepadID])
	{
		return 0.0;
	}

	const GLFWgamepadstate& gamePad = m_inputData.GamepadState[gamepadID];
	const GLFWgamepadstate& gamePadPrevious = m_inputData.GamepadStatePrevious[gamepadID];

	const i32 b = static_cast<int>(button);
	assert(b >= 0 && b <= GLFW_GAMEPAD_AXIS_LAST);
	return !gamePad.buttons[b] && gamePadPrevious.buttons[b];
}

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

void Input::KeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
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

void Input::MouseButtonCallback(GLFWwindow* window, i32 button, i32 action, i32 mods)
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

void Input::MouseWheelCallback(GLFWwindow* window, f64 xoffset, f64 yoffset)
{
	Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
	if (!input)
	{
		return;
	}

	input->m_inputData.MouseWheel += static_cast<f32>(yoffset);
	input->m_inputData.MouseWheel = std::clamp(input->m_inputData.MouseWheel, MOUSE_WHEEL_MINIMUM, MOUSE_WHEEL_MAXIMUM);
}

void Input::MousePositionCallback(GLFWwindow* window, f64 xposition, f64 yposition)
{
	Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
	if (!input)
	{
		return;
	}

	input->m_inputData.RawMousePosition.x = static_cast<f32>(xposition);
	input->m_inputData.RawMousePosition.y = static_cast<f32>(yposition);
}

// ------------------------
// Private Methods
// ------------------------

void Input::Update()
{
	// Update keyboard key states.
	for (i32 i = 0; i < NUMBER_OF_KEYS; ++i)
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
	for (i32 i = 0; i < NUMBER_OF_MOUSE_BUTTONS; ++i)
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
	for (int i = 0; i < MAX_NUMBER_OF_GAMEPADS; ++i)
	{
		m_inputData.GamepadStatePrevious[i] = m_inputData.GamepadState[i];

		if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i))
		{
			m_inputData.GamepadsConnected[i] = static_cast<bool>(glfwGetGamepadState(i, &m_inputData.GamepadState[i]));
		}
	}
}
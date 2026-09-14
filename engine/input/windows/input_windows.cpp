#include "input_windows.hpp"

// Standard library includes
#include <algorithm>

// Engine includes
#include "../commands/input_command.hpp"
#include "device/device.hpp"

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	bool Input::GetKeyboardKey(Input::KeyboardKey key) const
	{
		const int k = static_cast<int>(key);
		assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
		return m_inputRawData.KeysDown[k];
	}

	bool Input::GetKeyboardKeyDown(KeyboardKey key) const
	{
		const int k = static_cast<int>(key);
		assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
		return m_inputRawData.KeysDown[k] && !m_inputRawData.KeysDownPrevious[k];
	}

	bool Input::GetKeyboardKeyUp(KeyboardKey key) const
	{
		const int k = static_cast<int>(key);
		assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
		return !m_inputRawData.KeysDown[k] && m_inputRawData.KeysDownPrevious[k];
	}

	bool Input::GetMouseButton(MouseButton button) const
	{
		const int b = static_cast<int>(button);
		assert(b >= GLFW_MOUSE_BUTTON_1 && b <= GLFW_MOUSE_BUTTON_LAST);
		return m_inputRawData.MouseButtonsDown[b];
	}

	bool Input::GetMouseButtonDown(MouseButton button) const
	{
		const int b = static_cast<int>(button);
		assert(b >= GLFW_MOUSE_BUTTON_1 && b <= GLFW_MOUSE_BUTTON_LAST);
		return m_inputRawData.MouseButtonsDown[b] && !m_inputRawData.MouseButtonsDownPrevious[b];
	}

	bool Input::GetMouseButtonUp(MouseButton button) const
	{
		const int b = static_cast<int>(button);
		assert(b >= GLFW_MOUSE_BUTTON_1 && b <= GLFW_MOUSE_BUTTON_LAST);
		return !m_inputRawData.MouseButtonsDown[b] && m_inputRawData.MouseButtonsDownPrevious[b];
	}

	float Input::GetMouseWheel() const
	{
		return m_inputRawData.MouseWheel;
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
		return m_inputRawData.RawMousePosition;
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
				input->m_inputRawData.KeysAction[key] = static_cast<Input::InputPressState>(action);
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
				input->m_inputRawData.MouseButtonsAction[button] = static_cast<Input::InputPressState>(action);
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

		input->m_inputRawData.MouseWheel += static_cast<float>(yoffset);
		input->m_inputRawData.MouseWheel = std::clamp(input->m_inputRawData.MouseWheel, MOUSE_WHEEL_MINIMUM, MOUSE_WHEEL_MAXIMUM);
	}

	void Input::MousePositionCallback(GLFWwindow* window, double xposition, double yposition)
	{
		Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
		if (!input)
		{
			return;
		}

		input->m_inputRawData.RawMousePosition.x = static_cast<float>(xposition);
		input->m_inputRawData.RawMousePosition.y = static_cast<float>(yposition);
	}

	// ------------------------
	// Private Methods
	// ------------------------

	InputCommand Input::Update()
	{
		// Update keyboard key states.
		for (int i = 0; i < NUMBER_OF_KEYS; ++i)
		{
			m_inputRawData.KeysDownPrevious[i] = m_inputRawData.KeysDown[i];

			if (m_inputRawData.KeysAction[i] == InputPressState::Press)
			{
				m_inputRawData.KeysDown[i] = true;
			}
			else if (m_inputRawData.KeysAction[i] == InputPressState::Release)
			{
				m_inputRawData.KeysDown[i] = false;
			}

			m_inputRawData.KeysAction[i] = InputPressState::None;
		}

		// Update mouse button states.
		for (int i = 0; i < NUMBER_OF_MOUSE_BUTTONS; ++i)
		{
			m_inputRawData.MouseButtonsDownPrevious[i] = m_inputRawData.MouseButtonsDown[i];

			if (m_inputRawData.MouseButtonsAction[i] == InputPressState::Press)
			{
				m_inputRawData.MouseButtonsDown[i] = true;
			}

			else if (m_inputRawData.MouseButtonsAction[i] == InputPressState::Release)
			{
				m_inputRawData.MouseButtonsDown[i] = false;
			}

			m_inputRawData.MouseButtonsAction[i] = InputPressState::None;
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

		InputCommand command = ConfigureInputCommand();
		command.SequenceNumber = m_sequenceNumber++;
		command.ClientTick = m_clientTick++;

		return command;
	}

	const InputCommand Input::ConfigureInputCommand() const
	{
		InputCommand command{};

		signed char moveX = 0, moveY = 0;
		if (GetKeyboardKey(m_moveDownKey)) moveY -= 127;
		if (GetKeyboardKey(m_moveUpKey)) moveY += 127;
		if (GetKeyboardKey(m_moveLeftKey)) moveX -= 127;
		if (GetKeyboardKey(m_moveRightKey)) moveX += 127;

		command.MoveX = static_cast<int8_t>(moveX);
		command.MoveY = static_cast<int8_t>(moveY);

		if (GetKeyboardKeyDown(m_attackKey))
		{
			command.ActionsDown |= static_cast<uint16_t>(InputCommandTypeAction::Attack);
		}

		if (GetKeyboardKey(m_attackKey))
		{
			command.ActionsHeld |= static_cast<uint16_t>(InputCommandTypeAction::Attack);
		}

		if (GetKeyboardKeyUp(m_attackKey))
		{
			command.ActionsUp |= static_cast<uint16_t>(InputCommandTypeAction::Attack);
		}

		return command;
	}

} // namespace pldn
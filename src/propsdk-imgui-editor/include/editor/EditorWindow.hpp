//
//  EditorWindow.hpp
//  easyloggingpp
//
//  Created by admin on 29.05.2020.
//
#ifndef EDITOR_EDITORWINDOW_HPP
#define EDITOR_EDITORWINDOW_HPP

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Initialize with gladLoadGL()
#include <glad/glad.h>

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include <string_view>
#include <memory>
#include <functional>

namespace editor {
	class WindowView {
	};

	using WindowViewPtr = std::shared_ptr < WindowView >;
	class EditorWindow {
	public:
		EditorWindow();
		EditorWindow(const EditorWindow&) = delete;
		EditorWindow& operator = (const EditorWindow&) = delete;
		~EditorWindow();

		WindowViewPtr main;

		//        void addSubView(WindowViewPtr);
		[[nodiscard]] int setup_window();
		[[nodiscard]] int loop();

		[[nodiscard]] int error_code() const noexcept;

	private:
		enum class ErrorCode : int {
			NoError = 0,
			WindowSetupError = -1
		};

	private:
		[[nodiscard]] int cast_error_code(const ErrorCode code) const noexcept;
		static void glfw_error_callback(int error, const char* description);
		std::string_view set_glsl_glfw();
		void new_frame();

	private:
		std::function < void(GLFWwindow* window) > window_destroyer_;
		std::unique_ptr < GLFWwindow, std::function < void(GLFWwindow* window) >> window_;
		ErrorCode error_code_;
	};
};

#endif // EDITOR_EDITORWINDOW_HPP

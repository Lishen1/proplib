#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

// Initialize with gladLoadGL()
#include <glad/glad.h>

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include <string_view>
// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
//#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
//#pragma comment(lib, "legacy_stdio_definitions")
//#endif

#include <serialize/proplib.h>
#include <easylogging++.h>
INITIALIZE_EASYLOGGINGPP

std::string random_string(size_t length)
{
  auto randchar = []() -> char {
    const char charset[]
      = "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

class TestClass_1 : public proplib::Serializable
{
public:
  std::vector<int>           vector_int;
  std::vector<float>         vector_float;
  float                      float_field;
  int                        int_field;
  std::string                string_field;
  std::map<int, std::string> map_int_string;

  bool bool_field;

  TestClass_1() {}
  void set_random()
  {
    for (int i = 0; i < 3; i++)
    {
      vector_int.push_back(rand());
      vector_float.push_back((float)rand() / RAND_MAX);
    }

    float_field = (float)rand() / RAND_MAX;
    int_field = rand();

    string_field = random_string(20);
    map_int_string[rand()] = random_string(5);

    bool_field = rand()%2;
  }

  bool operator==(const TestClass_1& other)
  {
    if (fabs(float_field - other.float_field) > std::numeric_limits<float>::epsilon())
      return false;

    if (int_field != other.int_field)
      return false;

    if (bool_field != other.bool_field)
      return false;

    if (string_field != other.string_field)
      return false;

    if (vector_int != other.vector_int)
      return false;

    if (vector_float.size() != other.vector_float.size())
      return false;

    for (int i = 0; i < vector_float.size(); i++)
    {
      if (fabs(vector_float[i] - other.vector_float[i]) > std::numeric_limits<float>::epsilon())
        return false;
    }

    if (map_int_string != other.map_int_string)
      return false;

    return true;
  }

  void reset()
  {
    vector_int.clear();
    vector_float.clear();
    float_field = 0.f;
    int_field = 0;
    string_field = "";
    map_int_string.clear();
    bool_field = false;
  }

private:
  SERIALIZE(vector_int, "this is vector of integers");
  SERIALIZE(vector_float, "this is vector of floats");
  SERIALIZE(float_field, "just float");
  SERIALIZE(int_field, "just int");
  SERIALIZE(string_field, "it is string!");
  SERIALIZE(map_int_string, "map int <-> string");
  SERIALIZE(bool_field, "just bool");
};

	
template<typename T>
class WindowView {
public:
WindowView() : WindowView("Empty")
{
}

WindowView(const std::string_view &window_name)
{
    ImGui::Begin(window_name.data());
}

~WindowView()
{
    ImGui::End();
}
};
class Window {
public:
Window() = default;
Window(const Window&) = delete;
Window &operator=(const Window&) = delete;
~Window()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

[[nodiscard]] int setup_window()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) return cast_error_code(error_code_ = ErrorCode::WindowSetupError);

    // Decide GL+GLSL versions
    std::string_view glsl_version = set_glsl_glfw();

    // Create window with graphics context
    window_ = std::unique_ptr<GLFWwindow, decltype(window_destroyer_) >(glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL), window_destroyer_);
    auto window = window_.get();
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);   // Enable vsync

    // Initialize OpenGL loader
    bool err = gladLoadGL() == 0;
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return cast_error_code(error_code_ = ErrorCode::WindowSetupError);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const auto raw_glsl_version = glsl_version.data();
    ImGui_ImplOpenGL3_Init(raw_glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    //  bool show_demo_window = true;
    //  bool show_another_window = true;

    // Create Device Objects
    ImGui_ImplOpenGL3_NewFrame();
    return cast_error_code(error_code_ = ErrorCode::NoError);
}

[[nodiscard]] int main_loop()
{
    auto window = window_.get();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Recive new frame
        new_frame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //    if (show_demo_window)
        //      ImGui::ShowDemoWindow(&show_demo_window);
        //
        //    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        //    {
        //      static float f = 0.0f;
        //      static int counter = 0;
        //
        //      ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
        //
        //      ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        //      ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //      ImGui::Checkbox("Another Window", &show_another_window);
        //
        //      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //      ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        //
        //      if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //        counter++;
        //      ImGui::SameLine();
        //      ImGui::Text("counter = %d", counter);
        //
        //      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //      ImGui::End();
        //    }
        //
        //    // 3. Show another simple window.
        //    if (show_another_window)
        //    {
        //      ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //      ImGui::Text("Hello from another window!");
        //      if (ImGui::Button("Close Me"))
        //        show_another_window = false;
        //      ImGui::End();
        //    }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    return cast_error_code(ErrorCode::NoError);
}

[[nodiscard]] int error_code() const noexcept
{
    return cast_error_code(error_code_);
}

private:
enum class ErrorCode: int {
    NoError          = 0,
    WindowSetupError = -1
};

private:
[[nodiscard]] int cast_error_code(const ErrorCode code) const noexcept
{
    return static_cast<int>(code);
}

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

std::string_view set_glsl_glfw()
{
    #if __APPLE__
    // GL 3.2 + GLSL 150
    std::string_view glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);              // Required on Mac
    #else
    // GL 3.0 + GLSL 130
    std::string_view glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    #endif
    return glsl_version;
}

void new_frame()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

private:
std::function<void(GLFWwindow *window)> window_destroyer_ = [](GLFWwindow *window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    };
std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow *window)> > window_;
ErrorCode error_code_;
};

int main(int, char **)
{
  TestClass_1 ser_test;
  ser_test.set_random();
  YAML::Emitter out;
  out << YAML::BeginMap;
  ser_test.serialize(out, true);
  out << YAML::EndMap;
  std::cout << "yaml\n" << out.c_str() << std::endl;
  ser_test.deserialize(out);
//    Window window;
//    if (window.setup_window() ) {
//        return window.error_code();
//    }
//    return window.main_loop();
}

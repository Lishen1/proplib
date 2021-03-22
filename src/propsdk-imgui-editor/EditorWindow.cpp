//
//  EditorWindow.cpp
//  easyloggingpp
//
//  Created by admin on 29.05.2020.
//

#include <editor/EditorWindow.hpp>

#include <gui/gui.hpp>

#include <imfilebrowser.h>


namespace editor {


EditorWindow::EditorWindow() :
    window_destroyer_ {[](GLFWwindow *window) {
            glfwDestroyWindow(window);
            glfwTerminate();
    }},
    error_code_{ ErrorCode::NoError } {}

EditorWindow::~EditorWindow()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

int EditorWindow::setup_window()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) return cast_error_code(error_code_ = ErrorCode::WindowSetupError);

    // Decide GL+GLSL versions
    std::string_view glsl_version = set_glsl_glfw();

    window_ = std::unique_ptr<GLFWwindow, decltype(window_destroyer_) >(glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL), window_destroyer_);
    // Create window with graphics context
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


    // Create Device Objects
    ImGui_ImplOpenGL3_NewFrame();
    return cast_error_code(error_code_ = ErrorCode::NoError);
}

[[nodiscard]] int EditorWindow::loop()
{
    auto window = window_.get();
    ImVec4 clear_color = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    auto root = YAML::Node();
    gui::SerializableGuiElementPtr serializableGui;

    ImGui::FileBrowser fileDialog;

    // (optional) set browser properties
    fileDialog.SetTitle("title");
    fileDialog.SetTypeFilters({ ".yaml", ".yml" });
    
    int age_prop{ 77 };
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
        // Menu Bar
        

//        ImGui::ShowDemoWindow();
        ImGui::SetNextWindowPos(ImVec2(50, 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 340), ImGuiCond_FirstUseEver);
        bool is_opened{true};
        // Main body of the Demo window starts here.
        if(ImGui::Begin("Property ", &is_opened, ImGuiWindowFlags_MenuBar)) {

          if (ImGui::BeginMenuBar())
          {
            if (ImGui::BeginMenu("File"))
            {
              if(ImGui::MenuItem("Open")) {
                fileDialog.Open();
              }
              ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
          }

          fileDialog.Display();
          if (serializableGui) {
            // open file dialog when user clicks this button
            serializableGui->makeGui();
          } 

          if (fileDialog.HasSelected())
          {
            root = YAML::LoadFile(fileDialog.GetSelected().string());
            serializableGui = std::make_shared<gui::SerializableGuiElement>(root);
            fileDialog.ClearSelected();
          }
        }
        ImGui::End();
        

        
        //serializableGui->makeGui();
       // ImGui::ShowDemoWindow();

        
        ImGui::SetNextWindowPos(ImVec2(50, 380), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 200), ImGuiCond_FirstUseEver);

        // Main body of the Demo window starts here.
        ImGui::Begin("Detail");
        
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 560), ImGuiCond_FirstUseEver);

        // Main body of the Demo window starts here.
        ImGui::Begin("Console");
        YAML::Emitter out;
        out << root;
        ImGui::TextUnformatted(out.c_str());

        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
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

int EditorWindow::error_code() const noexcept
{
    return cast_error_code(error_code_);
}

int EditorWindow::cast_error_code(const ErrorCode code) const noexcept
{
    return static_cast<int>(code);
}

void EditorWindow::glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

std::string_view EditorWindow::set_glsl_glfw()
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

void EditorWindow::new_frame()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
}

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

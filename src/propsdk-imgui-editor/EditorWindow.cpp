//
//  EditorWindow.cpp
//  easyloggingpp
//
//  Created by admin on 29.05.2020.
//

#include <editor/EditorWindow.hpp>

#include <iostream>

#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <vector>

namespace editor {
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

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

struct YamlInfo {
    std::string name;
    std::string type_name;
    std::string doc;
    YAML::NodeType::value type;
    
    YamlInfo() = default;
    YamlInfo(YAML::iterator &node) {
        name = node->first.as<std::string>();
        type_name = node->second.Tag();
        this->node = node->second;
        type = this->node.Type();

        YAML::iterator iter = node;
        ++iter;
        // Check we have doc tag
        if (iter->second && iter->second.Tag() == "doc") {
            // we know after node data we put
            // <DOC> node so we just increment it
            ++node;
            try {
                doc = node->second.as<std::string>();
            }
            catch (...) {
                std::cout << "fuk";
            }
        }
        
    }
protected:
    YAML::Node node;
    
    
};

template<typename T>
struct GuiElement: YamlInfo {
    using type_value = T;
    using iterator = type_value*;
    using const_iterator = const iterator;
    
    virtual void makeGui() {}
    GuiElement() = default;
    GuiElement(YAML::iterator &node) : YamlInfo(node) {}
    virtual ~GuiElement() = default;
    
};


struct StringGuiElement: GuiElement<std::string> {
public:
    std::string& get() {
        return value;
    }
    void set(const std::string& new_value) {
        node[name.data()] = value = new_value;
    }
    void makeGui() override {
        std::array<char, 128> buffer{};
        std::copy(value.begin(), value.end(), buffer.begin());
        if (ImGui::InputText((name + " - " + type_name).data(), buffer.data(), buffer.size())){
            value = buffer.data();
        } ImGui::SameLine();
        HelpMarker(doc.data());
        
    }
    StringGuiElement(YAML::iterator &node) : GuiElement(node) {
        try {
            value = node->second.as<std::string>();
        } catch (...) {
            std::cout << "fuk";
        }
    }
    ~StringGuiElement() override = default;
private:
    std::string value;
};

struct IntGuiElement: GuiElement<int> {
    int& get() {
        return value;
    }
    void set(const int new_value) {
        node[name.data()] = value = new_value;
    }
    void makeGui() override {
        ImGui::InputInt((name + " - " + type_name).data(), &value); ImGui::SameLine();
        HelpMarker(doc.data());
    }
    IntGuiElement(YAML::iterator &node) : GuiElement(node) {
        std::cout << node->second;
        try {
            value = this->node.as<int>();
        } catch (...) {
            std::cout << "fuk";
        }
    }
    virtual ~IntGuiElement() override = default;
private:
    int value;
};

struct FloatGuiElement: GuiElement<double> {
    double& get() {
        return value;
    }
    void set(const int new_value) {
        node[name.data()] = value = new_value;
    }
    void makeGui() override {
        ImGui::InputDouble((name + " - " + type_name).data(), &value); ImGui::SameLine();
        HelpMarker(doc.data());
    }
    FloatGuiElement(YAML::iterator &node) : GuiElement(node) {
        try {
            value = this->node.as<double>();
        } catch (...) {
            std::cout << "fuk";
        }
    }
    virtual ~FloatGuiElement() override = default;
private:
    double value;
};

struct BoolGuiElement: GuiElement<bool> {
    bool& get() {
        return value;
    }
    void set(const int new_value) {
        node[name.data()] = value = new_value;
    }
    void makeGui() override {
        if (ImGui::Checkbox((name + " - " + type_name /*+ std::to_string(randHash)*/).data(), &value)) {
            std::cout << value;
        } ImGui::SameLine();
        HelpMarker(doc.data());
    }
    BoolGuiElement(YAML::iterator &node) : GuiElement(node) {
        randHash = rand();
        try {
            value = this->node.as<bool>();
        } catch (...) {
            std::cout << "fuk";
        }
    }
    virtual ~BoolGuiElement() override = default;
private:
    int randHash;
    bool value;
};

template <typename T>
T* gui_cast(YamlInfo *inherited) {
    return static_cast<T*>(inherited);
}

auto is_integral = [](std::string_view tname) {
    std::array types_name = { "int" };
    return std::any_of(types_name.begin(), types_name.end(), [tname](const auto& type) {return type == tname;});
};
auto is_float = [](std::string_view tname) {
    std::array types_name = { "float", "double" };
    return std::any_of(types_name.begin(), types_name.end(), [tname](const auto& type) {return type == tname;});
};

struct SerializableGuiElement: GuiElement<void> {
    private:
    void setup_gui(YAML::Node root) {
        for ( YAML::iterator iter = root.begin(); iter != root.end(); ++iter) {
            if ( iter->second.Tag() != "doc") {
                const auto& real_gansta_type_name_fuk = iter->second.Tag();
                if ( is_integral(real_gansta_type_name_fuk)) elements.push_back(new IntGuiElement(iter));
                if ( is_float(real_gansta_type_name_fuk)) elements.push_back(new FloatGuiElement(iter));
                if ( real_gansta_type_name_fuk == "string") elements.push_back(new StringGuiElement(iter));
                if ( real_gansta_type_name_fuk == "bool") elements.push_back(new BoolGuiElement(iter));
                if ( real_gansta_type_name_fuk == "serializable") elements.push_back(new SerializableGuiElement(iter));
            }
        }
    }
    public:
    std::vector<YamlInfo*> elements;
    
    SerializableGuiElement (YAML::Node root)  {
        setup_gui(root);
    }
    
    SerializableGuiElement (YAML::iterator &node) : GuiElement(node) {
        setup_gui(this->node);
    }
    
    void makeGui() override {
        auto makeSimpleGui = [this]() {
            for (auto* el : elements) {
                if (is_integral(el->type_name)) {
                    auto* as_void = gui_cast<IntGuiElement>(el);
                    as_void->makeGui();
                }
                if (is_float(el->type_name)) {
                    auto* as_void = gui_cast<FloatGuiElement>(el);
                    as_void->makeGui();
                }
                if (el->type_name == "string") {
                    auto* as_void = gui_cast<StringGuiElement>(el);
                    as_void->makeGui();
                }
                if (el->type_name == "bool") {
                    auto* as_void = gui_cast<BoolGuiElement>(el);
                    as_void->makeGui();
                }
            }
        };

        if (this->name.empty()) {
            makeSimpleGui();
            for (auto* el : elements) {
                if (el->type_name == "serializable") {
                    auto* as_void = gui_cast<SerializableGuiElement>(el);
                    as_void->makeGui();
                }
            }
        }
        else {
            if (ImGui::TreeNode(this->name.data()))
            {
                makeSimpleGui();
                for (auto* el : elements) {
                    if (el->type_name == "serializable") {
                        auto* as_void = gui_cast<SerializableGuiElement>(el);
                        as_void->makeGui();
                    }
                }
                ImGui::TreePop();
            }
        }

        if(!this->doc.empty()) { ImGui::SameLine(); HelpMarker(this->doc.data()); }
    }
};

[[nodiscard]] int EditorWindow::loop()
{
    auto window = window_.get();
    ImVec4 clear_color = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    
    auto serializableGui = SerializableGuiElement(YAML::LoadFile("test.yml"));

    
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

//        ImGui::ShowDemoWindow();
        ImGui::SetNextWindowPos(ImVec2(50, 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 340), ImGuiCond_FirstUseEver);

        // Main body of the Demo window starts here.
        ImGui::Begin("Property - Simple Test class with scalar and basic sequnece");
        

        
        serializableGui.makeGui();
        //ImGui::ShowDemoWindow();

        ImGui::End();
        
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

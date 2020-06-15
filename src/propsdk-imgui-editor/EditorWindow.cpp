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
    YamlInfo(YAML::iterator node, YAML::iterator end) {
        name = node->first.as<std::string>();
        type_name = node->second.Tag();
        this->node = node->second;
        type = this->node.Type();

        YAML::iterator iter = node;
        ++iter;
        // Check we have doc tag
        if (iter != end) {
            if (iter->second.Tag() == "doc") {
                // we know after node data we put
                // <DOC> node so we just increment it
                ++node;
                doc = node->second.as<std::string>();
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
    GuiElement(YAML::iterator &node, YAML::iterator& end) : YamlInfo(node, end) {}
    virtual ~GuiElement() = default;
    
};


struct StringGuiElement: GuiElement<std::string> {
public:
    void makeGui() override {
        std::array<char, 128> buffer{};
        std::copy(value.begin(), value.end(), buffer.begin());
        if (ImGui::InputText((name).data(), buffer.data(), buffer.size())) {
            value = buffer.data();
            this->node = value;
            this->node.SetTag(this->type_name);
        } 
        if (!doc.empty()) {
            ImGui::SameLine(); HelpMarker(doc.data());
        }
        
    }
    StringGuiElement(YAML::iterator node, YAML::iterator end) : GuiElement(node, end) {
        try {
            value = this->node.as<type_value>();
        } catch (...) {
        }
    }
    ~StringGuiElement() override = default;
private:
    type_value value;
};

struct IntGuiElement: GuiElement<int> {
    void makeGui() override {
        if (ImGui::InputInt((name).data(), &value)) {
            this->node = value;
            this->node.SetTag(this->type_name);
        }
        if (!doc.empty()) {
            ImGui::SameLine(); HelpMarker(doc.data());
        }
    }
    IntGuiElement(YAML::iterator node, YAML::iterator end) : GuiElement(node, end) {
        try {
            value = this->node.as<type_value>();
        } catch (...) {
        }
    }
    virtual ~IntGuiElement() override = default;
private:
    type_value value;
};

struct FloatGuiElement: GuiElement<double> {
    void makeGui() override {
        if (ImGui::InputDouble((name).data(), &value)) {
            this->node = value;
            this->node.SetTag(this->type_name);
        } 
        if (!doc.empty()) {
            ImGui::SameLine(); HelpMarker(doc.data());
        }
    }
    FloatGuiElement(YAML::iterator node, YAML::iterator end) : GuiElement(node, end) {
        try {
            value = this->node.as<type_value>();
        } catch (...) {
        }
    }
    virtual ~FloatGuiElement() override = default;
private:
    type_value value;
};

struct BoolGuiElement: GuiElement<bool> {

    void makeGui() override {
        if (ImGui::Checkbox(( name ).data(), &value)) {
            this->node = value;
            this->node.SetTag(this->type_name);
        } 
        if (!doc.empty()) {
            ImGui::SameLine(); HelpMarker(doc.data());
        }
    }
    BoolGuiElement(YAML::iterator node, YAML::iterator end) : GuiElement(node, end) {
        try {
            value = this->node.as<type_value>();
        } catch (...) {
        }
    }
    virtual ~BoolGuiElement() override = default;
private:
    type_value value;
};

template <typename T>
T* gui_cast(YamlInfo *inherited) {
    return static_cast<T*>(inherited);
}

auto is_integral = [](std::string_view tname) {
  std::array types_name = { "int\0int32_t" };
    return std::any_of(types_name.begin(), types_name.end(), [tname](const auto& type) {return type == tname;});
};
auto is_float = [](std::string_view tname) {
    std::array types_name = { "float\0double" };
    return std::any_of(types_name.begin(), types_name.end(), [tname](const auto& type) {return type == tname;});
};

auto is_vector = []( std::string_view tname) {
    std::size_t current = tname.find_first_of(":");
    auto end = std::find_if(tname.begin(), tname.end(), [](const auto& symbol) {return symbol == ':'; });
    const auto result = std::string(tname.begin(), end);
    return result == "vector";
};

auto is_map = [](std::string_view tname) {
    std::size_t current = tname.find_first_of(":");
    auto end = std::find_if(tname.begin(), tname.end(), [](const auto& symbol) {return symbol == ':'; });
    const auto result = std::string(tname.begin(), end);
    return result == "map";
};

template<typename T>
struct Enumerator {

    using enumerator_type_name = std::vector<std::pair<size_t, T>>;

    enumerator_type_name elements;

    Enumerator(YAML::iterator begin, YAML::iterator end) {
        for (size_t i = 0; begin != end; ++begin, ++i) {
            elements.push_back(std::make_pair(i, begin->as<YAML::Node>()));
        }
    }

    auto begin() {
        return elements.begin();
    }

    auto end() {
        return elements.end();
    }
};

struct SerializableGuiElement: GuiElement<void> {
    private:
    using YamlInfoPtr = std::shared_ptr<YamlInfo>;
    using SerializableGuiElementPtr = std::shared_ptr<SerializableGuiElement>;
    struct VectorGuiElement : GuiElement<void> {
        void makeGui() override {
            if (ImGui::TreeNode(this->name.data()))
            {
                for (auto& el : elements) {
                    el->makeGui();
                }
                ImGui::TreePop();
            } else {
              if (!doc.empty()) {
                ImGui::SameLine(); HelpMarker(doc.data());
              }
            }

            
        }
        VectorGuiElement(YAML::iterator node, YAML::iterator end) : GuiElement(node, end) {
            auto get_real_type_name = [](const auto& stoke_type_name) {
                std::size_t current = stoke_type_name.find_first_of(":");
                auto begin = std::next(std::find_if(stoke_type_name.begin(), stoke_type_name.end(), [](const auto& symbol) {return symbol == ':'; }));
                return std::string(begin, stoke_type_name.end());
            };

            for (auto [index, elementNode] : Enumerator<YAML::Node>( this->node.begin(), this->node.end() )) {
                YAML::Node map_node;
                map_node[std::to_string(index)] = elementNode;
                map_node[std::to_string(index)].SetTag(get_real_type_name(this->type_name));
                elements.push_back(std::make_shared<SerializableGuiElement>(map_node));
            }
        }

        ~VectorGuiElement() override = default;
    private:
        std::vector<SerializableGuiElementPtr> elements;
    };

    struct MapGuiElement : GuiElement<void> {
        void makeGui() override {
            if (ImGui::TreeNode(this->name.data()))
            {
                element->makeGui();
                

                ImGui::TreePop();
            } else {
              if (!doc.empty()) {
                ImGui::SameLine(); HelpMarker(doc.data());
              }
            }
            
        }
        MapGuiElement(YAML::iterator node, YAML::iterator end) : GuiElement(node, end) {
            auto get_real_type_name = [](const auto& stoke_type_name) {
                std::size_t current = stoke_type_name.find_first_of(":");
                auto begin = std::next(std::find_if(stoke_type_name.begin(), stoke_type_name.end(), [](const auto& symbol) {return symbol == ':'; }));
                return std::string(begin, stoke_type_name.end());
            };
            const auto real_type_name = get_real_type_name(get_real_type_name(this->type_name));

            for (YAML::iterator iter = this->node.begin(); iter != this->node.end(); ++iter) {
                iter->second.SetTag(real_type_name);
            }


            element = std::make_shared<SerializableGuiElement>(this->node);
            
        }

        ~MapGuiElement() override = default;
    private:
        SerializableGuiElementPtr element;
    };
    
    void setup_gui(YAML::Node root) {

        if (root.IsMap()) {
            for (YAML::iterator iter = root.begin(); iter != root.end(); ++iter) {
                if (iter->second.Tag() != "doc") {
                    const auto& real_type_name = iter->second.Tag();
                    if (is_integral(real_type_name)) elements.push_back(std::make_shared<IntGuiElement>(iter, root.end()));
                    if (is_float(real_type_name)) elements.push_back(std::make_shared < FloatGuiElement>(iter, root.end()));
                    if (is_vector(real_type_name)) elements.push_back(std::make_shared < VectorGuiElement>(iter, root.end()));
                    if (is_map(real_type_name)) elements.push_back(std::make_shared < MapGuiElement>(iter, root.end()));
                    if (real_type_name == "string") elements.push_back(std::make_shared < StringGuiElement>(iter, root.end()));
                    if (real_type_name == "bool") elements.push_back(std::make_shared < BoolGuiElement>(iter, root.end()));
                    if (real_type_name == "serializable") elements.push_back(std::make_shared < SerializableGuiElement>(iter, root.end()));
                }
            }
        } 
    }
    public:
    std::vector<YamlInfoPtr> elements;
    
    SerializableGuiElement (YAML::Node root)  {
        setup_gui(root);
    }
    
    SerializableGuiElement (YAML::iterator node, YAML::iterator end) : GuiElement(node, end) {
        setup_gui(this->node);
    }
    
    void makeGui() override {
        auto makeGui = [this]() {
            for (auto el : elements) {
                if (is_integral(el->type_name)) {
                    auto* as_void = gui_cast<IntGuiElement>(el.get());
                    as_void->makeGui();
                }
                if (is_float(el->type_name)) {
                    auto* as_void = gui_cast<FloatGuiElement>(el.get());
                    as_void->makeGui();
                }
                if (el->type_name == "string") {
                    auto* as_void = gui_cast<StringGuiElement>(el.get());
                    as_void->makeGui();
                }
                if (el->type_name == "bool") {
                    auto* as_void = gui_cast<BoolGuiElement>(el.get());
                    as_void->makeGui();
                }
                if (el->type_name == "serializable") {
                    auto* as_void = gui_cast<SerializableGuiElement>(el.get());
                    as_void->makeGui();
                }
                if (is_vector(el->type_name)) {
                    auto* as_void = gui_cast<VectorGuiElement>(el.get());
                    as_void->makeGui();
                }
                if (is_map(el->type_name)) {
                    auto* as_void = gui_cast<MapGuiElement>(el.get());
                    as_void->makeGui();
                }
            }
        };

        if (this->name.empty()) {
            makeGui();
        }
        else {
            if (ImGui::TreeNode(this->name.data()))
            {
                makeGui();
                ImGui::TreePop();
            }
            else {

              if (!this->doc.empty()) { ImGui::SameLine(); HelpMarker(this->doc.data()); }
            }
        }

    }
};



[[nodiscard]] int EditorWindow::loop()
{
    auto window = window_.get();
    ImVec4 clear_color = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    auto root = YAML::LoadFile("test.yml");
    auto serializableGui = SerializableGuiElement(root);

    
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

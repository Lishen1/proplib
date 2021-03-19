#ifndef EDITOR_GUI_HPP
#define EDITOR_GUI_HPP

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <array>
#include <vector>

namespace editor::gui {

static void HelpMarker(const char *desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

struct YamlInfo {
  std::string name;
  std::string type_name;
  std::string doc;
  YAML::NodeType::value type;

  YamlInfo() = default;
  YamlInfo(YAML::iterator node, YAML::iterator end) {
    this->name = node->first.as<std::string>();
    this->type_name = node->second.Tag();
    this->node = node->second;
    this->type = this->node.Type();
    ++node;
    if (node != end) {
      if (node->second.Tag() == "doc") {
        // we know after node data we put
        // <DOC> node so we just increment it
        this->doc = node->second.as<std::string>();
      }
    }
  }

protected:
  YAML::Node node;
};

template <typename T> struct GuiElement : YamlInfo {
  using type_value = T;
  using iterator = type_value *;
  using const_iterator = const iterator;

  GuiElement() = default;
  GuiElement(YAML::iterator &node, YAML::iterator &end) : YamlInfo(node, end) {}
  virtual ~GuiElement() = default;

  virtual void makeGui() {}
};
struct StringGuiElement : GuiElement<std::string> {
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
      ImGui::SameLine();
      HelpMarker(doc.data());
    }
  }
  StringGuiElement(YAML::iterator node, YAML::iterator end)
      : GuiElement(node, end) {
    try {
      value = this->node.as<type_value>();
    } catch (...) {
    }
  }
  ~StringGuiElement() override = default;

private:
  type_value value;
};

struct IntGuiElement : GuiElement<int> {
  void makeGui() override {
    if (ImGui::InputInt((name).data(), &value)) {
      this->node = value;
      this->node.SetTag(this->type_name);
    }
    if (!doc.empty()) {
      ImGui::SameLine();
      HelpMarker(doc.data());
    }
  }
  IntGuiElement(YAML::iterator node, YAML::iterator end)
      : GuiElement(node, end) {
    try {
      value = this->node.as<type_value>();
    } catch (...) {
    }
  }
  virtual ~IntGuiElement() override = default;

private:
  type_value value;
};

struct FloatGuiElement : GuiElement<double> {
  void makeGui() override {
    if (ImGui::InputDouble((name).data(), &value)) {
      this->node = value;
      this->node.SetTag(this->type_name);
    }
    if (!doc.empty()) {
      ImGui::SameLine();
      HelpMarker(doc.data());
    }
  }
  FloatGuiElement(YAML::iterator node, YAML::iterator end)
      : GuiElement(node, end) {
    try {
      value = this->node.as<type_value>();
    } catch (...) {
    }
  }
  virtual ~FloatGuiElement() override = default;

private:
  type_value value;
};

struct BoolGuiElement : GuiElement<bool> {

  void makeGui() override {
    if (ImGui::Checkbox((name).data(), &value)) {
      this->node = value;
      this->node.SetTag(this->type_name);
    }
    if (!doc.empty()) {
      ImGui::SameLine();
      HelpMarker(doc.data());
    }
  }
  BoolGuiElement(YAML::iterator node, YAML::iterator end)
      : GuiElement(node, end) {
    try {
      value = this->node.as<type_value>();
    } catch (...) {
    }
  }
  virtual ~BoolGuiElement() override = default;

private:
  type_value value;
};

template <typename T> T *gui_cast(YamlInfo *inherited) {
  return static_cast<T *>(inherited);
}

auto is_integral = [](std::string_view tname) {
  std::array<std::string_view, 2> types_name = {"int", "int32_t"};
  return std::any_of(types_name.begin(), types_name.end(),
                     [tname](const auto &type) { return type == tname; });
};
auto is_float = [](std::string_view tname) {
  std::array<std::string_view, 2> types_name = {"float", "double"};
  return std::any_of(types_name.begin(), types_name.end(),
                     [tname](const auto &type) { return type == tname; });
};

auto is_vector = [](std::string_view tname) {
  std::size_t current = tname.find_first_of(":");
  auto end = std::find_if(tname.begin(), tname.end(),
                          [](const auto &symbol) { return symbol == ':'; });
  const auto result = std::string(tname.begin(), end);
  return result == "vector";
};

auto is_map = [](std::string_view tname) {
  std::size_t current = tname.find_first_of(":");
  auto end = std::find_if(tname.begin(), tname.end(),
                          [](const auto &symbol) { return symbol == ':'; });
  const auto result = std::string(tname.begin(), end);
  return result == "map";
};

template <typename T> struct Enumerator {

  using enumerator_type_name = std::vector<std::pair<size_t, T>>;

  enumerator_type_name elements;

  Enumerator(YAML::iterator begin, YAML::iterator end) {
    for (size_t i = 0; begin != end; ++begin, ++i) {
      elements.push_back(std::make_pair(i, begin->as<YAML::Node>()));
    }
  }

  auto begin() { return elements.begin(); }

  auto end() { return elements.end(); }
};
struct SerializableGuiElement;
using SerializableGuiElementPtr = std::shared_ptr<SerializableGuiElement>;
struct SerializableGuiElement : GuiElement<void> {
private:
  using YamlInfoPtr = std::shared_ptr<YamlInfo>;
  struct VectorGuiElement : GuiElement<void> {
    void makeGui() override {
      if (ImGui::TreeNode(this->name.data())) {
        for (auto &el : elements) {
          el->makeGui();
        }
        ImGui::TreePop();
      } else {
        if (!doc.empty()) {
          ImGui::SameLine();
          HelpMarker(doc.data());
        }
      }
    }
    VectorGuiElement(YAML::iterator node, YAML::iterator end)
        : GuiElement(node, end) {
      auto get_real_type_name = [](const auto &stoke_type_name) {
        std::size_t current = stoke_type_name.find_first_of(":");
        auto begin = std::next(
            std::find_if(stoke_type_name.begin(), stoke_type_name.end(),
                         [](const auto &symbol) { return symbol == ':'; }));
        return std::string(begin, stoke_type_name.end());
      };

      for (auto [index, elementNode] :
           Enumerator<YAML::Node>(this->node.begin(), this->node.end())) {
        YAML::Node map_node;
        map_node[std::to_string(index)] = elementNode;
        map_node[std::to_string(index)].SetTag(
            get_real_type_name(this->type_name));
        elements.push_back(std::make_shared<SerializableGuiElement>(map_node));
      }
    }

    ~VectorGuiElement() override = default;

  private:
    std::vector<SerializableGuiElementPtr> elements;
  };

  struct MapGuiElement : GuiElement<void> {
    void makeGui() override {
      if (ImGui::TreeNode(this->name.data())) {
        element->makeGui();

        ImGui::TreePop();
      } else {
        if (!doc.empty()) {
          ImGui::SameLine();
          HelpMarker(doc.data());
        }
      }
    }
    MapGuiElement(YAML::iterator node, YAML::iterator end)
        : GuiElement(node, end) {
      auto get_real_type_name = [](const auto &stoke_type_name) {
        std::size_t current = stoke_type_name.find_first_of(":");
        auto begin = std::next(
            std::find_if(stoke_type_name.begin(), stoke_type_name.end(),
                         [](const auto &symbol) { return symbol == ':'; }));
        return std::string(begin, stoke_type_name.end());
      };
      const auto real_type_name =
          get_real_type_name(get_real_type_name(this->type_name));

      for (YAML::iterator iter = this->node.begin(); iter != this->node.end();
           ++iter) {
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
          const auto &real_type_name = iter->second.Tag();
          if (is_integral(real_type_name))
            elements.push_back(
                std::make_shared<IntGuiElement>(iter, root.end()));
          if (is_float(real_type_name))
            elements.push_back(
                std::make_shared<FloatGuiElement>(iter, root.end()));
          if (is_vector(real_type_name))
            elements.push_back(
                std::make_shared<VectorGuiElement>(iter, root.end()));
          if (is_map(real_type_name))
            elements.push_back(
                std::make_shared<MapGuiElement>(iter, root.end()));
          if (real_type_name == "string")
            elements.push_back(
                std::make_shared<StringGuiElement>(iter, root.end()));
          if (real_type_name == "bool")
            elements.push_back(
                std::make_shared<BoolGuiElement>(iter, root.end()));
          if (real_type_name == "serializable")
            elements.push_back(
                std::make_shared<SerializableGuiElement>(iter, root.end()));
        }
      }
    }
  }

public:
  std::vector<YamlInfoPtr> elements;

  SerializableGuiElement(YAML::Node root) { setup_gui(root); }

  SerializableGuiElement(YAML::iterator node, YAML::iterator end)
      : GuiElement(node, end) {
    setup_gui(this->node);
  }

  void makeGui() override {
    auto makeGui = [this]() {
      for (auto el : elements) {
        if (is_integral(el->type_name)) {
          auto *as_void = gui_cast<IntGuiElement>(el.get());
          as_void->makeGui();
        }
        if (is_float(el->type_name)) {
          auto *as_void = gui_cast<FloatGuiElement>(el.get());
          as_void->makeGui();
        }
        if (el->type_name == "string") {
          auto *as_void = gui_cast<StringGuiElement>(el.get());
          as_void->makeGui();
        }
        if (el->type_name == "bool") {
          auto *as_void = gui_cast<BoolGuiElement>(el.get());
          as_void->makeGui();
        }
        if (el->type_name == "serializable") {
          auto *as_void = gui_cast<SerializableGuiElement>(el.get());
          as_void->makeGui();
        }
        if (is_vector(el->type_name)) {
          auto *as_void = gui_cast<VectorGuiElement>(el.get());
          as_void->makeGui();
        }
        if (is_map(el->type_name)) {
          auto *as_void = gui_cast<MapGuiElement>(el.get());
          as_void->makeGui();
        }
      }
    };

    if (this->name.empty()) {
      makeGui();
    } else {
      if (ImGui::TreeNode(this->name.data())) {
        makeGui();
        ImGui::TreePop();
      } else {

        if (!this->doc.empty()) {
          ImGui::SameLine();
          HelpMarker(this->doc.data());
        }
      }
    }
  }
};
} // namespace editor::gui
#endif // EDITOR_GUI_HPP

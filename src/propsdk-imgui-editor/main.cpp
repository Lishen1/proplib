#include <editor/EditorWindow.hpp>

int main(int, char **)
{
    editor::EditorWindow window;
    
    if (window.setup_window() ) {
        return window.error_code();
    }
    return window.loop();
}

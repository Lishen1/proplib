#include <editor/EditorWindow.hpp>

//#include <serialize/proplib.h>
//#include <easylogging++.h>
//INITIALIZE_EASYLOGGINGPP

int main(int, char **)
{
    editor::EditorWindow window;
    
    if (window.setup_window() ) {
        return window.error_code();
    }
    return window.loop();
}

#include "sdl_listener.h"

#include <vector>

std::vector<ISDLEventListener*>& GetEventListeners()
{
    static std::vector<ISDLEventListener*> g_eventListeners;
    return g_eventListeners;
}

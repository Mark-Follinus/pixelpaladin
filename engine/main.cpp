#define PLDN_BUILD_MODE 1

#if PLDN_BUILD_MODE == 0
#include "core/standalone/engine.hpp"
#elif PLDN_BUILD_MODE == 1
#include "core/client/client_engine.hpp"
#elif PLDN_BUILD_MODE == 2
#include "core/server/server_engine.hpp"
#else
#error Invalid PLDN_BUILD_MODE
#endif

using namespace pldn;	

int main()
{
#if PLDN_BUILD_MODE == 0
    pldn::Engine engine;
#elif PLDN_BUILD_MODE == 1
    pldn::ClientEngine engine;
#elif PLDN_BUILD_MODE == 2
    pldn::ServerEngine engine;
#endif
    engine.Run();
}
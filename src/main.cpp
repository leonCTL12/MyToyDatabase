#include <iostream>
#include <string>
#include "sql_interpreter/sql_interpreter.h"
#include <boost/di.hpp>
#include "app.h"
#include "database_command_handler/database_command_handler.h"
#include "persistent_storage/persistent_storage.h"
#include "path_provider/path_provider.h"

namespace di = boost::di;

int main()
{
    auto injection = boost::di::make_injector(
        di::bind<DatabaseCommandHandler>().to<DatabaseCommandHandler>().in(di::singleton),
        di::bind<ISQLInterpreter>().to<SQLInterpreter>().in(di::singleton),
        di::bind<PathProvider>().to<PathProvider>().in(di::singleton),
        di::bind<PersistentStorage>().to<PersistentStorage>().in(di::singleton));
    auto app = injection.create<App>();
    app.run();
    return 0;
}
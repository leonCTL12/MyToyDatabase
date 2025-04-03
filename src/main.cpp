#include <iostream>
#include <string>
#include "sql_interpreter/sql_interpreter.h"
#include <boost/di.hpp>
#include "app.h"
#include "database_internal/database_command_handler.h"

namespace di = boost::di;

int main()
{
    auto injection = boost::di::make_injector(
        di::bind<DatabaseCommandHandler>().to<DatabaseCommandHandler>().in(di::singleton),
        di::bind<SQLInterpreter>().to<SQLInterpreter>().in(di::singleton));
    auto app = injection.create<App>();
    app.run();
    return 0;
}
#include <iostream>
#include <string>
#include "input_interpreter/input_interpreter.h"
#include <boost/di.hpp>
#include "app.h"
#include "input_interpreter/input_interpreter.h"

namespace di = boost::di;

int main()
{
    auto injection = boost::di::make_injector(
        di::bind<InputInterpreter>().to<InputInterpreter>().in(di::singleton));

    auto app = injection.create<App>();
    app.run();
    return 0;
}
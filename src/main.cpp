#include <iostream>
#include <string>
#include "key_value_store_internal/key_value_store.h"
#include <boost/di.hpp>
#include "app/app.h"
#include "app/input_interpreter/input_interpreter.h"

namespace di = boost::di;

int main()
{
    auto injection = boost::di::make_injector(
        di::bind<InputInterpreter>().to<InputInterpreter>().in(di::singleton),
        di::bind<KeyValueStore>().to<KeyValueStore>().in(di::singleton));

    auto app = injection.create<App>();
    app.run();
    return 0;
}
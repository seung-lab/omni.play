#include "zargs.h"

#include <list>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>

namespace zi { namespace Args {

ZiArguments* ZiArguments::args_ = 0;

ZiArguments& ZiArguments::get() {
  if (args_ == NULL) {
    args_ = new ZiArguments();
  }
  return *(args_);
}

bool ZiArguments::parseArgs(int &argc, char **argv, bool removeArgs) {
  fileName_ = std::string(argv[0]);
  std::list<std::string> q;
  for (int i=1; i<argc; ++i)
    q.push_back(std::string(argv[i]));

  std::vector<ZiArgHandler*> handlers_;

  FOR_EACH (it, handlerFactories_) {
    handlers_.push_back((*it)->getHandler());
  }

  FOR_EACH (it, q) {
    if (*it == "-h" || *it == "--help" || *it == "-help") {
      std::cout << "Usage: " << fileName_ << " [OPTIONS] ...\n\n";
      FOR_EACH (it, handlers_) {
        std::cout.width(29);
        std::cout << std::left
                  << ("  --" + (*it)->getName() + "=" + (*it)->getType());
        std::string desc = (*it)->getDescription();
        int snPos = 0;
        std::vector<std::string> lines;
        explode(lines, desc, '\n');
        FOR_EACH (lit, lines)
          std::cout << (*lit) << "\n" << std::string(29, ' ');
        std::cout << "default=" << (*it)->getDefault() << "\n";
        std::cout << "\n";
      }

      std::cout << "Notes:\n"
                << "  - For non-BOOLEAN types:\n"
                << "      -key VALUE (is equivalent to --key=VALUE)\n"
                << "      -key=VALUE (is equivalent to --key=VALUE)\n"
                << "  - For BOOLEAN types:\n"
                << "      -key   (is equivalent to --key=true)\n"
                << "      -nokey (is equivalent to --key=false)\n\n\n";

      return false;
    }
  }

  if (removeArgs) {
    argc = 1;
  }

  while (!q.empty()) {
    bool found = false;
    FOR_EACH (it, handlers_) {
      try {
        if ((*it)->parse(q)) {
          found = true;
          break;
        }
      } catch (ArgsException *e) {
        std::cout << "Error: " << e->what() << "\n";
        exit(1);
      }
    }

    if ((!found) && removeArgs) {
      char *arg = new char[q.front().size() + 1];
      strcpy(arg, q.front().c_str());
      argv[argc++] = arg;
    }
    q.pop_front();
  }

  FOR_EACH (it, handlers_)
    delete (*it);

  FOR_EACH (it, handlerFactories_)
    delete (*it);

  handlerFactories_.clear();

  return true;

}

void ZiArguments::ParseArgs(int &argc, char **argv, bool removeArgs) {
  if (!ZiArguments::get().parseArgs(argc, argv, removeArgs))
    exit(0);
}

} }

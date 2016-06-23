#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "was_driver.hpp"
#include "ti.hpp"

int main(const int argc, const char **argv) {
  if (argc < 2)
    return EXIT_FAILURE;

  WAS::WAS_Driver driver;
  int debug_level = 0;
  int print = 0;
  int inferTypes = 0;
  std::vector <std::string> sources;
  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "--debug") {
      debug_level = 1;
    } else if (std::string(argv[i]) == "--print") {
      print = 1;
    } else if (std::string(argv[i]) == "--infer-types") {
      inferTypes = 1;
    } else if (std::string(argv[i]) == "--infer-types-td") {
      inferTypes = 2;
    } else {
      sources.push_back(std::string(argv[i]));
    }
  }

  for (std::string &source : sources) {
    driver.parse(source.c_str(), debug_level);

    if (inferTypes)
      TI::infer_types(driver.result, inferTypes == 2);
    if (print) {
      driver.result->print(std::cout);
      std::cout << std::endl;
      // TODO free resources
      driver.result = nullptr;
    }
  }

  return EXIT_SUCCESS;
}

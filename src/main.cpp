#include "ft_irc.hpp"


int main(int argc, char** argv) {
  try {
    validator.validate(argc);
    validator.validate(argv[1]);

    int port = std::atoi(argv[1]);
    const std::string pass = argv[2];

    Server irc_server(port, pass);
    irc_server.start();
  }
  catch (const std::exception& e) {
    logger.fatal(e.what());
    return EXIT_ERR;
  }

  return EXIT_OK;
}

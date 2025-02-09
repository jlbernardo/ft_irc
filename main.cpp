#include "ft_irc.h"


int main(int argc, char** argv) {
  try {
    validate.args(argc);
    validate.port(argv[1]);
    validate.pass(argv[2]);

    int port = std::atoi(argv[1]);
    const std::string pass = argv[2];

    Server irc_server(port, pass);
    irc_server.start();
  }
  catch (const std::exception& e) {
    log.fatal(e.what());
    return EXIT_ERR;
  }

  return EXIT_OK;
}

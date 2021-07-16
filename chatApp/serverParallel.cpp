#include "serverParallel.hpp"

int main(int argc, char **argv) {

  unsigned short port_num = 3333;
  try {
    Server srv;
    srv.Start(port_num);
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // srv.Stop();
  } catch (boost::system::system_error &e) {
    std::cerr << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what();
  }
  return 0;
}
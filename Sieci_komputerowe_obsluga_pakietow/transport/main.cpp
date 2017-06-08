// Kacper Kulczak 279079
#include "part.h"
#include "window.h"


int main(int args, char const *argv[])
{
  if(args != 4)
  {
    std::cout << "Wrong input!   ./transport port file_name bytes\n";
    return 1;
  }
  size_t rozmiar, port;
  sscanf(argv[3], "%zu", &rozmiar);
  sscanf(argv[1], "%zu", &port );
  Window win(rozmiar);
  try
  {
    win.init(port, std::string(argv[2]) );
    win.download_file();
  }
  catch(...)
  {
    std::cerr << "error_ocured\n";
  }
  return 0;
}

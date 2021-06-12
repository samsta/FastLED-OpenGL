#include <fstream>
#include <iostream>
#include <iomanip>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
   if (argc != 4)
   {
      std::cerr << "usage: " << argv[0] << " <in_file> <out_dir> <blob_name>" << std::endl;
      std::cerr << std::endl;
      std::cerr << "convert <in_file> to source code representing a blob name blobs::<blob_name>" << std::endl;
      std::cerr << "and write the result to <out_dir>/<blob_name>.hpp" << std::endl;
      return 1;
   }
   const char* in_file_name = argv[1];
   const char* out_dir_name = argv[2];
   const char* blob_name = argv[3];

   std::ifstream in_file(in_file_name);
   if (not in_file.good())
   {
      std::cerr << "could not open input file " << in_file_name << ":" << strerror(errno) << std::endl;
      return 1;
   }

   std::string out_file_name = std::string(out_dir_name) + "/" + blob_name + ".hpp";
   std::ofstream out_file(out_file_name.c_str());
   if (not out_file.good())
   {
      std::cerr << "could not open output file " << out_file_name << ":" << strerror(errno) << std::endl;
      return 1;
   }

   out_file << "namespace blobs { " << std::endl
            << "const unsigned char " << blob_name << "[] = {";

   char c;
   unsigned count = 0;
   while (in_file.good() and out_file.good())
   {
      in_file.read(&c, sizeof(c));
      if (in_file.eof()) break;
      if (count > 0) out_file << ", "; 
      if (count % 16 == 0) out_file << std::endl << "   ";
      out_file << std::hex << "0x" << std::setw(2) << std::setfill('0') << (int(c) & 0xFF);
      count++;
   }

   if (in_file.bad() or out_file.bad())
   {
      std::cerr << "Error reading/writing: " << strerror(errno);
      out_file.close();
      remove(out_file_name.c_str());
      return 1;
   }
   
   out_file << std::endl
            << "   };" << std::endl
            << "}" << std::endl;
   
   return 0;
}

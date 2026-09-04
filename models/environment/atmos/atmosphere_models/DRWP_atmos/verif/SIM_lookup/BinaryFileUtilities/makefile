all : ReadBinFile WriteBinFile ReadEntireBinFile

ReadBinFile : read_bin_file.cc
		g++ -g -std=c++17 -Wall -Wextra -Werror -o ReadBinFile read_bin_file.cc -I.

ReadEntireBinFile : read_entire_bin_file.cc
		g++ -g -std=c++17 -Wall -Wextra -Werror -o ReadEntireBinFile read_entire_bin_file.cc -I.

WriteBinFile : write_bin_file.cc
		g++ -g -std=c++17 -Wall -Wextra -Werror -o WriteBinFile write_bin_file.cc -I.

zap :
		rm ReadBinFile ReadEntireBinFile WriteBinFile *.o

all : ReadBinFile WriteBinFile ReadEntireBinFile

ReadBinFile : ReadBinFile.cpp
		g++ -g -std=c++17 -Wall -Wextra -Werror -o ReadBinFile ReadBinFile.cpp -I.

ReadEntireBinFile : ReadEntireBinFile.cpp
		g++ -g -std=c++17 -Wall -Wextra -Werror -o ReadEntireBinFile ReadEntireBinFile.cpp -I.

WriteBinFile : WriteBinFile.cpp
		g++ -g -std=c++17 -Wall -Wextra -Werror -o WriteBinFile WriteBinFile.cpp -I.

zap :
		rm ReadBinFile ReadEntireBinFile WriteBinFile *.o

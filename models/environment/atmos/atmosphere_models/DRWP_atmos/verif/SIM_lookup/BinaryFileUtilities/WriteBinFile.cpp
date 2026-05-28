#include <fstream>
#include <limits>
#include <iostream>

int main() {

  std::ofstream BinFile;
  std::string filename;
  std::cout.precision(std::numeric_limits<double>::digits10);

  filename = "DRWP_6dependentvariables.bin";
  int num_variables = 6;
  int nProfiles = 3;
  int nAltitudes = 5;

  float lu_alt_array[1000] = {1000, 2000, 3000, 4000, 5000};
  float lu_u_array[1000] = {1, 2, 3, 4, 5, 11, 12, 13, 14, 15, 21, 22, 23, 24, 25};
  float lu_v_array[1000] = {-1, -2, -3, -4, -5, -11, -12, -13, -14, -15, -21, -22, -23, -24, -25};
  float lu_w_array[1000] = {71, 72, 73, 74, 75, 81, 82, 83, 84, 85, 91, 92, 93, 94, 95};

  float lu_temp_array[1000] = {10, 20, 30, 40, 50, 110, 120, 130, 140, 150, 210, 220, 230, 240, 250};
  float lu_rho_array[1000] = {0.10, 0.20, 0.30, 0.40, 0.50, 0.11, 0.12, 0.13, 0.14, 0.15, 0.21, 0.22, 0.23, 0.24, 0.25};
  float lu_press_array[1000] = {1000, 2000, 3000, 4000, 5000, 11000, 12000, 13000, 14000, 15000, 21000, 22000, 23000, 24000, 25000};

  int fileWindProfiles[10] = {38, 83, 55};

  BinFile.open(filename);
  if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
  BinFile.write(  (char *) &nProfiles, sizeof(nProfiles) );
  BinFile.write(  (char *) &nAltitudes, sizeof(nAltitudes) );
  BinFile.write( (char *) &lu_alt_array, nAltitudes*sizeof(lu_alt_array[0]) );
  BinFile.write( (char *) &fileWindProfiles, nProfiles*sizeof(fileWindProfiles[0]) );

  for (int loop=0; loop<nProfiles; loop++) {
    BinFile.write( (char *) &fileWindProfiles[loop],
                   sizeof(fileWindProfiles[0]) );
    BinFile.write( (char *) &(lu_u_array[loop * nAltitudes]),
                   nAltitudes*sizeof(lu_u_array[0]));
    BinFile.write( (char *) &(lu_v_array[loop * nAltitudes]),
                   nAltitudes*sizeof(lu_v_array[0]));
    // NOTE: remove next write for an equivalent 5-variable set
    BinFile.write( (char *) &(lu_w_array[loop * nAltitudes]),
                    nAltitudes*sizeof(lu_w_array[0]));
    BinFile.write( (char *) &(lu_temp_array[loop * nAltitudes]),
                   nAltitudes*sizeof(lu_temp_array[0]) );
    BinFile.write( (char *) &(lu_rho_array[loop * nAltitudes]),
                   nAltitudes*sizeof(lu_rho_array[0]) );
    BinFile.write( (char *) &(lu_press_array[loop * nAltitudes]),
                   nAltitudes*sizeof(lu_press_array[0]) );
  }

  BinFile.close();

  return 0;
}

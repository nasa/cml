#include <fstream>
#include <limits>
#include <iostream>
#include <cstdint>

int main() {
    std::ofstream BinFile;
    std::string filename;
    std::cout.precision(std::numeric_limits<double>::digits10);

    int32_t nProfiles = 3;
    int32_t nAltitudes = 5;

    float alt_array[5] = {1000, 2000, 3000, 4000, 5000};
    float u_array[15] = {1, 2, 3, 4, 5, 11, 12, 13, 14, 15, 21, 22, 23, 24, 25};
    float v_array[15] = {-1, -2, -3, -4, -5, -11, -12, -13, -14, -15, -21, -22, -23, -24, -25};
    float w_array[15] = {71, 72, 73, 74, 75, 81, 82, 83, 84, 85, 91, 92, 93, 94, 95};

    float temp_array[15] = {10, 20, 30, 40, 50, 110, 120, 130, 140, 150, 210, 220, 230, 240, 250};
    float rho_array[15] = {0.10, 0.20, 0.30, 0.40, 0.50, 0.11, 0.12, 0.13, 0.14, 0.15, 0.21, 0.22, 0.23, 0.24, 0.25};
    float press_array[15] = {1000, 2000, 3000, 4000, 5000, 11000, 12000, 13000, 14000, 15000, 21000, 22000, 23000, 24000, 25000};

    int32_t fileWindProfiles[3] = {11, 22, 33};

    filename = "no_number_of_profiles.bin";
    BinFile.open(filename);
    if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
    BinFile.close();

    filename = "invalid_number_of_profiles.bin";
    BinFile.open(filename);
    if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
    int32_t nProfiles_ = 12345678;
    BinFile.write(  (char *) &nProfiles_, 4 );
    BinFile.close();

    filename = "no_number_of_altitudes.bin";
    BinFile.open(filename);
    if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
    BinFile.write(  (char *) &nProfiles, 4 );
    BinFile.close();

    filename = "invalid_number_of_altitudes.bin";
    BinFile.open(filename);
    if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
    BinFile.write(  (char *) &nProfiles, 4 );
    int nAltitudes_ = 12345678;
    BinFile.write(  (char *) &nAltitudes_, 4 );
    BinFile.close();

    filename = "short_altitude_array.bin";
    BinFile.open(filename);
    if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
    BinFile.write(  (char *) &nProfiles, 4 );
    BinFile.write(  (char *) &nAltitudes, 4 );
    BinFile.write( (char *) &alt_array, 16 );
    BinFile.close();

    filename = "invalid_altitude_array.bin";
    BinFile.open(filename);
    if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
    BinFile.write(  (char *) &nProfiles, 4 );
    BinFile.write(  (char *) &nAltitudes, 4 );
    float alt_array_[5] = {1000, 2000, 1500, 1900, 1300};
    BinFile.write( (char *) &alt_array_, 20 );
    BinFile.close();

    filename = "short_profile_array.bin";
    BinFile.open(filename);
    if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
    BinFile.write(  (char *) &nProfiles, 4 );
    BinFile.write(  (char *) &nAltitudes, 4 );
    BinFile.write( (char *) &alt_array, 20 );
    BinFile.write( (char *) &fileWindProfiles, 8 );
    BinFile.close();

    filename = "no_profile_number.bin";
    BinFile.open(filename);
    if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
    BinFile.write(  (char *) &nProfiles, 4 );
    BinFile.write(  (char *) &nAltitudes, 4 );
    BinFile.write( (char *) &alt_array, 20 );
    BinFile.write( (char *) &fileWindProfiles, 12 );
    BinFile.close();

    filename = "invalid_profile_number.bin";
    BinFile.open(filename);
    if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
    BinFile.write(  (char *) &nProfiles, 4 );
    BinFile.write(  (char *) &nAltitudes, 4 );
    BinFile.write( (char *) &alt_array, 20 );
    BinFile.write( (char *) &fileWindProfiles, 12 );
    int32_t invalid_prof = 2;
    BinFile.write( (char *) &invalid_prof, 4 );
    BinFile.close();


    filename = "incorrect_filelength.bin";
    BinFile.open(filename);
    if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
    BinFile.write(  (char *) &nProfiles, 4 );
    BinFile.write(  (char *) &nAltitudes, 4 );
    BinFile.write( (char *) &alt_array, 20 );
    BinFile.write( (char *) &fileWindProfiles, 12 );
    for (int ii=0; ii<nProfiles; ii++) {
      int arr_ix = ii * nAltitudes;
      int write_size = 4*nAltitudes;
      BinFile.write( (char *) &fileWindProfiles[ii], 4 );
      BinFile.write( (char *) &(u_array[arr_ix]), write_size);
      BinFile.write( (char *) &(v_array[arr_ix]), write_size);
      BinFile.write( (char *) &(w_array[arr_ix]), write_size);
      BinFile.write( (char *) &(temp_array[arr_ix]),  write_size );
      BinFile.write( (char *) &(rho_array[arr_ix]),   write_size );
      BinFile.write( (char *) &(press_array[arr_ix]), write_size );
    }
    BinFile.write(  (char *) &nProfiles, 4 );
    BinFile.close();

    return 0;
}

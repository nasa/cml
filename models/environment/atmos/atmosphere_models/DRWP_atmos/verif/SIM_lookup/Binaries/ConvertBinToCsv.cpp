#include <fstream>
#include <iostream>
#include <cstdint> // uint32_t


void parse_binary( std::string bin_name,
                   size_t num_vars,
                   size_t profile,
                   std::string csv_name)
{
  std::ifstream BinFile;
  BinFile.open( bin_name);


  int32_t nProfiles;
  BinFile.read(  (char *) &nProfiles, 4);
  std::cout << "nProfiles = " << nProfiles << "\n";

  int32_t nAltitudes;
  BinFile.read(  (char *) &nAltitudes, sizeof(nAltitudes) );
  std::cout << "nAltitudes = " << nAltitudes << "\n";

  float alt_array[nAltitudes];
  BinFile.read( (char *) &alt_array, nAltitudes*4 );

  int32_t profiles[nProfiles];
  BinFile.read( (char *) &profiles, nProfiles* 4);
  std::cout << "Profiles = {";
  for  (int ii = 0; ii<nProfiles; ii++) { std::cout << profiles[ii] << ",  ";}
  std::cout << "}\n";

  size_t profile_ix;
  for (profile_ix = 0; profile_ix < nProfiles; profile_ix++) {
    if ( profile == profiles[profile_ix]) {
      std::cout << "Profile " << profile << " at index " << profile_ix << "\n";
      break;
    }
  }
  if (profile_ix == nProfiles) {
    std::cout << "FAILED " << __LINE__ << "\n";
    return;
  }

  int pointsPerBlock = nAltitudes * num_vars;
  int bytesPerBlock  = 4 * pointsPerBlock;
  int bytesPerProfile = bytesPerBlock + 4;

  BinFile.seekg( profile_ix * bytesPerProfile, std::ios::cur);

  int32_t profile_read;
  BinFile.read(  (char *) &profile_read, 4 );
  std::cout << "profile_read = " << profile_read << "\n";
  if (profile_read != profile) {
    std::cout << "FAILED " << __LINE__ << "\n";
    return;
  }

  float data[pointsPerBlock];
  BinFile.read((char *) data, bytesPerBlock);

  std::ofstream CsvFile;
  CsvFile.open( csv_name);

  for (int ii = 0; ii < nAltitudes; ++ii) {
    CsvFile << alt_array[ii];
    for (int jj = 0; jj < num_vars; ++jj) {
      CsvFile << ",  " << data[ jj*nAltitudes + ii];
    }
    CsvFile << "\n";
  }
  BinFile.close();
  CsvFile.close();
}




int main() {

  // This is a slimmed down version of LookupAtmosWinds::load_DRWP_file
  // intended as a single-use application to extract data from the binary
  // files used as verification tests, and export those data as csv files for
  // comparison. In the binary profile, the data is stored as a single string
  // {all-u-values},{all-v-values}, etc.
  // In the csv file, we convert that to tabulated with format
  //     alt_0   u_0    v_0  etc
  //     alt_1   u_1    v_1  etc.
  //     etc.

  parse_binary( "DRWP_w_comp.bin", 6, 2, "DRWP_w_comp__prof_2.csv");
  parse_binary( "DRWP_no_w_comp.bin", 5, 1771, "DRWP_no_w_comp__prof_1771.csv");
  return 0;
}

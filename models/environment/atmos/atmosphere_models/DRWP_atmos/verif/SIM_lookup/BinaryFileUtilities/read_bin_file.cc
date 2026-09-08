/* To modify the wind-profile options, modify the call the main_sub at the end
 * of this file.
 * */

#include <fstream>

int main_sub( int option,
              int wind_profile)
{
  std::ifstream BinFile;
  int nProfiles;
  int nAltitudes;
  float lu_alt_array[1000];
  float lu_u_array[1000];
  float lu_v_array[1000];
  float lu_w_array[1000];
  float lu_temp_array[1000];
  float lu_rho_array[1000];
  float lu_press_array[1000];
  int fileWindProfiles[1000];
  int valWindNumber;

  int num_variables;
  if (option ==1) {
    BinFile.open("DRWP_w_comp.bin");
    num_variables = 6;  //  u, v, w, rho, temp, press
  }
  else if (option ==2) {
    BinFile.open("DRWP_no_w_comp.bin");
    num_variables = 5;   //  u, v, rho, temp, press
  }
  else {
    BinFile.open("DRWP_6dependentvariables.bin");
    num_variables = 6; //  u, v, w, rho, temp, press
  }

  if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
  BinFile.read(  (char *) &nProfiles, sizeof(nProfiles) );
  printf("nProfiles = %d\n", nProfiles);
  BinFile.read(  (char *) &nAltitudes, sizeof(nAltitudes) );
  printf("nAltitudes = %d\n", nAltitudes);
  BinFile.read( (char *) &lu_alt_array, nAltitudes*sizeof(lu_alt_array[0]) );
  for  (int i = 0; i<nAltitudes; i++) { printf("lu_alt_array[%d] = %f\n", i, lu_alt_array[i]); }
  BinFile.read( (char *) &fileWindProfiles, nProfiles*sizeof(fileWindProfiles[0]) );
  for  (int i = 0; i<nProfiles; i++) { printf("fileWindProfiles[%d] = %d\n", i, fileWindProfiles[i]); }

  // determine the offset of the group of values for a particular wind profile number
  int offset;
  for (offset = 0; offset < nProfiles; offset++) {
    if ( wind_profile == fileWindProfiles[offset]) break;
  }
  if (offset == nProfiles) { printf("Bad wind profile\n"); return 1; }

  // this is the size of all the values for a particular wind profile
  int bytesPerProfile = (sizeof(lu_u_array[0])*nAltitudes) * num_variables + sizeof(wind_profile);

  //  advance to the correct position in the file
  BinFile.seekg(offset*bytesPerProfile,std::ios::cur);

  BinFile.read( (char *) &valWindNumber, sizeof(valWindNumber) );
  printf("valWindNumber = %d\n", valWindNumber);
  //  check for correct wind profile number
  if( valWindNumber != wind_profile )  { printf("file wind number != input wind number\n"); return 1; }

  BinFile.read( (char *) &lu_u_array, nAltitudes*sizeof(lu_u_array[0]));
  BinFile.read( (char *) &lu_v_array, nAltitudes*sizeof(lu_v_array[0]));
  if (num_variables == 6) BinFile.read( (char *) &lu_w_array, nAltitudes*sizeof(lu_w_array[0]));
  BinFile.read( (char *) &lu_temp_array, nAltitudes*sizeof(lu_temp_array[0]) );
  BinFile.read( (char *) &lu_rho_array, nAltitudes*sizeof(lu_rho_array[0]) );
  BinFile.read( (char *) &lu_press_array, nAltitudes*sizeof(lu_press_array[0]) );


  for (int i = 0; i<nAltitudes; i++) {
    printf("%d - %f, %f, ", i, lu_u_array[i], lu_v_array[i]);
    if (num_variables == 6) {
      printf("%f, ", lu_w_array[i]);
    }
    printf("%f, %f, %f", lu_temp_array[i], lu_rho_array[i], lu_press_array[i]);
    printf("\n");
  }


  BinFile.close();
  return 0;
}

int main() {
  /* available options for first argument:
   * 1, opens DRWP_w_comp.bin with 6 variables
   * 2: opens DRWP_no_w_comp.bin with 5 variables
   * else: opens DRWP_6dependentvariables.bin with 6 variables
   *
   * 2nd argument is wind_profile, which must be present in fileWindProfiles[]
   *
   * e.g.  main_sub(1, 2); main_sub(5, 1771); main_sub(3, 55)
   */
  return main_sub(1, 2);
}

#include <fstream>

int main_sub( std::string filename,
        int num_variables)
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

  BinFile.open( filename);

  if( BinFile.fail() ) { printf("Could not open file\n"); return 1; }
  BinFile.read(  (char *) &nProfiles, sizeof(nProfiles) );
  printf("nProfiles = %d\n", nProfiles);

  BinFile.read(  (char *) &nAltitudes, sizeof(nAltitudes) );
  printf("nAltitudes = %d\n", nAltitudes);

  BinFile.read( (char *) &lu_alt_array, nAltitudes*sizeof(lu_alt_array[0]) );
  for  (int ii = 0; ii<nAltitudes; ii++) {
    printf("lu_alt_array[%d] = %f\n", ii, lu_alt_array[ii]);
  }

  BinFile.read( (char *) &fileWindProfiles, nProfiles*sizeof(fileWindProfiles[0]) );
  for  (int ii = 0; ii<nProfiles; ii++) {
    printf("fileWindProfiles[%d] = %d\n", ii, fileWindProfiles[ii]);
  }

  for (int ii = 0; ii < nProfiles; ii++) {
    BinFile.read( (char *) &valWindNumber, sizeof(valWindNumber) );
    printf("valWindNumber = %d\n", valWindNumber);

    BinFile.read( (char *) &lu_u_array, nAltitudes*sizeof(lu_u_array[0]));
    BinFile.read( (char *) &lu_v_array, nAltitudes*sizeof(lu_v_array[0]));
    if (num_variables == 6) {
      BinFile.read( (char *) &lu_w_array, nAltitudes*sizeof(lu_w_array[0]));
    }
    BinFile.read( (char *) &lu_temp_array, nAltitudes*sizeof(lu_temp_array[0]) );
    BinFile.read( (char *) &lu_rho_array, nAltitudes*sizeof(lu_rho_array[0]) );
    BinFile.read( (char *) &lu_press_array, nAltitudes*sizeof(lu_press_array[0]) );

    for (int jj = 0; jj<nAltitudes; jj++) {
      printf("%d - %f, %f, ", jj, lu_u_array[jj], lu_v_array[jj]);
      if (num_variables == 6) {
        printf("%f, ", lu_w_array[jj]);
      }
      printf("%f, %f, %f", lu_temp_array[jj], lu_rho_array[jj], lu_press_array[jj]);
      printf("\n");
    }
    printf("\n");
  }
  BinFile.close();
  return 0;
}

int main()
{
  return main_sub( "DRWP_w_comp.bin",
           6);
}

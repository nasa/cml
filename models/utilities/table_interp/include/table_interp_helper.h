/*******************************************************************************
 PURPOSE:
    (Helper functions for table_interp)
 
 PROGRAMMERS:
    (((Bingquan Wang) (OSR) (May 2017) (ANTARES) (Initial implementation)))

*******************************************************************************/

#ifndef TABLE_INTERP_HELPER_H
#define TABLE_INTERP_HELPER_H

#include <assert.h>

/*Define the type of function pointer for the comparisor used in lower_bound().
 * Return: >0, if key is located after elem
 *         <0, otherwise
 */
typedef int (*comp_func_t)(const double key, const double elem);

/* Binary search the lower bound in table[] for the given key.
 * The lower bound is the first element in table[] not less than the give key.
 * Return: 
 *    if found, return the index of the lower bound element.
 *    if not found, return the length of table[] 
 */
static inline 
int lower_bound(
  const double table[], /* Array to search */
  int len,              /* Number of elements in table[] */
  const double key,     /* Key to compare */
  comp_func_t comp_func)  /* Comparisor */ 
{
  int index = 0;
  while (0 < len) {
    int step = len/2;
    if (0 < comp_func(key, table[index+step])){
      index += step+1;
      len -= step + 1;
    }
    else {
      len = step;
    }
  }  

  return index;
}

/*comparisor of lower_bound() for ascending */
static inline 
int comp_asc(const double key, const double elem)
{
  return key>elem ? 1 : -1;
}

/*comparisor of lower_bound() for descending */
static inline 
int comp_desc(const double key, const double elem)
{
  return key<elem ? 1 : -1;
}


/*helper function for scale*/
static inline 
void cal_scale(const double value, double *frac, int *low_index, const double *tbl_bpt,
               const int num_bps, const int is_asc)
{
  const int find_index = lower_bound(tbl_bpt, num_bps, value,  is_asc ? &comp_asc : &comp_desc);
  assert(0 <= find_index);

  if (!find_index) {
    *frac = 0.0;
    *low_index = 0;
    return;
  }

  *frac = (value-tbl_bpt[find_index-1])/(tbl_bpt[find_index] - tbl_bpt[find_index-1]);
  *low_index = find_index-1;
}


/*helper function for 1D interpolation*/
static inline 
double cal_interp_1d(const double *tbl, const double frac, const int low_index)
{
  return tbl[low_index] + frac*(tbl[low_index+1]-tbl[low_index]);
}

/*helper function for 2D interpolation*/
static inline 
double cal_interp_2d(const double *tbl, const double frac_x, const int low_index_x,
                     const double frac_y, const int low_index_y, const int ny)
{
  //calculation on Y dimension
  const int low_index = low_index_y + low_index_x*ny;
  const double val_y1 = cal_interp_1d(tbl, frac_y, low_index);
  const double val_y2 = cal_interp_1d(tbl, frac_y, low_index+ny);

  //calculation on X dimension
  return val_y1 + frac_x*(val_y2-val_y1);
} 

/*helper function for 3D interpolation*/
static inline
double cal_interp_3d(const double *tbl, const double frac_x, const int low_index_x,
                   const int nx, const double frac_y, const int low_index_y, const int ny,
                   const double frac_z, const int low_index_z)
{
  //calculation on Y-X planes
  int index_offset = low_index_z*nx*ny;
  const double val_z1 = cal_interp_2d(tbl+index_offset, frac_x, low_index_x, frac_y, 
                                      low_index_y, ny);
  index_offset += nx*ny;
  const double val_z2 = cal_interp_2d(tbl+index_offset, frac_x, low_index_x, frac_y,
                                      low_index_y, ny);
  //calculation on Z dimension
  return val_z1 + frac_z*(val_z2-val_z1);
}

/*helper function for 4D interpolation*/
static inline 
double cal_interp_4d(const double *tbl, const double frac_x, const int low_index_x,
                     const int nx, const double frac_y, const int low_index_y,
                     const int ny, const double frac_z, const int low_index_z, 
                     const int nz, const double frac_w, const int low_index_w)
{
  //calculation on Y-X-Z cubes
  int index_offset = low_index_w*nx*ny*nz;
  const double val_w1 = cal_interp_3d(tbl+index_offset, frac_x, low_index_x, nx, frac_y,
                     low_index_y, ny, frac_z, low_index_z);
  index_offset += nx*ny*nz;
  const double val_w2 = cal_interp_3d(tbl+index_offset, frac_x, low_index_x, nx, frac_y,
                     low_index_y, ny, frac_z, low_index_z);

  //calculation on W dimension
  return val_w1 + frac_w*(val_w2-val_w1); 
}

#endif /* TABLE_INTERP_HELPER_H */


% run from SIM directory
data = csvread("RUN_matrix/log_matrices.csv", 1, 0);

for ii = 1:size(data,1)
  Row = ii

  % Start from column 2 (skip time column)
  idx = 2;

  % vec8: 8 elements
  vec8 = reshape(data(ii, idx:idx+7), 8, 1); idx = idx + 8;

  % arr43: 2 arrays of 4x3 = 12 elements each
  arr43_0 = reshape(data(ii, idx:idx+11), 3, 4)'; idx = idx + 12;
  arr43_1 = reshape(data(ii, idx:idx+11), 3, 4)'; idx = idx + 12;

  % arr42: 3 arrays of 4x2 = 8 elements each
  arr42_0 = reshape(data(ii, idx:idx+7), 2, 4)'; idx = idx + 8;
  arr42_1 = reshape(data(ii, idx:idx+7), 2, 4)'; idx = idx + 8;
  arr42_2 = reshape(data(ii, idx:idx+7), 2, 4)'; idx = idx + 8;

  % arr34: 3x4 = 12 elements
  arr34 = reshape(data(ii, idx:idx+11), 4, 3)'; idx = idx + 12;

  % arr33: 11 arrays of 3x3 = 9 elements each
  arr33_0  = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;
  arr33_1  = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;
  arr33_2  = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;
  arr33_3  = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;
  arr33_4  = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;
  arr33_5  = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;
  arr33_6  = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;
  arr33_7  = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;
  arr33_8  = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;
  arr33_9  = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;
  arr33_10 = reshape(data(ii, idx:idx+8), 3, 3)'; idx = idx + 9;

  % arr32: 3x2 = 6 elements
  arr32 = reshape(data(ii, idx:idx+5), 2, 3)'; idx = idx + 6;

  % arr24: 2x4 = 8 elements
  arr24 = reshape(data(ii, idx:idx+7), 4, 2)'; idx = idx + 8;

  % arr23: 2x3 = 6 elements
  arr23 = reshape(data(ii, idx:idx+5), 3, 2)'; idx = idx + 6;

  % arr22: 3 arrays of 2x2 = 4 elements each
  arr22_0 = reshape(data(ii, idx:idx+3), 2, 2)'; idx = idx + 4;
  arr22_1 = reshape(data(ii, idx:idx+3), 2, 2)'; idx = idx + 4;
  arr22_2 = reshape(data(ii, idx:idx+3), 2, 2)'; idx = idx + 4;

  % test matrix_trans
  % arr24
  arr24_comp = arr24 - arr42_0'

  % test matrix_mult_left_trans
  % arr32
  arr32_comp = arr32 - arr43_0'*arr42_0

  % test matrix_mult_right_trans
  % arr34
  arr34_comp = arr34 - arr32 * arr42_0'

  % test matrix_mult_trans_trans
  % arr23
  arr23_comp = arr23 - arr42_0' * arr34'

  % test matrix_mult
  % arr43_1
  arr43_1_comp = arr43_1 - arr42_0 * arr23

  % test col_maj_vec_to_matrix
  % arr42_1
  arr42_1_comp = arr42_1 - reshape(vec8, 4, 2);

  % test row_maj_vec_to_matrix
  % arr42_2
  arr42_2_comp = arr42_2 - reshape(vec8, 2, 4)';

  % test matrix_inv_using_cholesky
  % NOTE -- on Row 2 and 5, arr33_1 fails, results in a zero matrix and a non-zero
  % comparison.
  arr33_0
  determinant = det(arr33_0)
  arr33_1
  arr33_1_comp = arr33_1 - inv(arr33_0)

  % test matrix_incr
  % arr33_2
  arr33_2_comp = arr33_2 - (arr33_0+arr33_1)

  % test matrix_decr
  % arr33_3
  arr33_3_comp = arr33_3 - (arr33_2-arr33_0)

  % test matrix_transformation
  % arr33_4
  arr33_4_comp = arr33_4 - (arr33_0*arr33_1*arr33_0')

  % test matrix_inverse_transformation
  % arr33_5
  arr33_5_comp = arr33_5 - (arr33_1'*arr33_1*arr33_1)

  % test zero_matrix
  % arr33_6
  arr33_6_comp = arr33_6 - zeros(3,3)

  % test matrix_scale
  % arr33_7
  arr33_7_comp = arr33_7 - 2.5*arr33_1

  % test matrix_copy_submatrix_out
  % arr22_0
  arr22_0_comp = arr22_0 - arr42_0(3:4, 1:2)

  % arr22_1
  result = zeros(2,2);
  result(:, 1) = arr42_0(3:4, 2);
  arr22_1_comp = arr22_1 - result

  % arr22_2
  result = zeros(2,2);
  result(1, :) = arr42_0(4, 1:2);
  arr22_2_comp = arr22_2 - result

  % test matrix_copy_submatrix_in
  % arr33_8 
  result = 100*ones(3,3);
  result(2:3, 2:3) = arr22_0;
  arr33_8_comp = arr33_8 - result
  
  % arr33_9 
  result = 100*ones(3,3);
  result(2:3, 3) = arr22_0(:, 1);
  arr33_9_comp = arr33_9 - result
  
  % arr33_10
  result = 100*ones(3,3);
  result(3, 2:3) = arr22_0(1, :);
  arr33_10_comp = arr33_10 - result

end

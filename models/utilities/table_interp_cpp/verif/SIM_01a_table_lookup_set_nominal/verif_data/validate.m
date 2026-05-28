% This matlab/Octave script provides an alternative multi-dimensional
% interpolation algorithm.
% It has been mostly replaced with verif.py, which provides the full data
% comparison, but has been left here for spot-checking specific
% interpolation issues if they arise.
x_index = load("x_index");
y_index = load("y_index");
z_index = load("z_index");
w_index = load("w_index");

if lookup_mode == 0
  x_frac = load("x_frac");
  y_frac = load("y_frac");
  z_frac = load("z_frac");
  w_frac = load("w_frac");
end

tbl1(:,:,1) = [1111,1112,1113; 1121,1122,1123; 1131,1132,1133; 1141,1142,1143];
tbl1(:,:,2) = [1211,1212,1213; 1221,1222,1223; 1231,1232,1233; 1241,1242,1243];
tbl1(:,:,3) = [1311,1312,1313; 1321,1322,1323; 1331,1332,1333; 1341,1342,1343];

tbl2(:,:,1) = [2111,2112,2113; 2121,2122,2123; 2131,2132,2133; 2141,2142,2143];
tbl2(:,:,2) = [2211,2212,2213; 2221,2222,2223; 2231,2232,2233; 2241,2242,2243];
tbl2(:,:,3) = [2311,2312,2313; 2321,2322,2323; 2331,2332,2333; 2341,2342,2343];

tbl3(:,:,1) = [3111,3112,3113; 3121,3122,3123; 3131,3132,3133; 3141,3142,3143];
tbl3(:,:,2) = [3211,3212,3213; 3221,3222,3223; 3231,3232,3233; 3241,3242,3243];
tbl3(:,:,3) = [3311,3312,3313; 3321,3322,3323; 3331,3332,3333; 3341,3342,3343];

[len, col] = size(x_index);

if lookup_mode == 0
  for i=1:len
    xi = x_index(i)+1; yi = y_index(i)+1; zi = z_index(i)+1;
    xw = x_frac(i); yw = y_frac(i); zw = z_frac(i);
    var1(i) =  xw*(    yw*(tbl1(yi,zi,xi)*zw     + tbl1(yi,zi+1,xi)*(1-zw)) +...
                   (1-yw)*(tbl1(yi+1,zi,xi)*zw   + tbl1(yi+1,zi+1,xi)*(1-zw)))...
          +(1-xw)*(   yw *(tbl1(yi,zi,xi+1)*zw   + tbl1(yi,zi+1,xi+1)*(1-zw)) +...
                   (1-yw)*(tbl1(yi+1,zi,xi+1)*zw + tbl1(yi+1,zi+1,xi+1)*(1-zw)));
    var2(i) =  xw*(    yw*(tbl2(yi,zi,xi)*zw     + tbl2(yi,zi+1,xi)*(1-zw)) +...
                   (1-yw)*(tbl2(yi+1,zi,xi)*zw   + tbl2(yi+1,zi+1,xi)*(1-zw)))...
          +(1-xw)*(   yw *(tbl2(yi,zi,xi+1)*zw   + tbl2(yi,zi+1,xi+1)*(1-zw)) +...
                   (1-yw)*(tbl2(yi+1,zi,xi+1)*zw + tbl2(yi+1,zi+1,xi+1)*(1-zw)));
    
    wi = w_index(i)+1; ww = w_frac(i);
    var3(i) =  xw*(    yw*(tbl3(yi,wi,xi)*ww     + tbl3(yi,wi+1,xi)*(1-ww)) +...
                   (1-yw)*(tbl3(yi+1,wi,xi)*ww   + tbl3(yi+1,wi+1,xi)*(1-ww)))...
          +(1-xw)*(   yw *(tbl3(yi,wi,xi+1)*ww   + tbl3(yi,wi+1,xi+1)*(1-ww)) +...
                   (1-yw)*(tbl3(yi+1,wi,xi+1)*ww + tbl3(yi+1,wi+1,xi+1)*(1-ww)));
  end
else
  for i=1:len
    xi = x_index(i)+1; yi = y_index(i)+1; zi = z_index(i)+1;
    var1(i) =  tbl1(yi,zi,xi);
    var2(i) =  tbl2(yi,zi,xi);
    
    wi = w_index(i)+1;
    var3(i) =  tbl3(yi,wi,xi);
  end
end

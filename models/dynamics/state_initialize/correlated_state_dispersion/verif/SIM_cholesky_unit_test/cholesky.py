#!/usr/bin/env python3
import numpy as np
covar = np.matrix('''
                    1    1   0   0   0   0   0   0   0;
                    1    1   0   0   0   0   0   0   0;
                    0    0   1   0   0   0   0   0   0;
                    0    0   0   1   0   0   0   0   0;
                    0    0   0   0   1   0   0   0   0;
                    0    0   0   0   0   1   0   0   0;
                    0    0   0   0   0   0   1   0   0;
                    0    0   0   0   0   0   0   1   0;
                    0    0   0   0   0   0   0   0   1 ''')
print(covar)
cholesky = np.linalg.cholesky(covar)
print(cholesky)

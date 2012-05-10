/*! \file rov_math.h
 *  \author Soo-Hyun Yoo
 *  \brief Math functions.
 *
 *  Details.
 */

// TODO: Most (if not all) of these functions should return instead of changing
// values internally.

#ifndef ROV_MATH_H
#define ROV_MATH_H

#define ABS(x) (((x) < 0) ? -(x) : (x))
#define CLAMP(x, min, max) (((x) < min) ? (min):(((x) > max) ? (max):(x)))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Dot product
float vDotP (float v1[3], float v2[3]) {
    float output = 0;
    output += v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
    return output;
}

// Cross product
void vCrossP (float v1[3], float v2[3], float vOut[3]) {
    float output[3];

    output[0] = (v1[1]*v2[2]) - (v1[2]*v2[1]);
    output[1] = (v1[2]*v2[0]) - (v1[0]*v2[2]);
    output[2] = (v1[0]*v2[1]) - (v1[1]*v2[0]);

    for (int i=0; i<3; i++) {
        vOut[i] = output[i];
    }
}

// Scalar multiplication
void vScale (float v[3], float scale, float vOut[3]) {
    for (int i=0; i<3; i++) {
        vOut[i] = v[i] * scale; 
    }
}

// Addition
void vAdd (float v1[3], float v2[3], float vOut[3]) {
    for (int i=0; i<3; i++) {
         vOut[i] = v1[i] + v2[i];
    }
}

// Calculate modulus of vector = sqrt(x^2 + y^2 + z^2)
float vMod (float v[3]) {
    float output;
    output = v[0] * v[0];
    output += v[1] * v[1];
    output += v[2] * v[2];
    output = sqrt(output);
    return output;
}

// Normalize vector to a vector with same direction, mod 1, and return old modulus.
float vNorm (float v[3]) {
    float mod;
    mod = vMod(v);
    v[0] /= mod;
    v[1] /= mod;
    v[2] /= mod;
    return mod;
}

// 3x3 matrix transpose -- receive matrix mIn as input and output its transpose mOut.
void mTranspose (float mIn[3][3], float mOut[3][3]) {
    int i, j;
    for (i=0; i<3; i++) {
        for (j=0; j<3; j++) {
            mOut[j][i] = mIn[i][j];
        }
    }
}


// 3x3 matrix multiplication
void mProduct (float m1[3][3], float m2[3][3], float mOut[3][3]) {
    float tmp[3];
    for (int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            for(int k=0; k<3; k++) {
                tmp[k] = m1[i][k] * m2[k][j];
            }
            mOut[i][j] = tmp[0] + tmp[1] + tmp[2];
        }
    }
}

// 3x3 matrix addition
void mAdd (float m1[3][3], float m2[3][3], float mOut[3][3]) {
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            mOut[i][j] = m1[i][j] + m2[i][j];
        }
    }
}

// Matrix Inversion Routine from http://www.arduino.cc/playground/Code/MatrixMath
// * This function inverts a matrix based on the Gauss Jordan method.
// * Specifically, it uses partial pivoting to improve numeric stability.
// * The algorithm is drawn from those presented in 
//       NUMERICAL RECIPES: The Art of Scientific Computing.
// * The function returns 1 on success, 0 on failure.
// * NOTE: The argument is ALSO the result matrix, meaning the input matrix is REPLACED
int mInverse(int n, float mat[3][3]) {
    // mat = input matrix matrix AND result matrix
    // n = number of rows = number of columns in mat (n x n)
    int pivrow;             // keeps track of current pivot row
    int k,i,j;              // k: overall index along diagonal; i: row index; j: col index
    int pivrows[n]; // keeps track of rows swaps to undo at end
    float tmp;              // used for finding max value and making column swaps

    for (k = 0; k < n; k++) {
        // find pivot row, the row with biggest entry in current column
        tmp = 0;
        for (i = k; i < n; i++) {
            if (fabs(mat[i][k]) >= tmp) {   // Avoid using other functions inside abs()?
                tmp = fabs(mat[i][k]);
                pivrow = i;
            }
        }

        // check for singular matrix
        if (mat[pivrow][k] == 0.0f) {
            //Inversion failed due to singular matrix
            return 0;
        }

        // Execute pivot (row swap) if needed
        if (pivrow != k) {
            // swap row k with pivrow
            for (j = 0; j < n; j++) {
                tmp = mat[k][j];
                mat[k][j] = mat[pivrow][j];
                mat[pivrow][j] = tmp;
            }
        }
        pivrows[k] = pivrow;   // record row swap (even if no swap happened)
        tmp = 1.0f/mat[k][k];   // invert pivot element
        mat[k][k] = 1.0f;   // This element of input matrix becomes result matrix

        // Perform row reduction (divide every element by pivot)
        for (j = 0; j < n; j++) {
            mat[k][j] = mat[k][j]*tmp;
        }

        // Now eliminate all other entries in this column
        for (i = 0; i < n; i++) {
            if (i != k) {
                tmp = mat[i][k];
                mat[i][k] = 0.0f;   // The other place where in matrix becomes result mat
                for (j = 0; j < n; j++) {
                    mat[i][j] = mat[i][j] - mat[k][j]*tmp;
                }
            }
        }
    }

    // Done, now need to undo pivot row swaps by doing column swaps in reverse order
    for (k = n-1; k >= 0; k--) {
        if (pivrows[k] != k) {
            for (i = 0; i < n; i++) {
                tmp = mat[i][k];
                mat[i][k] = mat[i][pivrows[k]];
                mat[i][pivrows[k]] = tmp;
            }
        }
    }
    return 1;
}

void kalmanUpdate(float& mean1, float& var1, float mean2, float var2) {
    mean1 = (var2 * mean1 + var1 * mean2) / (var1 + var2);
    var1  = 1/(1/var1 + 1/var2);
}

void kalmanPredict(float& mean1, float& var1, float mean2, float var2) {
    mean1 = mean1 + mean2;
    var1  = var1 + var2;
}

#endif // ROV_MATH_H


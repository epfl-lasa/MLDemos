size_t pontius_n = 40;
size_t pontius_p = 3;

double pontius_x[] = { 150000, 300000, 450000, 600000, 750000, 900000,
1050000, 1200000, 1350000, 1500000, 1650000, 1800000, 1950000, 2100000,
2250000, 2400000, 2550000, 2700000, 2850000, 3000000, 150000, 300000,
450000, 600000, 750000, 900000, 1050000, 1200000, 1350000, 1500000,
1650000, 1800000, 1950000, 2100000, 2250000, 2400000, 2550000, 2700000,
2850000, 3000000 };

double pontius_y[] = { .11019, .21956, .32949, .43899, .54803, .65694,
.76562, .87487, .98292, 1.09146, 1.20001, 1.30822, 1.41599, 1.52399,
1.63194, 1.73947, 1.84646, 1.95392, 2.06128, 2.16844, .11052, .22018,
.32939, .43886, .54798, .65739, .76596, .87474, .98300, 1.09150,
1.20004, 1.30818, 1.41613, 1.52408, 1.63159, 1.73965, 1.84696,
1.95445, 2.06177, 2.16829 };

void
test_pontius ()
{
  size_t i, j;
  {
    gsl_multifit_linear_workspace * work = 
      gsl_multifit_linear_alloc (pontius_n, pontius_p);

    gsl_matrix * X = gsl_matrix_alloc (pontius_n, pontius_p);
    gsl_vector_view y = gsl_vector_view_array (pontius_y, pontius_n);
    gsl_vector * c = gsl_vector_alloc (pontius_p);
    gsl_vector * r = gsl_vector_alloc (pontius_n);
    gsl_matrix * cov = gsl_matrix_alloc (pontius_p, pontius_p);
    gsl_vector_view diag;

    double chisq;

    double expected_c[3] = { 0.673565789473684E-03,
                             0.732059160401003E-06,
                            -0.316081871345029E-14};

    double expected_sd[3] = { 0.107938612033077E-03,
                              0.157817399981659E-09,
                              0.486652849992036E-16 };

    double expected_chisq = 0.155761768796992E-05;

    for (i = 0 ; i < pontius_n; i++) 
      {
        for (j = 0; j < pontius_p; j++) 
          {
            gsl_matrix_set(X, i, j, pow(pontius_x[i], j));
          }
      }

    gsl_multifit_linear (X, &y.vector, c, cov, &chisq, work);

    gsl_test_rel (gsl_vector_get(c,0), expected_c[0], 1e-10, "pontius gsl_fit_multilinear c0") ;
    gsl_test_rel (gsl_vector_get(c,1), expected_c[1], 1e-10, "pontius gsl_fit_multilinear c1") ;
    gsl_test_rel (gsl_vector_get(c,2), expected_c[2], 1e-10, "pontius gsl_fit_multilinear c2") ;

    diag = gsl_matrix_diagonal (cov);

    gsl_test_rel (gsl_vector_get(&diag.vector,0), pow(expected_sd[0],2.0), 1e-10, "pontius gsl_fit_multilinear cov00") ;
    gsl_test_rel (gsl_vector_get(&diag.vector,1), pow(expected_sd[1],2.0), 1e-10, "pontius gsl_fit_multilinear cov11") ;
    gsl_test_rel (gsl_vector_get(&diag.vector,2), pow(expected_sd[2],2.0), 1e-10, "pontius gsl_fit_multilinear cov22") ;

    gsl_test_rel (chisq, expected_chisq, 1e-10, "pontius gsl_fit_multilinear chisq") ;

    gsl_multifit_linear_residuals(X, &y.vector, c, r);
    gsl_blas_ddot(r, r, &chisq);
    gsl_test_rel (chisq, expected_chisq, 1e-10, "pontius gsl_fit_multilinear residuals") ;

    gsl_vector_free(c);
    gsl_vector_free(r);
    gsl_matrix_free(cov);
    gsl_matrix_free(X);
    gsl_multifit_linear_free (work);
  }


  {
    gsl_multifit_linear_workspace * work = 
      gsl_multifit_linear_alloc (pontius_n, pontius_p);

    gsl_matrix * X = gsl_matrix_alloc (pontius_n, pontius_p);
    gsl_vector_view y = gsl_vector_view_array (pontius_y, pontius_n);
    gsl_vector * w = gsl_vector_alloc (pontius_n);
    gsl_vector * c = gsl_vector_alloc (pontius_p);
    gsl_vector * r = gsl_vector_alloc (pontius_n);
    gsl_matrix * cov = gsl_matrix_alloc (pontius_p, pontius_p);

    double chisq;

    double expected_c[3] = {  0.673565789473684E-03,
                               0.732059160401003E-06,
                               -0.316081871345029E-14};

    double expected_chisq = 0.155761768796992E-05;

    double expected_cov[3][3] ={ 
      {2.76754385964916e-01 , -3.59649122807024e-07,   9.74658869395731e-14},
      {-3.59649122807024e-07,   5.91630591630603e-13,  -1.77210703526497e-19},
      {9.74658869395731e-14,  -1.77210703526497e-19,   5.62573661988878e-26} };


    for (i = 0 ; i < pontius_n; i++) 
      {
        for (j = 0; j < pontius_p; j++) 
          {
            gsl_matrix_set(X, i, j, pow(pontius_x[i], j));
          }
      }

    gsl_vector_set_all (w, 1.0);

    gsl_multifit_wlinear (X, w, &y.vector, c, cov, &chisq, work);

    gsl_test_rel (gsl_vector_get(c,0), expected_c[0], 1e-10, "pontius gsl_fit_multilinear c0") ;
    gsl_test_rel (gsl_vector_get(c,1), expected_c[1], 1e-10, "pontius gsl_fit_multilinear c1") ;
    gsl_test_rel (gsl_vector_get(c,2), expected_c[2], 1e-10, "pontius gsl_fit_multilinear c2") ;


    for (i = 0; i < pontius_p; i++) 
      {
        for (j = 0; j < pontius_p; j++)
          {
            gsl_test_rel (gsl_matrix_get(cov,i,j), expected_cov[i][j], 1e-10, 
                          "pontius gsl_fit_wmultilinear cov(%d,%d)", i, j) ;
          }
      }

    gsl_test_rel (chisq, expected_chisq, 1e-10, "pontius gsl_fit_wmultilinear chisq") ;

    gsl_multifit_linear_residuals(X, &y.vector, c, r);
    gsl_blas_ddot(r, r, &chisq);
    gsl_test_rel (chisq, expected_chisq, 1e-10, "pontius gsl_fit_wmultilinear residuals") ;

    gsl_vector_free(w);
    gsl_vector_free(c);
    gsl_vector_free(r);
    gsl_matrix_free(cov);
    gsl_matrix_free(X);
    gsl_multifit_linear_free (work);
  }
}

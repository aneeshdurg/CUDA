#include <mkl.h>
#include <math.h>

// Scalars
const float alpha = 1;
const float beta = 0;

void ckm( struct svm_problem *prob, struct svm_problem *pecm, float *gamma  )
{
	double g_val = *gamma;

	//long int nfa;
	
	int len_tv;
	int ntv;
	int i_v;
	int i_el;
	int i_r, i_c;
	int trvei;

	double *tv_sq;
	double *v_f_g;

	float *tr_ar;
	float *tva, *vtm, *DP;
	float *g_tva = 0, *g_vtm = 0, *g_DotProd = 0;

	len_tv = prob-> x[0].dim;
	ntv   = prob-> l;

	//nfa = len_tv * ntv; 

	tva = (float*) malloc ( len_tv * ntv* sizeof(float) );
	vtm = (float*) malloc ( len_tv * sizeof(float) );
	DP  = (float*) malloc ( ntv * sizeof(float) );

	tr_ar = (float*) malloc ( len_tv * ntv* sizeof(float) );

	tv_sq = (double*) malloc ( ntv * sizeof(double) );

	v_f_g  = (double*) malloc ( ntv * sizeof(double) );

	for ( i_r = 0; i_r < ntv ; i_r++ )
	{				 
		for ( i_c = 0; i_c < len_tv; i_c++ ) 
			tva[i_r * len_tv + i_c] = (float)(prob-> x[i_r].values[i_c]);
	}

	g_tva = (float*)mkl_malloc(len_tv * ntv * sizeof(float), sizeof(float));
	
	if ( !g_tva ) 
	{
		free( tva );
		free( vtm );
		free( DP  );
		free( v_f_g );
		free( tv_sq );
  		exit(1);
	}

	 g_vtm = (float*)mkl_malloc(len_tv * sizeof(float), sizeof(float));
	 g_DotProd = (float*)mkl_malloc(ntv * sizeof(float), sizeof(float));

	for( i_r = 0; i_r < ntv; i_r++ )
		for( i_c = 0; i_c < len_tv; i_c++ )
			tr_ar[i_c * ntv + i_r] = tva[i_r * len_tv + i_c];

	free( tr_ar );

	for( i_v = 0; i_v < ntv; i_v++ )
	{
		tv_sq[ i_v ] = 0;
		for( i_el = 0; i_el < len_tv; i_el++ )
			tv_sq[i_v] += pow( tva[i_v*len_tv + i_el], (float)2.0 );
	}



	for ( trvei = 0; trvei < ntv; trvei++ )
	{
		cblas_sgemv( CblasColMajor, CblasNoTrans, ntv, len_tv, alpha, g_tva, ntv , g_vtm, 1, beta, g_DotProd, 1 );

		for ( i_c = 0; i_c < ntv; i_c++ )
			v_f_g[i_c] = exp( -g_val * (tv_sq[trvei] + tv_sq[i_c]-((double)2.0)* (double)DP[i_c] ));
		

		pecm-> x[trvei].values[0] = trvei + 1;
		
		for ( i_c = 0; i_c < ntv; i_c++ )
			pecm-> x[trvei].values[i_c + 1] = v_f_g[i_c];				
		

	}

	free( tva );
	free( vtm );
	free( DP  );
	free( v_f_g );
	free( tv_sq );

	mkl_free( g_tva );
	mkl_free( g_vtm );
	mkl_free( g_DotProd );
}

void cal_km( struct svm_problem * p_km)
{
	float gamma = param.gamma;

	ckm(&prob, p_km, &gamma);
}

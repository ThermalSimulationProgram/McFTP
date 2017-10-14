#include <stdio.h>
#include <vector>
#include <string.h>

#include "CMI.h"
#include "utils/Parser.h"
#include "utils/utils.h"
#include "utils/c8lib.hpp"
#include "utils/r8lib.hpp"
#include "utils/FileOperator.h"

using namespace std;

void checkVectorMatrixOrder(vector<vector<double> > A, int order, string title);

int main(int argc, char const *argv[])
{
	string file;

	int nloop = 200;
	if (argc > 1){
		for(int i=1; i<=argc; i++){
			if(argv[i] == NULL)
				continue;
			if (argv[i] == string("-l")){
				i++;
				nloop = atoi(argv[i]);
				if (nloop <= 0){
					printf("input loop number must be positive\n");
					exit(1);
				}
			}else{
				file = string(argv[i]);
				if(file.find(".xml") == string::npos) {
					file = file + ".xml";
				}
			}
			
		}
	}else{
		file = "ThermalModelExample.xml";
	}

	Parser parser = Parser(file);

	vector<vector<double> > INVC;
	vector<vector<double> > G;
	vector<vector<double> > K;
	double ambientT, period;

	parser.parseThermalModel(INVC, G, K, ambientT, period);

	int N = INVC.size();
	checkVectorMatrixOrder(INVC, N, "C");
	checkVectorMatrixOrder(G, N, "G");
	checkVectorMatrixOrder(K, N, "K");

	double *array_invc = vectorMatrixTo2DArray(INVC);
	double *array_g = vectorMatrixTo2DArray(G);
	double *array_k = vectorMatrixTo2DArray(K);

	double *gPlusk = r8mat_add_new ( N, N, 1.0, array_g, 1.0, array_k );

	double *A = r8mat_mm_new(N, N, N, array_invc, gPlusk);

	double *B = array_invc;

	double *W, *V;


	matrixAAndBToWAndV( N, A, B, period, &W, &V, nloop);

	vector<vector<double> > w = vector2DArrayToVectorMatrix(W, N, N);

  	vector<vector<double> > v = vector2DArrayToVectorMatrix(V, N, N);

  	saveToNewFile("thermal_model_W.csv", w);

  	saveToNewFile("thermal_model_V.csv", v);


  	delete[] array_invc;
  	delete[] array_k;
  	delete[] array_g;


  	delete[] A;
  	delete[] W;
  	delete[] V;

  	return 1;
}

void checkVectorMatrixOrder(vector<vector<double> > A, int order, string title){
	int N = A.size();
	if (N <= 0){
		printf("Error, given matrix %s is empty!\n", title) ;
		exit(1);
	}else{
		if (N != order){
			printf("Error, given matrix %s row number should be %d!\n", title, order) ;
			exit(1);
		}
		for (int i = 0; i < N; ++i)
		{
			if ((int) A[i].size() != N){
				printf("Error, given matrix %s must be a square matrix!\n", title);
				exit(1);  
			}
		}
	}
}

void useCMI(){
	CMI cmi = CMI("NoFileHere");
}
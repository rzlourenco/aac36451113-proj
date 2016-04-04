#include "stdio.h"
#include <stdio.h>

#define N 100

int Data[N+10];

int fibonacci(){
	int i, aux;
	char c;

	Data[0] = 1;
	aux = Data[0];
	if (N>1) {Data[1] = 1; aux += Data[1];}
	for (i=2;i<N; i++) {
		Data[i] = (Data[i-1]+Data[i-2]) & 0xffff;
		aux += Data[i];
	}
	return aux & 0xffff;
}

int simple_randomize(){
	int i, aux;

	for (i=0, aux=0; i<N; i++) {
		Data[i] = (Data[i]*1103515245 + 12345)>>16;
		aux += Data[i];
	}
	return aux & 0xffff;
}

int randomize(){
	int i, aux;
	int val_10, val_20, val_30, val_40;
	int val_11, val_21, val_31, val_41;
	int val_12, val_22, val_32, val_42;
	int val_13, val_23, val_33, val_43;

	if (N<0) return 0;

	val_10 = Data[0];
	val_11 = Data[1];
	val_12 = Data[2];
	val_13 = Data[3];

	val_20 = val_10*1103515245;
	val_21 = val_11*1103515245;
	val_22 = val_12*1103515245;

	val_30 = val_20 + 12345;
	val_31 = val_21 + 12345;

	val_40 = val_30 >> 16;

	for (i=0, aux=0; i<N; i++) {
		aux += val_40;
		Data[i] = val_40;
		val_40 = val_31 >> 16;
		val_31 = val_22 + 12345;
		val_22 = val_13*1103515245;
		val_13 = Data[i+4];
	}
	return aux & 0xffff;
}

int sortUp(){
	int sorted=0;
	int aux, n_loops=0, n_swaps=0;
	int i;

	while (!sorted){
		n_loops++;
		sorted = 1;
		for (i=0;i<N-1; i++) {
			if (Data[i+1]<Data[i]){
				aux = Data[i];
				Data[i] = Data[i+1];
				Data[i+1] = aux;
				sorted = 0;
				n_swaps++;
			}
		}
	}

	return (n_loops << 16) | (n_swaps & 0xffff);

}

int main(int argc, char **argv) {
	int Q0, Q1, Q2, Q3, Q4;
	int i, res;

	res = fibonacci();
	xil_printf("fibonacci res = %X\n",res);

	res = randomize();
	xil_printf("randomize res = %X\n",res);

	res = simple_randomize();
	xil_printf("simple_randomize res = %X\n",res);

	res = sortUp();
	xil_printf("sortUp res = %X\n",res);

}

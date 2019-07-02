#include "sort.c"


 
int main()
{
	int A[] = { 4, 1, 44, -12, 5, 125, 30,60};
	int B[] = { 4, 1, 44, -12, 5, 125, 30 ,10};
	int C[] = { 4, 1, 44, -12, 5, 125, 30,50 };
	int D[] = { 4, 1, 44, -12, 5, 125, 30,50 };
	int E[] = { 4, 1, 44, -12, 5, 125, 30,50 };
	int F[] = { 4, 1, 44, -12, 5, 125, 30,50 };
#if 1
	out_int_array(A, 8);
	quicksort(A, 0, 7);
	out_int_array(A, 8);
#endif 
	printf("\n 快速排序\n \n");
	sort_quick(A, 0, 7);
	out_int_array(A, 8);
 
	printf("\n 选择排序\n\n");
	xuanzhe_sort(B,8);
	out_int_array(B, 8);
 
	printf("\n 冒泡排序\n\n");
	maopao(C, 8);
	out_int_array(C, 8);
 
	printf("\n\n 插入排序 从右往左\n");
	insert_sort(D,8);
	out_int_array(D, 8);
	printf("\n\n 插入排序 从左往右 \n");
	insert_sort1(E, 8);
	out_int_array(E, 8);
 
	printf("\n\n归并排序\n");
	MergeSort(F,8);
	out_int_array(F,8);
 
 	getchar();
	return 0;
}

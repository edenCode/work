#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define print_array out_int_array

void out_int_array(int data[], int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		printf("%d ", data[i]);
	}
	printf("\r\n");
}


void swap(int *a, int *b)
{
	int x;
	x = *a;
	*a = *b;
	*b = x;
}
 
void swap_xor(int a,int b){
	a  = a ^ b;
	b  = b ^ a;
	a  = a ^ b;
}

 



/*
参考： http://blog.csdn.net/yuehailin/article/details/68961304
归并排序;
归并排序是将一个集合分成两部分:part1和part2，分别对part1和part2进行排序(使用递归法，直到子集合的大小为1，则停止将集合拆分，
此时因为子集合中只有一个元素，所以，这个子集合也就相当于已经拍好了顺序)，最后将这两部分排好顺序的集合合并为一。
*/
 
//将有二个有序数列a[first...mid]和a[mid...last]合并。  
void mergearray(int a[], int first, int mid, int last, int temp[])
{
	//printf("mergearray\n");
	int i = first, j = mid + 1;
	int m = mid, n = last;
	int k = 0;
 
	while (i <= m && j <= n)
	{
		if (a[i] <= a[j])
			temp[k++] = a[i++];
		else
			temp[k++] = a[j++];
	}
 
	while (i <= m)
		temp[k++] = a[i++];
 
	while (j <= n)
		temp[k++] = a[j++];
 
	for (i = 0; i < k; i++)
		a[first + i] = temp[i];
}

void mergesort(int a[], int first, int last, int temp[])
{
 
	if (first < last)
	{
		int mid = (first + last) / 2;
		printf("mid  %d  first %d last %d \n", mid,first,last);
		mergesort(a, first, mid, temp);    //左边有序  
//		printf("右 mid : %d  first %d last %d \n", mid, first, last);
		mergesort(a, mid + 1, last, temp); //右边有序  
	//	printf("合并 mid : %d  first %d last %d \n", mid, first, last);
		mergearray(a, first, mid, last, temp); //再将二个有序数列合并  
	}
 
	//printf("递归返回\n");
}
 
 
/*
  快速排序 ： 方法 2
*/
int new_random(int min, int max)
{
	return (min + (int)(((float)rand() / RAND_MAX)*(max - min)));
}
 
int partition(int A[], int p, int r)
{
	//p = 0 r = 7  p 为 中心
	int i = p - 1, j;
	for (j = p; j < r; j++)
	{
		if (A[j] >= A[r])
		{
			i++;
			swap(&A[i], &A[j]);
		}
		swap(&A[i + 1], &A[r]);
	}
 
	return i + 1;
}

//  
void quicksort(int A[], int p, int r)
{
	int i;
	if (p < r)
	{
		i = partition(A, p, r);
	//	printf("partion :");
	//	out_int_array(A, r);
		quicksort(A, 0, i - 1);
	//	out_int_array(A,r);
		quicksort(A, i + 1, r);
//		out_int_array(A, r);
	}
	
}
 
int randomize_partition(int A[], int p, int r)
{
	int i = new_random(p, r);
	swap(&A[i], &A[r]);
	return partition(A, p, r);
}
 
void randomize_quicksort(int A[], int p, int r)
{
	int i;
	if (p < r)
	{
		i = randomize_partition(A, p, r);
		quicksort(A, 0, i - 1);
		quicksort(A, i + 1, r);
	}
}
 
/*
快速排序的原理：
		快速排序在数组中选择一个称为主元(pivot)的元素将数组分成两个部分，使得第一部分的所有元素小于或等于主元（pivot），而第二部分的所有元素均大于主元（pivot）。
然后对第一部分递归得应用快速排序算法，然后对第二部分递归得应用快速排序算法。
Pivot的英文意思为中心点，所以，快速排序可以理解为找中心点，然后根据中心点，将数组分成两部分，一部分小于或等于中心点，另一部分大于中心点，然后按照此方法对两个部分分别执行，最后就将数组排好了顺序。
*/
int sort_quick(int a[],int left,int right) {
 
	int temp = a[left]; // 选取一个中心点，分割成两个数组，使得中心点右边的元素都大于中心点，中心左边的值都小于中心点。
	int i = left;
	int j = right;
	int t;
	if (left < right)
	{
		while (i != j)
		{
			while (a[j] <= temp  && i < j)
				j--;
 
			while (a[i] >= temp  && i < j )
				i++;
		
			if (i < j) {
				swap_xor(a[j],a[i]);
			}
		}

		a[left] = a[i];
		a[i] = temp;
		sort_quick(a,left,i - 1);//继续处理左边的，这里是一个递归的过程 
		sort_quick(a,i + 1, right);//继续处理右边的 ，这里是一个递归的过程 
	}
	return 0;
}
/*
	    选择排序：
		比如在一个长度为N的无序数组中，在第一趟遍历N个数据，找出其中最小的数值与第一个元素交换，
		第二趟遍历剩下的N-1个数据，找出其中最小的数值与第二个元素交换
		......第N-1趟遍历剩下的2个数据，找出其中最小的数值与第N-1个元素交换，至此选择排序完成
*/
int xuanzhe_sort(int a [],int len)
{
	int i, j;
	//int temp;
 
	for (i = 0;i < len;i++) {
		for (j = i + 1;j < len ;j++) { // 从 i + 1 后 开始寻找比a[i] 大的进行位子的交换。
			if (a[j] > a[i]) {
				swap(&a[j],&a[i]);
			}
		}
	}
 
	return 0;
}
/*
	冒泡排序算法的运作如下：（从后往前）
				1.比较相邻的元素。如果第一个比第二个大，就交换他们两个。
				2.对每一对相邻元素作同样的工作，从开始第一对到结尾的最后一对。在这一点，最后的元素应该会是最大的数。
				3.针对所有的元素重复以上的步骤，除了最后一个。
				4.持续每次对越来越少的元素重复上面的步骤，直到没有任何一对数字需要比较
*/
int maopao(int a[], int len)
{
	int i, j;
 
	for (i = 0;i < len;i++) {
		for (j = 0;j < len;j++) {
			if (a[j + 1] > a[j]) // 相邻之间的元素比较
				swap(&a[j + 1], &a[j]); // 满足条件的，相邻之间的进行交换。
		}
/*
		printf("第 %d 次 ：", i + 1);
		out_int_array(a, len);
		printf("\n");
*/
	}
	return 0;
}
/*
	插入排序：
		就像打扑克牌是一样，首先手上没有牌然后摸牌，手上有牌了 A ，在摸牌 B 比较与手上牌 A 的大小,进行插入，比A大放到 A 的左边，小的放到右边。
*/
 
int insert_sort(int a[], int len)
{
	int i, j;
	int key;
	if (len < 2)
	{
		printf("lenght is too small !!!!\n");
	}
	for (j = 0; j < len; j++)
	{
		key = a[j];
 
		i = j - 1;
		while (i >= 0 && a[i]  < key) // 比较 0 ~ i 之间与 插入值key的大小 （从右往左比较）
		{
			a[i + 1] = a[i];// a[i] 是小于 key 的，所以在a[i] 之前插入key,先保存a[i]的值移到a[i + 1]的位子?
				/*
					此时有个问题 a[i + 1] 的位子 会不会覆盖其他数组的值？
				*/
			i = i - 1;
			a[i + 1] = key; // 插入值key，此时的 i + 1的位子是之前的 a[i] 的位子
		}
/*
		printf("第 %d 次 ：", j + 1);
		out_int_array(a, len);
		printf("\n");
		*/
	}
	return 0;
}
 
int insert_sort1(int a[], int len)
{
	int i,j,k = 0;
	int tmp;
	for (i = 0; i < len; i++)
	{
		for (j = 0; j < i; j++) // 从左往右比较
		{
			if (a[j] < a[i])
			{
				tmp = a[i];
				
				k = 0;
				
				while (k  < i-j+1) // 这里主要是将 a[j]的位子空出来，将a[j] 往后移动一位，空出a[j]以便插入 a[i] 的值。
				{
					a[i-k] = a[i-k -1];
					k++;
				}
				a[j] = tmp; // 空出来的位子插入 a[i] 的值。
			}
			
		}
		/*
		printf("第 %d 次 ：", i+1);
		out_int_array(a, len);
		printf("\n");
		*/
	}
	return 0;
}
 


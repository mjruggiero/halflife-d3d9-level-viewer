
#ifndef _QUICKSORT_H_
#define _QUICKSORT_H_

template<class T>
void Swap(T &a, T &b)
{
	T temp = a;
	a = b;
	b = temp;
}

template<class T>
void QuickSort(T *elements, int low, int high)
{
	if(low + 2 >= high)
		return;
	
	// sort low, middle, high
	int middle = (low + high) / 2;
	if(elements[middle] < elements[low])
	{
		Swap(elements[middle], elements[low]);
	}
	if(elements[high] < elements[low])
	{
		Swap(elements[low], elements[high]);
	}
	if(elements[high] < elements[middle])
	{
		Swap(elements[middle], elements[high]);
	}

	// place pivot at position high - 1
	T pivot = elements[middle];
	Swap(elements[middle], elements[high - 1]);

	// begin partitioning
	int i, j;
	for(i = low, j = high - 1; ; )
	{
		while(elements[++i] < pivot)
			;
		while(pivot < elements[--j]);
		if(i < j)
			Swap(elements[i], elements[j]);
		else
			break;
	}
	// restore pivot
	Swap(elements[i], elements[high - 1]);

	// sort small elements
	QuickSort(elements, low, i + 1);
	// sort large elements
	QuickSort(elements, i + 1, high);
}

#endif // _QUICKSORT_H_

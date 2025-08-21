#ifndef HEAP_HPP
#define HEAP_HPP


class heap
{
public:
    int arr[100];
    int size;

    heap()
    {
        arr[0] = -1;//Taking 1 base indexing
        size = 0;
    };
};


#endif // HEAP_H
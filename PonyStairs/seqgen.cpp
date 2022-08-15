#include "seqgen.h"

SeqGen::SeqGen(int n)
{
    setN(n) ;
}

SeqGen::SeqGen()
{
    setN(1) ;
}

void SeqGen::setN(int n)
{
    this->n=n ;
}

int SeqGen::getNext()
{
    if (seq.count()==0) {
        for (int i=0; i<n; i++) seq.append(i) ;
        for (int i=0; i<n*n; i++)
            seq.swapItemsAt(rand()%n,rand()%n) ;
    }

    return seq.takeAt(0) ;
}

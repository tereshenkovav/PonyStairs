#ifndef SEQGEN_H
#define SEQGEN_H

#include <QList>

class SeqGen
{
private:
    int n ;
    QList<int> seq ;
public:
    SeqGen(int n);
    SeqGen();
    void setN(int n) ;
    int getNext() ;
};

#endif // SEQGEN_H

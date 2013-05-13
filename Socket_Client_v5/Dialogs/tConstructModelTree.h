#ifndef TCONSTRUCTMODELTREE_H
#define TCONSTRUCTMODELTREE_H

class tConstructModelTree
{
public:
    tConstructModelTree(QString &table);
    ~tConstructModelTree();
    bool NextModel();

private:
    tConstructModelTree();
};

#endif // TCONSTRUCTMODELTREE_H

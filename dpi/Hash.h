//
// Created by jyc on 17-4-17.
//

#ifndef NOFF_HASH_H
#define NOFF_HASH_H
#include <stdlib.h>
class Hash
{
public:
    Hash();
    u_int get_key(u_int , u_short , u_int , u_short,size_t hash_size);

private:
    void init_hash();
    void getrnd();
    u_char xors[12];
    u_char perm[12];
};

#endif //NOFF_HASH_H

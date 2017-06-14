#include <cstdio>
#include <iostream>

#define MAX_TOWERS 2000005
#define SIZE_TREE (1 << 21)


size_t * starting, * ending,* tree,* tab;
size_t n;

void reindexing();
void gen_start_end();
size_t query(size_t a, size_t b);
void insert(size_t x, size_t value);
void insert();
bool read_data();
void erase_data();
void print_starting_ending();

size_t aa[MAX_TOWERS];
size_t bb[MAX_TOWERS];
std::pair<size_t,size_t> to_sort[MAX_TOWERS];
size_t cc[MAX_TOWERS];
size_t  dd[SIZE_TREE *2 +10];
int main()
{
  size_t T=1;
  if( scanf("%zu",&T) != 1 ) return 1;
  starting =aa;
  ending =bb;
  tab = cc;
  tree = dd;
  // starting = new size_t[MAX_TOWERS],  ending = new size_t[MAX_TOWERS];
  // tab = new size_t[MAX_TOWERS], tree = new size_t[SIZE_TREE * 2 + 10];
  erase_data();
  for(size_t it=0; it<T; it++)
  {
    size_t res = 0;
    if( read_data() ) {std::cout << "ERROR: read_data\n"; return 1;}
    reindexing(); //for(size_t i=0; i<n; i++) { std::cout << tab[i] << " "; } std::cout << "\n";
    gen_start_end();
    // print_starting_ending();
    for(size_t i=0; i<n; i++)
    {
      res = std::max(res, query(1, tab[i]-1) + starting[i]  );
      insert(tab[i], ending[i]);
    }
    printf("%zu\n",res );
    erase_data();
  }

  return 0;
}
#include <algorithm>


template <class T1, class T2, class Pred = std::less<T2> >
struct sort_pair_first {
    bool operator()(const std::pair<T1,T2>&left, const std::pair<T1,T2>&right) {
        Pred p;
        return p(left.first, right.first);
    }
};
void reindexing()
{
  for(size_t i=0; i<n; i++)
  {
    to_sort[i] = { tab[i], i };
  }
  std::sort(to_sort , to_sort+n, sort_pair_first<size_t, size_t>() );
  size_t counter = 2;
  tab[to_sort[0].second] = counter;
  for(size_t i=1; i<n; i++)
  {
    if( to_sort[i].first == to_sort[i-1].first )
    {
      tab[to_sort[i].second] = counter;
    }
    else
    {
      tab[to_sort[i].second] = ++counter;
    }
  }
}

void gen_start_end()
{
  ending[0] = 1;
  size_t  length = 1;
  for(size_t i=1; i<n; i++)
  {
    if( tab[i-1] < tab[i] )
    {
      ending[i] = ++length;
    }
    else
    {
      ending[i] = (length = 1);
    }
    // std:: cout << ending[i] << " ";
  }
  starting[n-1] = 1;
  length = 1;
  for(size_t i=1; i<n; i++)
  {
    if( tab[n - 1 - (i-1) ] > tab[n-1 - i] )
    {
      starting[n-1-i] = ++length;
    }
    else
    {
      starting[n-1-i] = (length = 1);
    }
    // std:: cout << ending[i] << " ";
  }

}

size_t query(size_t a, size_t b)
{
  size_t va = SIZE_TREE + a, vb = SIZE_TREE + b;
  size_t res = tree[va];
  if( va != vb )
  {
    res = std::max(res, tree[vb]);
  }
  while( va/2 != vb/2 )
  {
    if (va % 2 == 0) res = std::max( res, tree[va + 1] );
    if (vb % 2 == 1) res = std::max( res,  tree[vb - 1] );
    va /= 2; vb /= 2;
  }
  return res;
}

void insert(size_t x, size_t value)
{
  size_t v = SIZE_TREE + x;
  tree[v] = std::max(value, tree[v]);
  while( v != 1  )
  {
    v /= 2;
    tree[v] = std::max(tree[2*v], tree[2*v +1]  );
  }
}

bool read_data()
{
  if( scanf("%zu\n", &n) != 1 )
  {
    return 1;
  }
  for(size_t i=0; i<n; i++)
  {
    if ( scanf("%zu", &(tab[i])) != 1) return 1;
  }
  return 0;
}

void erase_data()
{
  for(size_t i=0; i<n; i++)
  {
    starting[i] = ending[i] = tab[i] = 0;
  }
  for(size_t i=0; i<SIZE_TREE * 2; i++)
  {
    tree[i] =0;
  }
  // std::cout << "TAK\n";
}

void print_starting_ending()
{
  std::cout << "starting: ";
  for(size_t i=0; i<n; i++)
  {
    std::cout << starting[i] << " ";
  }
  std::cout << "\nending: ";
  for(size_t i=0; i<n; i++)
  {
    std::cout << ending[i] << " ";
  }
}

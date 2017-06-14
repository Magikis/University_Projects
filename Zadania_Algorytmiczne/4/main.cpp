#include <iostream>
#include <cstdio>
#include <vector>
#include <array>
#include <queue>
#include <unordered_map>
#include <tuple>

#define MAX_N 500005
#define PRIME_SEC 199
#define PRIME_FIRST 163819
typedef std::tuple<size_t, size_t, size_t> set_key;
struct tuple_hash
{
  size_t operator () (const set_key & sk) const
    { return std::get<0>(sk) ^ std::get<1>(sk) ^ std::get<2>(sk);  }
};

std::array < std::array<std::vector<size_t>, MAX_N>,2> tab;
std::array < std::array<std::vector<size_t>, MAX_N>,2> keys;
std::array < std::array< size_t, MAX_N>,2> id;
std::unordered_map< set_key , size_t, tuple_hash>  map;
size_t n;
size_t id_counter=1;
std::pair<size_t, size_t> counted_centers[2];
enum trees {fst =0, scd };

bool hashig_root(size_t res_hash);
void clear_tabs();
void show_hashing(size_t nt);
void find_id(size_t nt, size_t v);
size_t hashing_tree(size_t nt);
void count_degs( size_t nt, std::vector<size_t> & degs );
void read_data();
void read_tree(size_t i);
void print_tree(size_t nt);

int main()
{
  size_t d;
  if ( scanf("%zu\n",&d) != 1) return 1;
  for(size_t x = 0; x < d; x++ )
  {
    clear_tabs();
    read_data();
    size_t res_hash[2];
    for(size_t i=0; i<2; i++)
    {
      res_hash[i] = hashing_tree(i);
    }
    if( res_hash[fst] != res_hash[scd] )
    {
      std::cout << "NIE\n";
    }
    else
    {
      if( hashig_root(res_hash[fst]) )
      {
        std::cout << "TAK\n";
      }
      else
      {
        std::cout << "NIE\n";
      }
    }
    #ifdef DEBUG
      for(size_t i=0; i<2; i++)
      {
          print_tree(i);
          show_hashing(i);
          show_fathers(i);
          std::cout << "id_of_root: " << id[i][root[i]] <<"\n";
      }
    #endif
  }
  return 0;
}

bool hashig_root(size_t res_hash)
{
  if(res_hash == 1)
  {
    find_id(fst, counted_centers[fst].first);
    find_id(scd, counted_centers[scd].first);
    return id[fst][counted_centers[fst].first] ==
        id[scd][counted_centers[scd].first];
  }
  find_id(fst, counted_centers[fst].first);
  find_id(scd, counted_centers[scd].first);
  find_id(fst, counted_centers[fst].second);
  find_id(scd, counted_centers[scd].second);
  return
    (
      id[fst][counted_centers[fst].first] == id[scd][counted_centers[scd].first] &&
      id[fst][counted_centers[fst].second] == id[scd][counted_centers[scd].second]
    ) ||
    (
      id[fst][counted_centers[fst].first] == id[scd][counted_centers[scd].second] &&
      id[fst][ counted_centers[fst].second] == id[scd][counted_centers[scd].first]
    );
}

void clear_tabs()
{
  for(size_t i=0; i<=2+n; i++)
  {

    for(size_t nt=0; nt<2; nt++)
    {
      tab[nt][i].clear();
      keys[nt][i].clear();
      id[nt][i] = 0;
    }
  }
  for(size_t nt=0; nt < 2; nt++)
  {
    counted_centers[nt] = std::make_pair(0,0);
  }
  map.clear();
  n=0;
  id_counter = 1;
}

void show_hashing(size_t nt)
{
  std::cout << "nt: " << nt <<" show_hashing:\n";
  for(size_t i=1; i<=n; i++)
  {
    std::cout << i << ": id: " << id[nt][i] << " [";
    for(auto j : keys[nt][i])
    {
      std::cout << j << ",";
    }
    std::cout << "]\n";
  }
  std::cout << "\n\n";
}

void find_id(size_t nt, size_t v)
{
  size_t a=0, b=0, c=0;
  for( auto i : keys[nt][v] )
  {
    a ^= i;
    b += i*i;
    c += i*i*i;
  }
  auto ansewer = map.emplace(std::make_tuple(a,b,c), id_counter);
  if( ansewer.second )
  {
    id[nt][v] = id_counter++;
  }
  else
  {
    id[nt][v] = ansewer.first->second;
  }

}

size_t hashing_tree(size_t nt)
{
  std::vector<size_t> degs;
  count_degs(nt, degs);
  std::queue<size_t> leafs;
  for(size_t i=1; i<=n; i++)
  {
    if(degs[i] == 1)
    {
      leafs.push(i);
      keys[nt][i].push_back(0);
    }
  }
  size_t left_vortex = n;

  bool cuted[MAX_N];
  for(size_t i= 0; i<=n; i++)
    cuted[i]=false;
  while( left_vortex > 2 )
  {
    size_t steps_in_phase = leafs.size();
    for( size_t step=0; step < steps_in_phase; step++ )
    {
      size_t v = leafs.front();
      leafs.pop();
      cuted[v] = true;
      size_t father=0;
      for( auto i : tab[nt][v] )
      {
        if( !cuted[i] )
        {
          father = i;
          break;
        }
      }
      left_vortex--;
      find_id(nt,  v);
      degs[father]--;
      keys[nt][father].push_back( id[nt][v] );
      if( degs[father] == 1 )
      {
        leafs.push(father);
      }
    }
  }
  if(leafs.size() == 2)
  {
    counted_centers[nt].first = leafs.front();
    leafs.pop();
    counted_centers[nt].second = leafs.front();
    return 2;
  }
  counted_centers[nt].first = leafs.front();
  return 1;
  #ifdef DEBUG
    if(leafs.front() != root [nt] )
    {
      std::cout  << "nt: " << nt << " Last isn't root, WTF??\n";
    }
  #endif
  find_id(nt, leafs.front() );
}

void count_degs( size_t nt, std::vector<size_t> & degs )
{
  degs.resize(n+1);
  for( size_t i=1; i<=n; i++)
  {
    degs[i] = tab[nt][i].size();
  }
}

void read_data()
{
  if ( scanf("%zu\n", &n) != 1) return;;
  read_tree(fst);
  read_tree(scd);
}

void read_tree(size_t i)
{
  for(size_t j = 0; j < n-1; j++)
  {
    size_t a,b;
    if ( scanf("%zu  %zu\n",&a ,&b ) != 2 ) return;
    tab[i][a].push_back(b);
    tab[i][b].push_back(a);
  }
}

void print_tree(size_t nt)
{
  for(size_t i = 1; i<=n; i++ )
  {
    printf("%zu: ", i );
    for(auto  & j : tab[nt][i] )
    {
      printf("%zu ",j );
    }
    puts("");
  }
  puts("");
  puts("");
}

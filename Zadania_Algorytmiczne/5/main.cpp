#include <iostream>
#include <array>
#include <queue>
#include <forward_list>
#include <climits>

#include <cstdio>
#define MAX_BARRELS 7
#define PRIME 2416190071
#define MAX_ARRAY ( UINT_MAX >> 17 )

typedef std::array<unsigned short int, MAX_BARRELS> Array;
typedef std::pair<size_t, size_t> Key;
typedef std::array<std::forward_list<Key>, MAX_ARRAY> Dictionary;

Array barrels;
size_t n=0,d=0, maks=0, counter=0;
size_t hash(Key & a){size_t c = a.first, b=a.second;   return (((c+b)*(c+b+1)/2+b)  % PRIME) % MAX_ARRAY; }

void check_dict (Array & tab, size_t steps, std::queue<Array> & queue, Dictionary & dict);
void read_data();
Key pack_key( Array  & tab );

int main()
{

  std::queue< Array >  queue;
  Dictionary dict;
  std::cin >> d;
  for(size_t test=0; test<d; test++)
  {
    read_data();
    maks=0;
    counter = 1;
    queue.push(barrels);
    size_t queue_size =queue.size();
    size_t steps= 0;
    while(!queue.empty())
    {
      steps++;
      queue_size = queue.size();
      for(size_t it = 0; it < queue_size; it++)
      {
        Array act_tab = queue.front();
        // size_t steps = queue.front().second;
        queue.pop();
        for(size_t i=0; i<n; i++)
        {
          if( act_tab[i] == 0  ) continue;
          Array temp = act_tab;
          temp[i] = 0;
          check_dict(temp,  steps, queue, dict);
          temp[i] = act_tab[i];
          for(size_t j=0; j<n; j++)
          {
            if( i==j ||  act_tab[j] == barrels[j] ) continue;
            temp = act_tab;
            if( act_tab[j] + act_tab[i] <= barrels[j]  )
            {
              temp[j] += act_tab[i];
              temp[i] = 0;
            }
            else
            {
              temp[j] = barrels[j];
              temp[i] = act_tab[j] + act_tab[i] - barrels[j];
            }
            check_dict(temp,  steps, queue, dict);
          }
        }
      }
    }
    std::cout << counter << " " << maks << "\n";
    size_t biggest_bucket = 0;
    for(auto i = dict.begin() ; i != dict.end() ; i++)
    {
      #ifndef DEBUG
      size_t list_size=0;
        for( auto j = i->begin(); j != i->end(); j++ )
        {
          list_size++;
        }
        biggest_bucket = std::max(biggest_bucket,list_size );
      #endif
      i->clear();
    }
    //  std::cerr << biggest_bucket << "\n";
  }
  return 0;
}

bool emplace(std::forward_list< Key> & l , Key & data )
{
  for(auto i =l.begin(); i != l.end(); i++ )
  {
    if( *i == data )
    {
      return true;
    }
  }
  l.push_front(data);
  return false;
}

// bool is_in_dict( Key data  )
// {
//   return !set.emplace(data).second;
// }

void  check_dict(Array & tab, size_t steps, std::queue<Array> &  queue, Dictionary & dict)
{
  Key key = pack_key( tab );
  if( emplace( dict[hash(key)], key)  ) return;
  maks = std::max( steps, maks );
  counter++;
  queue.push(tab);
}

std::pair<size_t,size_t> pack_key( Array & tab )
{
  Key ans = {0,0};
  size_t temp1, temp2;
  temp1 = n/2;
  temp2 = n - temp1;

  for(size_t i=0; i<temp1; i++)
  {
    ans.first |= (((1<<10) - 1) & (size_t)tab[i] ) << (i*10);
  }
  for(size_t i = 0; i < temp2; i++ )
  {
    ans.second |= (((1<<10)-1) & (size_t)tab[i+temp1] )<< (i*10);
  }
  return ans;
}

void read_data()
{
  std::cin >> n;
  for(size_t i=0; i<n; i++)
  {
    std::cin >> barrels[i];
  }
}

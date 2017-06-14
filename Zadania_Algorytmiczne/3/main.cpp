#include <cstdio>
#include <cstring>
#include <vector>
#include <bitset>
#include <iostream>
#include <array>

#define TERMINALS 8
#define DEBUG 0

char data_string[1005];
std::vector<std::array<char,3> > first_prod;
std::vector<std::array<char,2> > second_prod;
unsigned char cached[256][256];
unsigned char tab[1005][1005];
size_t word_length = 0;
size_t m1,m2;

bool cyk();
void init_cache();
void init();
bool read_task();
void show_vectors();
void show_tab();
void print_data();
void shift_chars();

int main()
{
  size_t n;
  if( scanf("%zu\n",&n ) != 1 )
  {
    return 1;
  }
  for(size_t i=0; i<n; i++)
  {
    if( read_task() )
    {
      puts("ERROR: read_data");
      return 1;
    }

    shift_chars();
    init();
    init_cache();
    if( cyk() )
    {
      puts("TAK");
    }
    else
    {
      puts("NIE");
    }

    // printf("%s\n",data_string );
    // std::cout << (std::bitset<TERMINALS>)cached[64][2] << "\n";
    // show_tab();
    // return 0;
  }
  return 0;
}

bool cyk()
{
  // printf("WL:%zu\n",word_length );
  for(size_t h=1; h<word_length; h++)
  {
    for(size_t s=0 ; s < word_length-h; s++)
    {
      // printf("Wybralem punkt: (%zu,%zu)\n",s,h);
      std::bitset<TERMINALS> temp(0);
      for(size_t i=0; i < h; i++ )
      {
        size_t i1 = s, j1= i, i2= s+i+1, j2 = h-i-1;
        // printf("(%zu,%zu) with (%zu,%zu)\n",i1,j1,i2,j2 );
        temp  |= cached[(size_t)tab[i1][j1]][(size_t)tab[i2][j2]];
      }
      tab[s][h] = (unsigned char)temp.to_ulong();
      #if DEBUG
        std::cout << (std::bitset<TERMINALS>) tab[s][h] << "  ";
      #endif
    }
    #if DEBUG
      std::cout << "\n";
    #endif
  }
  std::bitset<TERMINALS> res(tab[0][word_length-1]);
  if(res.test(0) )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void init_cache()
{
  for(size_t i=0; i<256; i++)
  {
    for(size_t j=0; j<256; j++)
    {
      std::bitset<TERMINALS> one(i);
      std::bitset<TERMINALS> two(j);
      std::bitset<TERMINALS> result(0);
      for(size_t x=0; x < first_prod.size(); x++)
      {
        if( one.test(first_prod[x][1]) && two.test(first_prod[x][2]) )
        {
          result.set(first_prod[x][0]);
        }
      }
      cached[i][j] = (unsigned char)result.to_ullong();
    }
  }
}



void init()
{
  for(size_t i=0; i<word_length; i++)
  {
    std::bitset<TERMINALS> result(0);
    for(size_t j=0; j<second_prod.size(); j++)
    {
      if(second_prod[j][1] == data_string[i])
      {
        result.set(second_prod[j][0]);
      }
    }
    tab[i][0] = (char) result.to_ulong();
  }
}

bool read_task()
{
  if( scanf("%zu %zu\n",&m1,&m2) != 2 )
  {
    printf("%zu / %zu\n",m1,m2 );
    puts("scanf error");
    return true;
  }
  first_prod.resize(m1);
  second_prod.resize(m2);
  for(size_t i=0; i<m1; i++)
  {
    if( scanf("%c %c %c\n",&first_prod[i][0], &first_prod[i][1], &first_prod[i][2]) != 3)
    {
      printf("first_prod_error: on line %zu\n",i );
      return true;
    }
  }
  for(size_t i=0; i<m2; i++)
  {
    if( scanf("%c %c\n",&second_prod[i][0], &second_prod[i][1]) != 2 )
    {
      printf("second_prod_error: on line %zu\n",i );
      return 1;
    }
  }
  // strcpy(data_string, "");
  // if( fgets(data_string, 1003,  stdin) == NULL)
  // {
  //   printf("ERROR: fgets\n");
  //   return true;
  // }
  // word_length = strlen(data_string)-1;
  if( scanf("%s", data_string) != 1 )
  {
    puts("ERROR: word scanf");
    return 1;
  }
  word_length = strlen(data_string);
  #if DEBUG
    printf("Word_length:%zu",word_length);
  #endif
  return false;
}

void show_tab()
{
  for(size_t h=0; h<word_length; h++)
  {
    for(size_t s=0 ; s < word_length-h; s++)
    {
      std::cout << (std::bitset<TERMINALS>)tab[s][h] << "  ";
    }
    std:: cout << "\n";
  }
  for(size_t h=0; h<word_length; h++)
  {
    for(size_t s=0 ; s < word_length-h; s++)
    {
      std::cout << (size_t)tab[s][h] << "  ";
    }
    std:: cout << "\n";
  }
  // std::bitset<3> a(0);
  // a.set(2);
  // std::cout << a << " " <<a.to_ulong() << "\n";
}

void print_data()
{
    printf("%zu %zu\n",m1,m2 );
    for(size_t i=0; i<first_prod.size(); i++)
    {
      for(size_t j=0; j<first_prod[i].size(); j++)
      {
        printf("%c ",first_prod[i][j] );
      }
      puts("");
    }
    for(size_t i=0; i<second_prod.size(); i++)
    {
      for(size_t j=0; j<second_prod[i].size(); j++)
      {
        printf("%c ",second_prod[i][j] );
      }
      puts("");
    }
    puts(data_string);
}

void shift_chars()
{
  for(size_t i=0; i<first_prod.size(); i++)
  {
    for(size_t j=0; j<first_prod[i].size(); j++)
    {
      first_prod[i][j]-= 'A';
    }
  }
  for(size_t i=0; i<second_prod.size(); i++)
  {
    second_prod[i][0] -= 'A';
  }
}

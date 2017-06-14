#include <cstdio>
#include <bitset>
#include <vector>
#include <iostream>
#include <unistd.h>

#define FIELDS 6
#define MAX_BOARD 1000005
#define MAX_BOARD_0_5 500003
#define SIZE_MASKS 36
size_t n, start_of_board=0;
std::vector<std::bitset<FIELDS> > av_masks;
std::vector<std::bitset<FIELDS> > at_masks;
std::vector<char> prev_ok_masks[SIZE_MASKS];
char board[MAX_BOARD][4];
std::vector<size_t> knights_prev;
std::vector<size_t> knights_act;
char paths[SIZE_MASKS][MAX_BOARD_0_5];
char result_path[MAX_BOARD_0_5];

void apply_knights_to_board(size_t last_mask_id);
void print_board_with_knights();
std::bitset<FIELDS> mask_from_board(int x);
void read_input();
void generate_masks();
void create_atacking_mask(const std::bitset<FIELDS> & a);
void save_masks_pairs();
bool wrong_mask(const std::bitset<FIELDS> & a);
bool is_attacking_field(const int i,const int j);
void show_mask(const std::bitset<FIELDS> &);
void show_all_masks();
void show_output();
void print_board();

int main()
{
  read_input();
  generate_masks();
  save_masks_pairs();
  knights_act.resize(av_masks.size());
  knights_prev.resize(av_masks.size());
  std::vector<size_t> used_masks_copy;
  std::vector<size_t> used_masks;
  used_masks_copy.push_back(0);
  for(size_t board_num = 0; board_num < n; board_num += 2)
  {
    std::bitset<FIELDS> board_mask = mask_from_board(board_num);
    for(size_t act = 0; act < av_masks.size(); act++ )
    {
      size_t res = 0,  choosen_prev=37;
      if( (av_masks[act] & board_mask) == av_masks[act] )
      {
        used_masks.push_back(act);
      }
      else continue;
      for(size_t prev = 0; prev < prev_ok_masks[act].size(); prev++)
      {
        if(res < knights_prev[prev_ok_masks[act][prev]] + av_masks[act].count() )
        {
          res = knights_prev[prev_ok_masks[act][prev]] + av_masks[act].count();
          choosen_prev = prev_ok_masks[act][prev];
        }
      }
      knights_act[act] = res;
      paths[act][board_num/2] = choosen_prev;
    }
    //knights_prev.clear();
    knights_prev = knights_act;
    //knights_act.clear();
    used_masks_copy = used_masks;
    used_masks.clear();
  }
  size_t result=0;
  size_t last_mask_id=0;
  for(size_t i=0; i<knights_prev.size(); i++)
  {
    if(result < knights_prev[i])
    {
      last_mask_id = i;
      result = knights_prev[i];
    }
  }
  printf("%lu\n",result );
  apply_knights_to_board(last_mask_id);
  print_board();
  return 0;
}
//##########################################
//      END MAIN                  ##########
//##########################################
void apply_knights_to_board(size_t last_mask_id)
{
  size_t board_i=n;
  for(long int path_it=n/2-1; path_it >= 0; path_it--)
  {
    size_t next_mask_id = paths[last_mask_id][path_it];
    for(size_t i=0, ib= board_i-2; i<2 ; i++,ib++)
    {
      for(size_t j=0; j<3; j++)
      {
        if(av_masks[last_mask_id].test(i*3+j))
        {
          if(board[ib][j] == '.')
          {
            board[ib][j]='S';
          }
          else
          {
            printf("ERROR i:%lu j%lu:\n",board_i, j );
          }
        }
      }
    }
    board_i -=2;
    last_mask_id = next_mask_id;//printf("AKTB_last_mask_id:%lu\n",last_mask_id );
  }
}

std::bitset<FIELDS> mask_from_board(int x)
{
  std::bitset<FIELDS> res;
  for(int i=0; i<2; i++)
  {
    for(int j=0; j<3; j++)
    {
      if(board[i+x][j] == 'x')
      {
        res.reset(i*3+j);
      }
      else if(board[i+x][j] == '.')
      {
        res.set(i*3+j);
      }
    }
  }
  return res;
}

void read_input()
{
  if( !scanf("%lu\n",&n ) ) return;
  if(n % 2 == 1)
  {
    start_of_board = 1;
    n++;
    for(int i=0; i<3; i++) board[0][i]='x';
  }
  //test read_input
  //if( read(STDIN_FILENO,  (void *)board , n) ) return;
  //return;
  for(size_t i=start_of_board; i<n; i++)
  {
/*
    for(int j=0; j<3; j++)
    {
      char c ;
      c= getchar();
      while( !(c=='.' || c=='x') )
      {
        c = getchar();
      }
      board[i][j] = c;
    }
    */
    fgets(board[i], 5, stdin);
  }
  //print_board();
}

void generate_masks()
{
  for(size_t i=0; i< 64; i++)
  {
    std::bitset<FIELDS> mask(i);
    if( !wrong_mask(mask) )
    {
      av_masks.push_back(mask);
    }
  }
  for(size_t i=0; i<av_masks.size(); i++)
  {
    create_atacking_mask(av_masks[i]);
  }
}

void create_atacking_mask(const std::bitset<FIELDS> & a)
{
  std::bitset<FIELDS> at_m(63);
  int is[8] = {2,2,-2,-2,1,1,-1,-1};
  int js[8] = {1,-1,1,-1,2,-2,2,-2};
  //show_mask(at_m);
  for(int i=0; i<2; i++)
  {
    for(int j=0; j<3; j++)
    {
      if(!a.test(i*3+j) ) continue;
      for(int z=0; z<8; z++)
      {
        int ni = i+is[z];
        int nj = j +js[z];
        if(is_attacking_field(ni,nj))
        {
          at_m.reset(ni*3+nj - 6);

          //show_mask(at_m);
        }
      }
    }
  }
  at_masks.push_back(at_m);
  //zapisanie do vectorow dla kazfej maski
}

void save_masks_pairs()
{
  for(size_t i=0; i<SIZE_MASKS; i++)
  {
    for(size_t j=0; j<SIZE_MASKS; j++)
    {
      if( (av_masks[i] & at_masks[j]) == av_masks[i] )
      {
        prev_ok_masks[i].push_back(j);
      }
    }
  }
}

bool wrong_mask(const std::bitset<FIELDS> & a)
{
  return  (a.test(0) & a.test(5) || (a.test(2) & a.test(3)) );
}

bool is_attacking_field(const int i,const int j)
{
  return (i == 3 || i == 2) && (j==0 || j==1 || j==2);
}

void show_mask(const std::bitset<FIELDS> & a)
{
  for(int i=0; i<2; i++)
  {
    for(int j=0; j<3; j++)
    {
      std::cout << a.test(i*3+j) << "";
    }
    std::cout << "\n";
  }
}

void show_all_masks()
{
  for(size_t i=0; i<av_masks.size(); i++)
  {
    show_mask(av_masks[i]);
    show_mask(at_masks[i]);
    std::cout << "\n";
  }
}

void show_output()
{
  for(size_t i=0; i<n; i++)
  {
    for(int j=0; j<3; j++)
    {
      printf("%c", board[i][j]);
    }
    printf("\n" );
  }
}

void print_board()
{
  for(size_t i=start_of_board; i<n; i++)
    {
      for(size_t j=0; j<3; j++)
      {
        printf("%c",board[i][j] );
      }
      printf("\n" );
    }
}

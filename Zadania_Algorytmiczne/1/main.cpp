#include <cstdio>
#include <queue>
#define MAX 2001

char fields[MAX][MAX];
bool visited[MAX][MAX];
std::queue<std::pair<int,int> > kol;

int cities = 0,n,m;

void bfs();

int main()
{
  scanf("%d %d",&n,&m);getchar();
  for(int i = 0; i < n; i++)
  {
    fgets(fields[i],MAX+1,stdin);
    for(int j=0; j < m; j++ )
    {
      visited[i][j] = false;
    }
  }//my_test();return 0;
  for(int i=0; i<n; i++)
  {
    for(int j=0; j<m; j++)
    {
      if(!visited[i][j])
      {
        visited[i][j] = true;
        if(fields[i][j] != 'A' )
        {
          cities++;
          kol.push(std::make_pair(i,j));
          bfs();
        }
      }
    }
  }
  printf("%d\n",cities );
  return 0;
}



void bfs()
{
  while(!kol.empty())
  {
    int i = kol.front().first, j= kol.front().second;
    kol.pop();
    if(fields[i][j] == 'B' || fields[i][j] == 'E' || fields[i][j] == 'F')
    {
      if( !(i+1 < 0 || i+1 >= n ) ){
        if(!visited[i+1][j])
        {
          if( fields[i+1][j] == 'C' || fields[i+1][j] == 'D' || fields[i+1][j] == 'F')
          {
            visited[i+1][j] = true;
            kol.push(std::make_pair(i+1,j));
          }
        }
      }
    }
    if(fields[i][j] == 'B' || fields[i][j] == 'C' || fields[i][j] == 'F')
    {
      if( !( j-1 < 0 || j-1 >= m) ){
        if(!visited[i][j-1])
        {
          if( fields[i][j-1] == 'D' || fields[i][j-1] == 'E' || fields[i][j-1] == 'F')
          {
            visited[i][j-1] = true;
          kol.push(std::make_pair(i,j-1));
          }
        }
      }
    }
    if(fields[i][j] == 'C' || fields[i][j] == 'D' || fields[i][j] == 'F')
    {
      if( !(i-1 < 0 || i-1 >= n ) ){
        if(!visited[i-1][j])
        {
          if( fields[i-1][j] == 'B' || fields[i-1][j] == 'E' || fields[i-1][j] == 'F')
          {
            visited[i-1][j] = true;
            kol.push(std::make_pair(i-1,j));
          }
        }
      }
    }
    if( (fields[i][j] == 'D') || (fields[i][j] == 'E') || (fields[i][j] == 'F') )
    {
      if( !( j+1 < 0 || j+1 >= m) ){
        if(!visited[i][j+1])
        {
          if( fields[i][j+1] == 'C' || fields[i][j+1] == 'B' || fields[i][j+1] == 'F')
          {
            visited[i][j+1] = true;
            kol.push(std::make_pair(i,j+1));
          }
        }
      }
    }
  }
}

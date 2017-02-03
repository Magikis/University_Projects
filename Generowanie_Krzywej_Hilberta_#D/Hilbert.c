#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.1415926535
typedef double Point[3];
typedef double Matrix[3][3];
enum alfa {moveto, lineto};
//               Dane wejściowe
double step_length;
int DW_n;
double DW_s;
double DW_u;
double DW_d;
double DW_x;
double DW_y;
double DW_z;
double DW_KatX;
double DW_KatY;
//					Matrix Obrotu
 Matrix rotXm = {{1,0,0},{0,0,-1},{0,1,0}};
 Matrix rotXp = {{1,0,0},{0,0,1},{0,-1,0}};
 Matrix rotYm = {{0,0,1},{0,1,0},{-1,0,0}};
 Matrix rotYp = {{0,0,-1},{0,1,0},{1,0,0}};
 Matrix rotZm = {{0,-1,0},{1,0,0},{0,0,1}};
 Matrix rotZp = {{0,1,0},{-1,0,0},{0,0,1}};
 Matrix id = {{1,0,0},{0,1,0},{0,0,1}};
// 		ZMIENNE GLOBALNE
Matrix TURN;
Matrix myrotations[8];
Point steps[7]={
{0,-1,0},
{-1,0,0},
{0,1,0},
{0,0,1},
{0,-1,0},
{1,0,0},
{0,1,0}
};

double radians(double a){
	return PI * (a / 180.0);
}
//		SKLADANIE OBROTOW
void multiplication( Matrix a, Matrix b, Matrix result){
    for(int i=0; i<3; i++){
        for (int j=0; j<3; j++){
            result[i][j]=0;
            for(int z=0; z<3; z++){
                result[i][j]+=a[i][z]*b[z][j];
            }
        }
    }

}
// OBRACANIE WEKTORA
void rotate( Matrix m, Point v, Point result){
    result[0] = result[1] = result[2] = 0;
    for (int i=0; i<3; i++) {
        result[0]+= v[i] * m[0][i];
        result[1]+= v[i] * m[1][i];
        result[2]+= v[i] * m[2][i];
    }

}
// TWORZENIE MACIERZY OBROTU DLA 8 KOSTEK n-1
void make_myrotations(void){
	multiplication(rotZp, rotYm, myrotations[0]);
	multiplication(rotZm, rotXp, myrotations[1]);
	multiplication(rotZm, rotXp, myrotations[2]);
	multiplication(rotZp, rotZp, myrotations[3]);
	multiplication(rotZp, rotZp, myrotations[4]);
	multiplication(rotZm, rotXm, myrotations[5]);
	multiplication(rotZm, rotXm, myrotations[6]);
	multiplication(rotYp, rotXp, myrotations[7]);
}
// PRZESKALOWANIE VEKTOROW TWORZENIA KOSTKI n=1
void make_steps(void){
	for(int i=0; i<7; i++){
		for(int j=0; j<3; j++){
			*(*(steps+i)+j)*=step_length;
		}
	}
}
// PRZESUNIECIE PUNKTU O WEKTOR
void move( Point vec, Point p){
    for(int i=0; i<3; i++){
        p[i]+= vec[i];
    }
}
// DO DEBUGINGU
void printpoint( Point p){
	printf("%lf\t%lf\t%lf\n",p[0],p[1],p[2]);
}
// RZUTOWANIE
void Projection(Point p, int activity){
    double x= DW_x + ((-DW_d) * (p[0] - DW_x) / (-DW_d + p[2]));
    double y= DW_y + ((-DW_d) * (p[1] - DW_y) / (-DW_d + p[2]));
    printf("%lf %lf",x,y);
    if(activity==lineto)   printf(" lineto\n");
    else if(activity==moveto)   printf(" moveto\n");
}
// 1) NALOZENIE KOSTKI NA SRODEK UKL WSP. 2) OBROT. 3) PRZESUNIECIE NA UKL WSP Z WEJSCIA
void ProjectionPrepare( Point p, int activity){
    Point temp;
    for(int i=0; i<3; i++){
    	temp[i]=p[i];
    }
    temp[0]+=(DW_u/2.0);
    temp[1]+=(DW_u/2.0);
    temp[2]-=(DW_u/2.0);
    Point temp2;
    rotate(TURN, temp, temp2);
    temp2[0]+=DW_x;
    temp2[1]+=DW_y;
    temp2[2]+=DW_z;
    Projection(temp2,activity);
}

void hilbert(int n, Point p, Matrix m, Matrix TURN){
	if(n==0) return;
	Matrix rot;
	Point vector;
	for(int i=0; i<8; i++){
		multiplication(m,myrotations[i],rot);
		hilbert(n-1, p, rot, TURN);
		if(i==7) break;
		rotate(m,steps[i],vector);
		move(vector,p);		
		ProjectionPrepare(p,lineto);
//		printpoint(p);
	}
}

int LengthOfSteps(int n){
    if(n==1)    return 1;
    int result=LengthOfSteps(n-1);
    return result*2+1;
}

int main(int argc,char *args[]){
    if(argc!=10) {
    	fprintf(stderr,"Zle dane. Przyklad: %s n s u d x y z KatX KatY\n",args[0]);
    	return 0;
    }
    int temp=0;
    temp+=sscanf(args[1], "%d", &DW_n);
    temp+=sscanf(args[2], "%lf", &DW_s);
    temp+=sscanf(args[3], "%lf", &DW_u);
    temp+=sscanf(args[4], "%lf", &DW_d);
    temp+=sscanf(args[5], "%lf", &DW_x);
    temp+=sscanf(args[6], "%lf", &DW_y);
    temp+=sscanf(args[7], "%lf", &DW_z);
    temp+=sscanf(args[8], "%lf", &DW_KatX);
    temp+=sscanf(args[9], "%lf", &DW_KatY);
    if(temp!=9)	{
    	fprintf(stderr,"Zle dane. Przyklad: %s n s u d x y z KatX KatY\n",args[0]);
    	return 0;
    	}
    printf("%%!PS-Adobe-2.0 EPSF-2.0\n");
    printf("%%%%BoundingBox: 0 0 ");
    printf("%lf %lf\n",DW_s, DW_s);
    printf("newpath\n");
	step_length=DW_u / LengthOfSteps(DW_n);
//	step_length=1;
	make_myrotations();
	make_steps();
	Point p;
	for(int i=0; i<3; i++){
	p[i]=0;
	}
	DW_KatX=radians(DW_KatX);
	DW_KatY=radians(DW_KatY);
	
	Matrix OX={
	{1,0,0},
	{0, cos(DW_KatX), -sin(DW_KatX)},
	{0, sin(DW_KatX), cos(DW_KatX)}	
	};
	
	Matrix OY={
	{ cos(DW_KatY), 0, sin(DW_KatY)	},
	{ 0, 1, 0	},
	{ -sin(DW_KatY), 0, cos(DW_KatY)	},
	};
	
	multiplication(OX, OY, TURN);
	ProjectionPrepare(p, moveto);
	hilbert(DW_n,p, id,TURN );
	printf(".4 setlinewidth\n");
	printf("stroke\n");
	printf("showpage\n");
	printf("%%%%Trailer\n");
  	printf("%%EOF");				
	return 0;
}

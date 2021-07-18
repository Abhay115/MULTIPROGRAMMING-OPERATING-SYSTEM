#include<iostream>
#include<fstream>
#include<string.h>
#include<stdlib.h>
using namespace std;
/*ASSUMPTIONS:
 Jobs entered without error in input file
 No physical separation between jobs
 Job outputs separated in output file by 2 blank lines
 Program loaded in memory starting at location 00
 No multiprogramming, load and run one program at a time
 SI interrupt for service request
*/
char M[100][4];     //Physical Memory
char IR[4];         //Instruction Register (4 bytes)
char R[4];          //General Purpose Register (4 bytes)
int IC;             //Instruction Counter Register (2 bytes)
int SI;             //Interrupt
bool C;             //Toggle (1 byte)
char buffer[40];
fstream infile;
fstream outfile;

void init()
{

    for(int i = 0; i<100; i++)
    {
        for(int j = 0; j<4; j++ )
        {
            M[i][j]=' ';

        }

    }


    IR[0] = {' '};
    R[0] = {' '};
    C = false;

}
void readf()
{

    if(SI == 1)     //Read Mode
    {
        for(int i=0;i<=39;i++)
           buffer[i]='\0';

        infile.getline(buffer,40);

        int k = 0;
        int i = IR[2]-48;
        i = i*10;


        for( int l=0 ; l<10 ;  ++l)
        {
            for(int j = 0 ; j<4; ++j)
            {
                M[i][j] = buffer[k];
                k++;
            }
            if(k == 40)
            {
                break;
            }
            i++;
        }

        /*for(int i = 0; i<100; i++)
        {
            cout<<"M["<<i<<"]\t";
            for(int j = 0; j<4; j++ )
            {
                cout<<M[i][j];
            }
            cout<<endl;
        }*/
    }
}

void writef()
{
    if(SI == 2)        //Write Mode
    {
      for(int i=0;i<=39;i++)
           buffer[i]='\0';

        int k = 0;
        int i = IR[2]-48;
        i = i*10;


        for( int l=0 ; l<10 ;  ++l)
        {
            for(int j = 0 ; j<4; ++j)
            {
                buffer[k]=M[i][j];
                outfile<<buffer[k];

                k++;
            }
            if(k == 40)
            {
                break;
            }
            i++;
        }
          /* for(int i = 0; i<100; i++)
        {
            cout<<"M["<<i<<"]\t";
            for(int j = 0; j<4; j++ )
            {
                cout<<M[i][j];
            }
            cout<<endl;
        }*/

        outfile<<"\n";

    }
}
void ter()
{
    if(SI == 3)        //Terminate
    {

        outfile<<"\n";
        outfile<<"\n";

    }

}

void Execute()
{
    while(true)
    {
        for(int i = 0; i<4; i++)        //Load in register
        {
            IR[i] = M[IC][i];
        }
        IC++;

        if(IR[0] == 'G' && IR[1] == 'D')    //GD
        {
            SI = 1;
            readf();
        }
        else if(IR[0] == 'P' && IR[1] == 'D')       //PD
        {
            SI = 2;
            writef();
        }
        else if(IR [0] == 'H')      //H
        {
            SI = 3;
            ter();
            break;
        }
        else if(IR[0] == 'L' && IR[1] == 'R')       //LR
        {
            int i = IR[2]-48;
            i = i*10 + ( IR[3]-48);

            for(int j=0;j<=3;j++)
                R[j]=M[i][j];

            //for(int j=0;j<=3;j++)
              // cout<<R[j];

           // cout<<endl;
        }
        else if(IR[0] == 'S' && IR[1] == 'R')       //SR
        {
            int i = IR[2]-48;
            i = i*10 +( IR[3]-48) ;
            //cout<<i;
            for(int j=0;j<=3;j++)
                M[i][j]=R[j];

            //cout<<endl;
        }
        else if(IR[0] == 'C' && IR[1] == 'R')       //CR
        {
            int i = IR[2]-48;
            i = i*10 + (IR[3] - 48);
            //cout<<i;
            int count=0;

            for(int j=0;j<=3;j++)
                if(M[i][j] == R[j])
                    count++;

            if(count==4)
                C=true;

            //cout<<C;
        }
        else if(IR[0] == 'B' && IR[1] == 'T')       //BT
        {
            if(C == true)
            {
                int i = IR[2]-48;
                i = i*10 + (IR[3] - 48);

                IC = i;

            }
        }
    }
}
void startexecution()
{
    IC = 00;
    Execute();
}
void LOAD()
{


    cout<<"Reading Data from memory"<<endl;
    int x=0;
    do
    {
        for(int i=0;i<=39;i++)      
           buffer[i]='\0';

        infile.getline(buffer,40);

        for(int k=0;k<=39;k++)
           cout<<buffer[k];
           cout<<"\n";

        if(buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J')
        {
            init();

        }
        else if(buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A')
        {
            startexecution();
            

        }
        else if(buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
        {
            x=0;
            continue;
        }
        else
        {
            int k = 0;

            for(; x <100; ++x)
            {
                for(int j = 0 ; j<4; ++j)
                {

                    M[x][j] = buffer[k];
                    k++;
                }

                if(k == 40 || buffer[k] == ' ' || buffer[k] == '\n')
                {
                    break;
                }

            }

        }

    }while(!infile.eof());         //continues to take input till eof


}
int main()
{
    infile.open("newinput.txt");
    outfile.open("output.txt");

    LOAD();

    return 0;
}
/*
$AMJ000100030001
GD10PD10H
$DTA
Hello World-
$END0001
$AMJ0001000120004
GD20GD30GD40GD50PD20PD30LR20CR30BT11
PD50HPD40H
$DTA
VIT
VIIT 
SAME
NOT SAME
$END0001
$AMJ0003000120003
GD20LR20SR32LR21SR31LR22SR30PD30H
$DTA
_how_areyou_
$END0002
$AMJ0005000170003		
GD40GD20GD30PD40LR40CR43BT09PD20PD30
SR40LR41CR42BT14PD20PD30H
$DTA
0___1___1___0___
NOT
PALINDROME
$END0005
*/
#include<stdio.h>
#include<math.h>
void add(int a,int b);
void sub(int a,int b);
void mul(int a,int b);
void div(int a,int b);
void mod(int a,int b);
int main()
{
    int a,b;
    char op,choice;
    while(1)
    {
       printf("enter two opreands(a and b):\n");
       scanf("%d %d",&a,&b);
       printf("enter a opreator(+,-,*,/,%%):\n");
       scanf(" %c",&op);
       switch(op)
       {
           case'+':
           add(a,b);
           break;

           case'-':
           sub(a,b);
           break;

           case'*':
           mul(a,b);
           break;

           case'/':
           div(a,b);
           break;

           case'%':
           mod(a,b);
           break;

           default:
           printf("INVALIDE OPERATOR!(kuch toh glat bhra h bhai bera h mena)");
        }
        printf("\n\ndo you want to calculate it again (y/n):");
        scanf(" %c",&choice);
        
        if(choice=='y' || choice=='Y')
        {
          continue;
        }
        else if(choice=='n' || choice=='N')
        {
          printf("calculator closed!!!");
          break;
        }
        else
        {
          printf("invalide choice, please enter your choice in 'y' and 'n' only!!!!");
          continue;
        }
    }

  return 0;
}    
    void add(int a,int b)
   {
       printf("%d+%d=%d",a,b,a+b);
    }
    void sub(int a,int b)
   {
       printf("%d-%d=%d",a,b,a-b);
    }
    void mul(int a,int b)
   {
       printf("%d*%d=%d",a,b,a*b);
    }
    void div(int a,int b)
   {
       printf("%d/%d=%d",a,b,a/b);
    }
    void mod(int a,int b)
   {
       printf("%d%%%d =%d",a,b,a%b);
    }


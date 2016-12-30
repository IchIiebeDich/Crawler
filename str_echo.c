#include "unp.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define MaxSize 50
#define BUFFER_SIZE 1024

typedef struct
{
	float data[MaxSize];
	int top;
}OpStack;

typedef struct
{
	char data[MaxSize];
	int top;
}SeqStack;

void InitStack(SeqStack *S);
int StackEmpty(SeqStack S);
int PushStack(SeqStack *S,char e);
int PopStack(SeqStack *S,char *e);
int GetTop(SeqStack S,char *e);
void TranslateExpress(char s1[],char s2[]);
float ComputeExpress(char s[], int *p);


void InitStack(SeqStack *S)
{
	S->top=0;
}

int StackEmpty(SeqStack S)
{
	if(S.top ==0)
	return 1;
	else
	return 0;
}

int PushStack(SeqStack *S,char e)
{
	if(S->top>=MaxSize)
	{
		printf("The Stack is full!\n");
		return 0;
	}
	else
	{
		S->data[S->top]=e;
		S->top++;
		return 1;
	}
}

int PopStack(SeqStack *S,char *e)
{
	if(S->top==0)
	{
		printf("The Stack is empty\n");
		return 0;
	}
	else
	{
		S->top--;
		*e=S->data[S->top];
		return 1;
	}
}
int GetTop(SeqStack S,char *e)
{
	if(S.top<=0)
	{
		printf("The stack is empty\n");
		return 0;
	}
	else
	{
		*e=S.data[S.top-1];
		return 1;
	}
}

void TranslateExpress(char str[],char exp[])
{
	SeqStack S;
	char ch;
	char e;
	int i=0,j=0;
	InitStack(&S);
	ch=str[i];
	i++;
	while(ch!='\0') 
	{
		switch(ch)
		{
		case'(':
				PushStack(&S,ch);
				break;
		case')':
				while(GetTop(S,&e)&&e!='(')
				{
					PopStack(&S,&e);
					exp[j]=e;
					j++;
				}
				PopStack(&S,&e);
				break;
		case'+':
		case'-':
				while(!StackEmpty(S)&&GetTop(S,&e)&&e!='(')
				{
					PopStack(&S,&e);
					exp[j]=e;
					j++;
				}
				PushStack(&S,ch);
				break;
		case'*':
		case'/':
				while(!StackEmpty(S)&&GetTop(S,&e)&&e=='/'||e=='*')
				{
					PopStack(&S,&e);
					exp[j]=e;
					j++;
				}
				PushStack(&S,ch);
				break; 
		case' ':
		break;
		default:
				while(ch>='0'&&ch<='9')
				{
				exp[j]=ch;
				j++;
				ch=str[i];
				i++;
				}
				i--;
				exp[j]=' ';
				j++;
		}
	ch=str[i];
	i++;
	}
	while(!StackEmpty(S)) 
	{
		PopStack(&S,&e);
		exp[j]=e;
		j++;
	}
	exp[j]='\0';
}

float ComputeExpress(char a[], int *p)
{
	OpStack S;
	int i=0;
	float x1,x2,value;
	float result;
	S.top=-1;
	while(a[i]!='\0') 
	{
		if(a[i]!=' '&&a[i]>='0'&&a[i]<='9')
		{
			value=0;
			while(a[i]!=' ') 
			{
				value=10*value+a[i]-'0';
				i++;
			}
			S.top++;
			S.data[S.top]=value; 
		}
		else 
		{
			switch(a[i])
			{
				case'+':
						x1=S.data[S.top];
						S.top--;
						x2=S.data[S.top];
						S.top--;
						result=x1+x2;
						S.top++;
						S.data[S.top]=result;
						break;

				case'-':
						x1=S.data[S.top];
						S.top--;
						x2=S.data[S.top];
						S.top--;
						result=x2-x1;
						S.top++;
						S.data[S.top]=result;
						break;

				case'*':
						x1=S.data[S.top];
						S.top--;
						x2=S.data[S.top];
						S.top--;
						result=x1*x2;
						S.top++;
						S.data[S.top]=result;
						break;

				case'/':
						x1=S.data[S.top];
						if(x1 == 0){
							printf("wrong input.\n");
							*p = 0;
							return 0;
						}
						S.top--;
						x2=S.data[S.top];
						S.top--;
						result=x2/x1;
						S.top++;
						S.data[S.top]=result;
						break;
				
			}
			i++;
		}
	}
	if(!S.top!=-1) 
	{
		result=S.data[S.top];
		S.top--;
		if(S.top==-1)
			return result;
		else
		{
			printf("Wrong input\n");
			*p = 0;	
			return 0;
		}
	}
	return 0;
}

void calculator(char buf[],int sockfd){

	char postFix[MaxSize];
	TranslateExpress(buf,postFix);
	int flag = 1;
	float val = ComputeExpress(postFix,&flag);
	if(flag == 0){
		char error[20] = "Wrong input.\n";
		Writen(sockfd, error, strlen(error));
	}
	else{
		printf("=%.2lf\n",val);
		char send[20];
		snprintf(send,sizeof(send), "%.2lf\n",val);
		Writen(sockfd, send, strlen(send));
	}
}

void HomeMenu(int sockfd){
	char  mark[100] = "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n"; 
	Writen(sockfd, mark, strlen(mark)); 
	char  home[120] = "     The server have three functions\n     (Send 1,2,3 or send '/exit/' to return to the home)\n"; 
	Writen(sockfd, home, strlen(home)); 
	char  func1[35] = "     1.print the content.\n"; 
	Writen(sockfd, func1, strlen(func1)); 
	char  func2[25] = "     2.calculation.\n"; 
	Writen(sockfd, func2, strlen(func2)); 
	char  func3[40] = "     3.send file.(-s file_name)\n"; 
	Writen(sockfd, func3, strlen(func3)); 
	Writen(sockfd, mark, strlen(mark));
}

void RecvFile(int sockfd, char file_name[], int filelength){

	char buffer[BUFFER_SIZE];
	FILE *fp = fopen(file_name, "w");
	if(NULL == fp){
		printf("File:%s Open Error.\n",file_name);
		return ;
	}
	printf("begin to write to %s\n",file_name);
	bzero(buffer, BUFFER_SIZE);
	int length = 0;
	int getlength = 0;
	int t = 1;
	while(getlength < filelength){
		length = recv(sockfd, buffer, BUFFER_SIZE, 0);
		if(fwrite(buffer, sizeof(char), length, fp) < length){
			printf("File:%s Write Failed\n",file_name);
			break;
		}
		getlength += length;
		bzero(buffer, BUFFER_SIZE);
	}

	fclose(fp);
	printf("Receive File:%s Successful!\n",file_name);

}

#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;

char *itoa(unsigned long);

int main()
{
	cout << "enter number: ";
	int xyzzy=0;
	cin >> xyzzy;
	cout << itoa(xyzzy) << endl;
	return 0;
}

char *ltoa(unsigned long rval)
{
	int n=2; // magic number, don't change.
	
	if(rval / 10 <= 0){
		char *x=new char[n];
		if(snprintf(x,n,"%ul",rval)==-1)
			return NULL;
		return x;
	} 
	
	int tmp=rval;
	while(tmp / 10 > 0){
		tmp = tmp / 10;
		n++;
	}
	
	char *x=new char[n];
	if(snprintf(x,n,"%ul",rval)==-1)
		return NULL;
	return x;
}

// unsigned long
// unsigned int
// bool

#include<iostream>
#include<fstream>
#include<string>
#include <sstream>
#include<cstdlib>
using namespace std;

int main()
{
	int pNum =0;
	cout<<"how mamy points you want?"<<endl;
	cin>>pNum;
	string s;
	stringstream ss(s);
	ss<<pNum;
	string outName = "randP-"+ss.str()+".txt";
	
	ofstream fout(outName.c_str());
	
	for(int i=0;i<pNum;i++)
	{
		fout<< (1+(rand() %10000))<<" "<<(1+(rand() %10000));
			if(i+1<pNum)
				fout<<endl;
	}

	fout.close();
	return 0;
}
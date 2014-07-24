/*****************************************

**Simple Rtree and BBS implementation**

:Author: Otto Huang
:Contact: xhomegg@gmail.com
:Revision: 1.0 beta
:Date: 21/07/2014

basic algorithm :Hilbert_R-tree
see more:http://en.wikipedia.org/wiki/Hilbert_R-tree

*******************************************/



#include<cstdlib>
#include<cstdio>
#include<iostream>
#include<fstream>
#include<utility>
#include<vector>
#include<cmath>
#include<algorithm>
#include<string>
#define MAX 10000 //max dataset size
using namespace std;


typedef struct n
{
	int eNum; //entry num
	vector<struct n> entry;
	int leafID;
	int num;
	int MBR;
	int level;
	pair<int,int> parentID;
	pair<int,int> min;
	pair<int,int> max;

}Node;

vector<Node> Rtree;
int M = 0; //the max entry size;
int m = 0; // the min entry size;
int leafNum =1;
int currentUse = 0;// the index of not used node array 

void insert(pair<int,int>);
int chooseLeaf(pair<int,int> );
pair<pair<int,int>,pair<int,int>> MBRP(Node,Node) ;
int area(pair<int,int>,pair<int,int>);
pair<int,int> pickSeeds(Node,Node);
int pickNext(Node ,Node ,Node );
void spilt(int,Node);
vector<Node> bbs(vector<Node> rTree);
bool cmp_node(Node ,Node );

int main(int argc, char* argv[])
{

	cout << "Please enter the max entry size:";
	cin >> M;
	m = M/2;
	
	
	if(argc!=2)
	{
		cout<<"No file input"<<endl;
		cin.get();
		return 1;
	}

	ifstream fin(argv[1]);
	
	if(!fin)
	{
		cout<<"No such file"<<endl;
		cin.get();
		return 1;
	}

	cout<<"Running....."<<endl;

	while(!fin.eof())
	{	
		pair<int,int> in;
		fin>>in.first >> in.second;

		insert(in);
	};
	fin.close();
	
	vector<Node> skyline = bbs(Rtree);
	string outName(argv[1]);
	int dotPosition = outName.find(".",0);
	outName = outName.insert(dotPosition,"-out");
	ofstream fout(outName.c_str());
	for(vector<Node>::iterator sk = skyline.begin();sk!= skyline.end();sk++)
	{
		fout<<sk->min.first<<" "<<sk->min.second<<endl;
	}
	cout<<"Complete"<<endl;
	cin.get();
	return 0;
}

void insert(pair<int,int> p)
{
	int currentNode = chooseLeaf(p);
	
	Node e ;
	e.parentID = make_pair(-2,-2);
	e.leafID = -1;
	e.num = -1;
	e.min = p;
	e.max = p;
	e.MBR = 0;
	
	if(Rtree[currentNode].eNum < M)
	{
		
		

		//更新 min 和 max
		pair<pair<int,int>,pair<int,int>> minMax = MBRP(Rtree[currentNode] , e);
		Rtree[currentNode].min = minMax.first;
		Rtree[currentNode].max = minMax.second;
		Rtree[currentNode].MBR = area(Rtree[currentNode].min,Rtree[currentNode].max);
		
		//int parentID = Rtree[currentNode].parentID.first;

		/*pair<pair<int,int>,pair<int,int>> minMax = MBRP(Rtree[parentID] ,Rtree[currentNode]);
		Rtree[parentID].min = minMax.first;
		Rtree[parentID].max = minMax.second;
		Rtree[parentID].MBR = area(Rtree[parentID].min,Rtree[parentID].max);*/
		
		int tmpCurrent = currentNode;
		int parentID = Rtree[tmpCurrent].parentID.first;
		//cout<<"current"<<currentUse<<endl;
		while(parentID >=0)
		{
			pair<pair<int,int>,pair<int,int>> minMax = MBRP(Rtree[parentID] ,Rtree[tmpCurrent]);
			
			Rtree[parentID].min = minMax.first;
			
			Rtree[parentID].max = minMax.second;
			Rtree[parentID].MBR = area(Rtree[parentID].min,Rtree[parentID].max);
			
			int chgEntry = Rtree[tmpCurrent].parentID.second;
		
			//Rtree[parentID].entry[chgEntry] = Rtree[tmpCurrent];
				
			tmpCurrent = parentID;
			parentID = Rtree[tmpCurrent].parentID.first;
	
		}
		
		//cout<< "currentNode : " <<  currentNode <<endl;
		//cout<< "input point : (" <<p.first<<" , "<<p.second<<")"<<endl;
		//cout<< "MBR : " <<Rtree[currentNode].MBR<<endl;
		//system("pause");
		
		Rtree[currentNode].entry.push_back(e);
		Rtree[currentNode].eNum++;
	}
	else //the node's entry is full
	{
		spilt(currentNode,e);
	}
}

void spilt(int index,Node newNode)
{	
	
	Node L,LL;
	if(Rtree[index].leafID !=1)
	{
		L.leafID = 0;
		LL.leafID = 0;
	}
	else
	{
		L.leafID = 1;
		LL.leafID = 1;
	}
	
	L.max = make_pair(-1,-1);
	L.min = make_pair(-1,-1);
	L.MBR = 0;
	L.eNum = 0;
	L.num = index;
	L.parentID = Rtree[index].parentID;
	LL.max = make_pair(-1,-1);
	LL.min = make_pair(-1,-1);
	LL.MBR = 0;
	LL.eNum = 0;
	LL.num = currentUse++;

	pair<int,int> seeds;
	seeds = pickSeeds(Rtree[index],newNode);

	Rtree[index].entry.push_back(newNode);
	
	L.entry.push_back(Rtree[index].entry[seeds.first]);
	LL.entry.push_back(Rtree[index].entry[seeds.second]);
	L.eNum++;
	LL.eNum++;
	//cout<<"seeds : ("<<seeds.first<<" , "<<seeds.second<<" )"<<endl;

	//更新MBR 及min max
	
	pair<pair<int,int>,pair<int,int>> twoP = MBRP(L,Rtree[index].entry[seeds.first]);
	L.min = twoP.first;
	L.max = twoP.second;
	L.MBR  = area(L.min,L.max );

	pair<pair<int,int>,pair<int,int>> twoP2 = MBRP(LL,Rtree[index].entry[seeds.second]);
	LL.min = twoP2.first;
	LL.max = twoP2.second;
	LL.MBR  = area(LL.min,LL.max);

	//pickNext
	Node lessE ;

	for(int i=0;i<Rtree[index].entry.size();i++)
	{
		if(i==seeds.first || i== seeds.second)
			continue;
		lessE.entry.push_back(Rtree[index].entry[i]);
		//cout<<"( "<<Rtree[index].entry[i].min.first<<Rtree[index].entry[i].min.second<<" )"<<endl;
	}
	int lessSize  = lessE.entry.size();
	for(int i=0;i<lessSize;i++)
	{
		int next = pickNext(L,LL,lessE);
		//cout<<next<<endl;

		//choose one of the two nodes by MBR
		pair<pair<int,int>,pair<int,int>> lp = MBRP(L,lessE.entry[next]);
		int d1 =  area(lp.first,lp.second) - area(L.min,L.max);
		pair<pair<int,int>,pair<int,int>> llp = MBRP(LL,lessE.entry[next]);
		int d2 =  area(llp.first,llp.second) - area(LL.min,LL.max);
		
		if(d1<d2)
		{
			//cout<<"add to L"<<endl;
			//cout<<"L :(" <<L.min.first<<L.min.second<<")"<<endl;
			//cout<<"in :(" <<lessE.entry[next].min.first<<lessE.entry[next].min.second<<")"<<endl;
			if(lessE.entry[next].leafID >=0)
			{
				//cout<<"&& :"<<lessE.entry[next].num<<endl;
				Rtree[lessE.entry[next].num].parentID =make_pair(L.num,L.eNum);
			}
				 //Rtree[lessE.entry[next].num].parentID = make_pair(L.num,0);
			L.entry.push_back(lessE.entry[next]);
			L.eNum++;
			pair<pair<int,int>,pair<int,int>> twoP = MBRP(L,lessE.entry[next]);
			L.min = twoP.first;
			L.max = twoP.second;
			L.MBR  = area(L.min,L.max );

			int parentID = L.parentID.first;
			 Node tmpL = L;
			while(parentID >= 0)
			{
				
				pair<pair<int,int>,pair<int,int>> twoParent = MBRP(Rtree[parentID],tmpL);
				Rtree[parentID].min = twoParent.first;
				Rtree[parentID].max = twoParent.second;
				Rtree[parentID].MBR  = area(Rtree[parentID].min,Rtree[parentID].max );
				tmpL = Rtree[parentID];
				parentID = Rtree[parentID].parentID.first;
			
			}

		}
		else
		{
			//cout<<"add to LL"<<endl;
			//cout<<"LL :(" <<LL.min.first<<LL.min.second<<")"<<endl;
			//cout<<"in :(" <<lessE.entry[next].min.first<<lessE.entry[next].min.second<<")"<<endl;
			if(lessE.entry[next].leafID >=0)
			{
				//cout<<"&& :"<<lessE.entry[next].num<<endl;
				Rtree[lessE.entry[next].num].parentID = make_pair(LL.num,LL.eNum);
			}
				//Rtree[lessE.entry[next].num].parentID = make_pair(LL.num,0);
			
			LL.entry.push_back(lessE.entry[next]);
			LL.eNum++;
			pair<pair<int,int>,pair<int,int>> twoP2 = MBRP(LL,lessE.entry[next]);
			LL.min = twoP2.first;
			LL.max = twoP2.second;
			LL.MBR  = area(LL.min,LL.max);
		
			
			
		}

		lessE.entry.erase(lessE.entry.begin()+next);
		
		//system("pause");
	}

	//check if add new root

	int parentID = Rtree[index].parentID.first;
	if(parentID == -1) //spilt root node
	{
		//cout<<"L :(" <<L.min.first<<L.min.second<<")"<<endl;
		//cout<<"LL :(" <<LL.min.first<<LL.min.second<<")"<<endl;
		Node root;
		root.eNum =2;
		root.parentID = make_pair(-1,-1);
		root.leafID  =0;

		
		
		L.parentID = make_pair(currentUse,0);
		LL.parentID = make_pair(currentUse,1);

		Rtree[index] = L;
		root.entry.push_back(L);
		root.entry.push_back(LL);
		
		pair<pair<int,int>,pair<int,int>> twoP = MBRP(L,LL);
		root.min =twoP.first;
		root.max =twoP.second;
		root.MBR = area(root.min,root.max);

		
		Rtree.push_back(LL);
		root.num = currentUse++;
		Rtree.push_back(root);
	
		
	}
	else
	{
		
		if(Rtree[parentID].eNum ==M) //父結點滿了
		{
			//cout<<"parent is full"<<endl;
			//cout<<"L :(" <<L.min.first<<L.min.second<<")"<<endl;
			//cout<<"index:"<<index<<endl;
			L.parentID = Rtree[index].parentID;
			Rtree[index] = L;

			Rtree[parentID].entry[L.parentID.second] = L;
			pair<pair<int,int>,pair<int,int>> twoP = MBRP(Rtree[parentID],L);
			Rtree[parentID].min =twoP.first;
			Rtree[parentID].max =twoP.second;
			Rtree.push_back(LL);
			Rtree[parentID].leafID = 0;
			
			spilt(parentID, LL);
			
			
		}
		else
		{
			
			L.parentID = Rtree[index].parentID;
			Rtree[index] = L;
			LL.parentID = make_pair( parentID,Rtree[parentID].eNum);
			Rtree[parentID].entry.push_back(LL);
			Rtree[parentID].eNum++;
			
			//更新min max MBR
			
			pair<pair<int,int>,pair<int,int>> twoP = MBRP(Rtree[parentID],L);
			Rtree[parentID].min =twoP.first;
			Rtree[parentID].max =twoP.second;

			pair<pair<int,int>,pair<int,int>> twoP2 = MBRP(Rtree[parentID],LL);
			Rtree[parentID].min =twoP2.first;
			Rtree[parentID].max =twoP2.second;
			Rtree[parentID].MBR = area(Rtree[parentID].min,Rtree[parentID].max);
			

			int tmpID = Rtree[Rtree[index].parentID.first].parentID.first;
			Node tmpL = Rtree[Rtree[index].parentID.first];
			
			while(tmpID>=0)
			{
				pair<pair<int,int>,pair<int,int>> twoP2 = MBRP(Rtree[tmpID],tmpL);
				Rtree[tmpID].min =twoP2.first;
				Rtree[tmpID].max =twoP2.second;
				Rtree[tmpID].MBR = area(Rtree[parentID].min,Rtree[tmpID].max);
				int chgEntry = tmpL.parentID.second;
				//Rtree[tmpID].entry[chgEntry] = tmpL;
				
				tmpL = Rtree[tmpID];
				tmpID = tmpL.parentID.first;
			}
			
			Rtree.push_back(LL);
			
		}
	
	}

	//system("pause");
}

int chooseLeaf(pair<int,int> p)
{
	int index = 0;
	if (currentUse == 0) //no any node
	{
		Node n;
		n.eNum = 0;
		n.leafID = 1;
		n.min = make_pair(-1,-1);
		n.max = make_pair(-1,-1);
		n.MBR = 0;
		n.parentID = make_pair(-1,-1);
		n.num = currentUse;
		Rtree.push_back(n);
		currentUse++;
		return index;
	}
	else // find the min increase MBR area
	{
		int min = 100000000000;
		int entryNum = 10000000000;
		int i=0;
		for(vector<Node>::iterator itr = Rtree.begin();itr!=Rtree.end();itr++,i++)
		{
		
				if(itr->leafID ==1) // leaf node
				{
					//cout<<"@@"<<itr->min.first<<itr->min.second<<endl;
					Node  tmpN;
					tmpN.max = p;
					tmpN.min = p;
					pair<pair<int,int>,pair<int,int>> twoP = MBRP(*itr,tmpN);
					int incrArea = area(twoP.first,twoP.second) -area(itr->min,itr->max);
					if(incrArea<=min)
					{
						if(incrArea == min)
						{
							if(itr ->eNum >= entryNum)
								continue;
						}
						min = incrArea;
						entryNum = itr ->eNum;
						index = i;
						//cout<<"@index"<<index<<endl;
					}
				}
		}
	}
	//cout<<"index:"<<index<<endl;
	return index;
}

pair<pair<int,int>,pair<int,int>> MBRP(Node node ,Node node2)
{
	int minX = node.min.first;
	int minY = node.min.second;
	int maxX = node.max.first;
	int maxY = node.max.second;

	int x = node2.min.first;
	int y = node2.min.second;
	int mx = node2.max.first;
	int my = node2.max.second;
				
	if(minX == -1) //first point
	{
		minX = x;
		minY = y;
		maxX = mx;
		maxY = my;
	}
	else
	{
		int xArray[4] = {minX,maxX,x,mx};
		int yArray[4] = {minY,maxY,y,my};

		for(int i=0;i<4;i++)
		{
			if(xArray[i]<minX)
				minX = xArray[i];
			if(xArray[i] >maxX)
				maxX = xArray[i];
		}
		for(int i=0;i<4;i++)
		{
			if(yArray[i]<minY)
				minY = yArray[i];
			if(yArray[i]>maxY)
				maxY = yArray[i];
		}
	}
	return make_pair(make_pair(minX,minY),make_pair(maxX,maxY));

}
int area(pair<int,int> a,pair<int,int> b)
{	
	return (abs(b.first-a.first) * abs(b.second-a.second));
}

pair<int,int> pickSeeds(Node n,Node newNode) //pick two seeds for two new nodes
{
	//add new point into entry

	n.entry.push_back(newNode);

	int bestD = 0;
	pair<int,int> bestP;
	for(int i=0;i < n.entry.size();i++)
	{
		for(int j=i+1;j<n.entry.size();j++)
		{
			pair<pair<int,int>,pair<int,int>> twoP = MBRP(n.entry[i],n.entry[j]);
			int totalA = area(twoP.first,twoP.second);
			int d = totalA - (n.entry[i].MBR) - (n.entry[j].MBR);
			if(d > bestD)
			{
				bestD = d;
				bestP = make_pair(i,j);
			}
		}
	}	
	return bestP;
}

int pickNext(Node l,Node ll,Node e)
{
	int max_d_val = 0;

	int index = 0;

	for(int i = 0;i< e.entry.size();i++)
	{
		
		int d1 = 0,d2 =0 ,dval = 0;
		pair<pair<int,int>,pair<int,int>> twoP = MBRP(l,e.entry[i]);
		d1 = area(twoP.first,twoP.second) - area(l.min,l.max);
		
		pair<pair<int,int>,pair<int,int>> twoP2 = MBRP(ll,e.entry[i]);
		d2 = area(twoP2.first,twoP2.second) - area(ll.min,ll.max);
		
		dval = abs(d1-d2);

		if(dval > max_d_val)
		{
			max_d_val = dval;
			index = i;
		}
	}
	return index;
}


vector<Node> bbs(vector<Node> rTree)
{
	vector<Node> heap;
	vector<Node> skyline;
	
	int rootNum = -1;
	for(vector<Node>::iterator it = rTree.begin();it!=rTree.end();it++)
	{
		if(it->parentID.first == -1)
		{
			rootNum = it->num;
			cout<<"root : "<<it->num<<endl;
			break;
		}
	}
	if(rootNum!=-1)
	{
		heap.push_back(rTree[rootNum]);
		while(!heap.empty())
		{
			sort(heap.begin(),heap.end(),cmp_node);
			//for(vector<Node>::iterator it = heap.begin();it!=heap.end();it++)
				//cout<<" "<<it->num;

			Node current = heap[0];
			if(current.leafID >=0)
			{
				
				for(vector<Node>::iterator it = current.entry.begin();it!= current.entry.end();it++)
				{
					
					int num = it->num;
					
					if(num== -1) //child is point
					{
						heap.push_back(*it);
					}
					else
					{
						if(!skyline.empty()) //skyline不為空
						{
							for(vector<Node>::iterator sk = skyline.begin();sk!= skyline.end();sk++)
							{
								if(sk->min.first <it->min.first && sk->min.second <it->min.second )
									break;
								else
									heap.push_back(rTree[num]);
							}
						}
						else
						heap.push_back(rTree[num]);
					}
				}
			}
			else //point
			{
				
				if(!skyline.empty()) //skyline不為空
					{
						vector<Node> tmpSk = skyline;
						for(vector<Node>::iterator sk = tmpSk.begin();sk!= tmpSk.end();sk++)
						{

							if(sk->min.first <=current.min.first && sk->min.second <=current.min.second )
								break;
							
							if(sk+1 == tmpSk.end())
							{
								
								vector<Node> tmpSk2 = tmpSk;
								for(int i=0;i<tmpSk.size();i++)
								{
									if(current.min.first <= tmpSk[i].min.first&&current.min.second <= tmpSk[i].min.second)
									{
										/*cout<<"test:"<<endl;
										cout<<current.min.first<<current.min.second<<endl;
										cout<<tmpSk[i].min.first<<tmpSk[i].min.second<<endl;
										cout<<"index:"<<i;
										cout<<"size:"<<tmpSk.size()<<endl;
										system("pause");*/
										for(vector<Node>::iterator sk2 = tmpSk2.begin();sk2!= tmpSk2.end();sk2++)
										{
											if(sk2->min.first == tmpSk[i].min.first&&sk2->min.second == tmpSk[i].min.second)
											{
												tmpSk2.erase(sk2);
												break;
											}
											
										}
										
									}
										
								}
								tmpSk2.push_back(current);
								skyline = tmpSk2;
							}
						}
					}
				else
					skyline.push_back(current);
			
			}
			
			heap.erase(heap.begin());
		}
	
	}

	return skyline;

}

bool cmp_node(Node a,Node b)
{
	return a.MBR < b.MBR;
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <list>
using namespace std;
template< typename T, class C = std::list<T> >
class stack{
public:
    typedef typename C::iterator iterator;
    iterator begin(){return cc.begin();}
    iterator end() {return cc.end();}
    void push(const T& vaule){ cc.push_front(vaule); }
    void pop(){cc.pop_front();}
    T top(){return cc.front(); }
private:
    C cc;
};

const int state_maxn = 300;
const int guiyue = 1<<20;
const int acc = 1<<31-1;
struct Lex
{
	string word;
	string type;
	Lex(string word, string type): word(word), type(type){}
};
vector<Lex> lex;

struct Node
{
	string left;
	vector<string> right;
	set<string> search;
	int position;

	Node(string left, vector<string> right, set<string> search, int position):
	left(left), right(right), search(search), position(position) {}

	bool operator < (Node const &other) const
	{
		if (position != other.position)
			return position < other.position;
		if (left != other.left)
			return left < other.left;
		if (right != other.right)
		{
			if (right.size() != other.right.size())
				return right.size() < other.right.size();
			for (int i = 0; i < right.size(); i++)
				if (right[i] != other.right[i])
					return right[i] < other.right[i];
		}
		if (search != other.search)
			return search < other.search;
		return false;
	}

	bool operator == (Node const &other) const
	{
		if (other.left == left && other.right == right &&
			other.search == search && other.position == position)
			return true;
		else
			return false;
	}
};

class LR1
{
	public:
		map<string, int> Action[state_maxn], Goto[state_maxn];
		vector<set<Node> > I;
		vector<Node> init;
		//vn 语法变量 vt 终结符 vnt V&T
		set<string> vn, vt, vnt;

		void Init()
		{
			vector<string> right_tmp;
			right_tmp.push_back("S");
			set<string> search_tmp;
			search_tmp.insert("#");
			init.push_back(Node(string("S'"), right_tmp, search_tmp, 0));
			ifstream file_in("syntax.txt");
			string buffer;
			while (getline(file_in, buffer))
			{
				string left, tmp;
				vector<string> right;
				stringstream stringcin(buffer);
				stringcin >> left >> tmp;
				vn.insert(left), vnt.insert(left);
				while (stringcin >> tmp)
				{
					right.push_back(tmp);
					if (tmp == "@")//@ is e
						continue;
					vnt.insert(tmp);
					if (tmp[0] >= 'A' && tmp[0] <= 'Z')
						vn.insert(tmp);
					else
						vt.insert(tmp);
				}
				init.push_back(Node(left, right, search_tmp, 0));
			}
			vt.insert("#"), vnt.insert("#");
			file_in.close();
		}

		void printInit()
		{
			for (vector<Node>::iterator iter = init.begin(); iter != init.end(); iter++)
			{
				cout<<iter->left<<" -> ";
				for (int i = 0; i < iter->right.size(); i++)
				{
					if (i == iter->position)
						cout<<".";
					cout<<(iter->right)[i]<<' ';
				}
				cout<<",";
				for (set<string>::iterator it = iter->search.begin(); it != iter->search.end(); it++)
					cout<<*it<<' ';
				cout<<endl;
			}
		}


		set<string> dfs(vector<string> &afterset)
		{
			set<string> firstset;
			for (int i = 0; i < afterset.size(); i++)
			{
				string after = afterset[i];
				if (vt.find(after) != vt.end() || after == "@")
				{
					firstset.insert(after);
					return firstset;
				}
				bool flag = true;
				for (int j = 0; j < init.size(); j++)
				{
					if (init[j].left == after)
					{
						if (init[j].right[0] == after)
							continue;
						set<string> first = dfs(init[j].right);
						if (first.find("@") == first.end())
						{
							flag = false;
							firstset.insert(first.begin(), first.end());
						}
						else
						{
							flag = true;
							if ((i + 1) == afterset.size())
								firstset.insert(first.begin(), first.end());
							else
							{
								for (set<string>::iterator iter = first.begin(); iter != first.end(); iter++)
								{
									if (*iter != "@")
										firstset.insert(*iter);
								}
							}
						}

					}
				}
				if (flag == false)
					break;
			}
			return firstset;
		}


		set<Node> Derive(Node &pre)
		{
			set<Node> derivation;
			set<string> first;
			derivation.insert(pre);
			string next = pre.right[pre.position];
			if (vn.find(next) == vn.end() || next == "@")
				return derivation;
			if ((pre.position + 1) < pre.right.size())
			{
				vector<string> after;
				for (int i = pre.position + 1; i < pre.right.size(); i++)
					after.push_back(pre.right[i]);
				first = dfs(after);
				if (first.count("@") > 0)
				{
					first.insert(pre.search.begin(), pre.search.end());
					first.erase("@");
				}
			}
			else
			{
				first.insert(pre.search.begin(), pre.search.end());
				for (set<string>::iterator iter = pre.search.begin(); iter != pre.search.end(); iter++)
					first.insert(*iter);
			}
			for (int i = 0; i < init.size(); i++)
			{
				if (init[i].left == next)
					derivation.insert(Node(next, init[i].right, first, 0));

			}
			return derivation;
		}

		void getProjectset()
		{
            ofstream out("table.txt");
			Node accept = init[0];
			accept.position++;
			set<Node> I0 = Derive(init[0]);
			while (true)
			{
				int len = I0.size();
				for (set<Node>::iterator iter = I0.begin(); iter != I0.end(); iter++)
				{
					if (iter->position < iter->right.size())
					{
						Node tmp = *iter;
						set<Node> derivation = Derive(tmp);
						I0.insert(derivation.begin(), derivation.end());
					}
				}
				if (len == I0.size())
					break;
			}

/* PRINTI0*/
			out<<"I0:\n";
			for (set<Node>::iterator iter = I0.begin(); iter != I0.end(); iter++)
			{
				out<<iter->left<<"->";
				for (int i = 0; i < iter->right.size(); i++)
				{
					if (iter->position == i)
						out<<". ";
					out<<iter->right[i]<<" ";
				}

				if (iter->position == iter->right.size())
					out<<".";
				out<<",";
				for (set<string>::iterator it = iter->search.begin(); it != iter->search.end(); it++)
					out<<'/'<<*it;
				out<<endl;
			}
			out<<endl;
//PRINTI0
/*INIT I1 - In*/
			for (set<Node>::iterator iter = I0.begin(); iter != I0.end(); iter++)
			{
				if (iter->right[0] == "@")
				{
					Node tmp = *iter;
					int id = 0;
					for (; id < init.size(); id++)
						if(init[id].left == tmp.left && init[id].right == tmp.right)
							break;
					set<string> search = iter->search;
					for (set<string>::iterator it = search.begin(); it != search.end(); it++)
						Action[0][*it] = id+guiyue;
				}
			}
			I.push_back(I0);
//INIT I1 - In
			for (int i = 0; i < I.size(); i++)
			{
				set<Node> iset = I[i];
				//vnt为待填字母集合 *iter
				for (set<string>::iterator iter = vnt.begin(); iter != vnt.end(); iter++)
				{
					set<Node> niset;//CLOSURE()
/* createNewCLOSURE*/
					for (set<Node>:: iterator it = iset.begin(); it != iset.end(); it++)
					{
						if (it->position < it->right.size() && (it->right[it->position]) == (*iter))
							niset.insert(Node(it->left,it->right, it->search, it->position+1));
					}

					while (true)
					{
						int len = niset.size();
						for (set<Node>:: iterator it = niset.begin(); it != niset.end(); it++)
						{
							if (it->position < it->right.size())
							{
								Node tmp = (*it);
								set<Node> derivation = Derive(tmp);
								niset.insert(derivation.begin(), derivation.end());
							}
						}
						if (len == niset.size())
							break;
					}
					if (niset.empty())
						continue;
//createNewCLOSURE
/* isNEW*/
					bool isnew = true;
					for (int j = 0; j < I.size(); j++)
					{
						if (I[j] == niset)
						{
							isnew = false;
							if (vt.find(*iter) != vt.end())
								Action[i][*iter] = j;
							else
								Goto[i][*iter] = j;
							break;
						}
					}
//isNEW
/* PRINT*/
					if (isnew)
					{
						out<<"I"<<I.size()<<':'<<endl;
						for (set<Node>::iterator ite = niset.begin(); ite != niset.end(); ite++)
						{
							out<<ite->left<<"->";
							for (int i = 0; i < ite->right.size(); i++)
							{
								if (ite->position == i)
									out<<". ";
								out<<ite->right[i]<<" ";
							}
							if (ite->position == ite->right.size())
								out<<'.';
							out<<",";
							for(set<string>::iterator t = ite->search.begin(); t != ite->search.end(); t++)
								out<<'/'<<*t;
							out<<endl;
						}
						out<<endl;
					}
//PRINT
/* ACTION AND GOTO*/
					if (isnew)
					{
						I.push_back(niset);
						if (vt.find(*iter) != vt.end())
							Action[i][*iter] = I.size()-1;
						else
							Goto[i][*iter] = I.size()-1;
						if (niset.find(accept) != niset.end())
							Action[I.size()-1]["#"] = acc;
						else
						{
							for (set<Node>::iterator it = niset.begin(); it != niset.end(); it++)
							{
								if (it->position == it->right.size() || it->right[0] == "@")
								{
									Node tmp = *it;
									int id = 0;
									for (; id < init.size(); id++)
										if(init[id].left == tmp.left && init[id].right == tmp.right)
											break;
									for (set<string>::iterator t = it->search.begin(); t != it->search.end(); t++)
											Action[I.size()-1][*t] = id+guiyue;
								}
							}
						}
					}
//ACTION AND GOTO
				}
			}
			out.close();
		}

		LR1()
		{
			Init();
			getProjectset();
		}
        void printStack(stack<string> s ,ofstream &out){
            for (stack<string>::iterator i = --s.end() ; i != s.begin(); i--){
                out<<*i<<' ';
            }
            out<<*(s.begin())<<endl;
        }
        void printVector(vector<Lex> s,ofstream &out){
            for (vector<Lex>::iterator i = --s.end(); i!= s.begin(); i--){
                out<<(*i).word<<' ';
            }
            out<<(*s.begin()).word<<endl;
        }
		void judge()
		{
			ofstream out("process.txt");
			stack<int> status;
			stack<string> symbol;
			status.push(0);
			int step = 0;
			while (lex.size() > 0)
			{
			    out<<"-------------"<<step++<<"----------------"<<endl;
			    printVector(lex,out);
				Lex frolex = lex.back();
				out<<frolex.word<<" :";
				int topsta = status.top();
				if (Action[topsta][frolex.word] == acc)
				{
					out<<"Yeah!\n";
					return;
				}
				if (Action[topsta][frolex.word] != 0 || Action[topsta][frolex.type] != 0)
				{
					int number = max(Action[topsta][frolex.word], Action[topsta][frolex.type]);
					out<<number<<"\t\t";
					//reduce
					if (number >= guiyue)
					{
					    out<<"\nreduce: ";
						int r = number - guiyue;
						Node pre = init[r];
						out<<pre.left<<"->";
						if (pre.right[0] != "@")
						{
							for (int i = 0; i < pre.right.size(); i++){
								symbol.pop(); status.pop();
                                out<<pre.right[i]<<' ';
							}
						}
						out<<endl;

						symbol.push(pre.left);
						if (Goto[status.top()][pre.left] == 0)
						{
							out<<"NO!\n";
							return;
						}
						else
							status.push(Goto[status.top()][pre.left]);
					}
					//move in
					else
					{
					    out<<"\nmove in: ";
						lex.pop_back();
						symbol.push(frolex.word);
						status.push(number);
					}
				}
				else
				{
					out<<"NO!\n";
					out<<"status: "<<topsta<<" "<<frolex.word<<" "<<frolex.type<<endl;
					return;
				}
                printStack(symbol,out);
                out<<"-------------end----------------"<<endl<<endl;
			}
			out.close();
		}

};



int main()
{
	LR1 lr1;
	freopen("Lex.cpp", "r", stdin);
	char word[100], type[100];
	while (scanf("%s%s", type, word) != EOF)
		lex.push_back(Lex(word, type));
	lex.push_back(Lex("#", "halt"));
	for (int i = 0, j = lex.size()-1; i < j; i++, j--)
		swap(lex[i], lex[j]);
	fclose(stdin);

	//for (int i = 0; i < lex.size(); i++)
    // 	cout<<lex[i].type<<' '<<lex[i].word<<endl;
    //lr1.printInit();
	lr1.judge();

	return 0;
}

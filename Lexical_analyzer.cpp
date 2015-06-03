/***********************************************
*FileName:   Lexical_analyzer.cpp
*Author:     aaronzark/吴震
*Path:       /C/Users/吴震/Desktop/编译原理课设/My/compiler/Lexical_analyzer.cpp 
***********************************************/
#include <fstream>
#include <iostream>
#include <cstring>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <map>
using namespace std;

const int buffer_maxn = 1000;
const int char_maxn = 100;

map<string, bool> _operator;
map<string, bool> keyword;
map<string, bool> delimiter;

void initKeyword()
{
	keyword["auto"] = true;
	keyword["break"] = true;
	keyword["case"] = true;
	keyword["char"] = true;
	keyword["const"] = true;
	keyword["continue"] = true;
	keyword["default"] =true;
	keyword["do"] = true;
	keyword["double"] = true;
	keyword["else"] = true;
	keyword["enum"] = true;
	keyword["extern"] = true;
	keyword["float"] = true;
	keyword["for"] = true;
	keyword["goto"] = true;
	keyword["if"] = true;
	keyword["int"] = true;
	keyword["long"] = true;
	keyword["register"] = true;
	keyword["return"] = true;
	keyword["short"] = true;
	keyword["signed"] = true;
	keyword["sizeof"] = true;
	keyword["static"] = true;
	keyword["struct"] = true;
	keyword["switch"] = true;
	keyword["typedef"] = true;
	keyword["union"] = true;
	keyword["unsigned"] = true;
	keyword["void"] = true;
	keyword["volatile"] = true;
	keyword["while"] = true;
}

void initOperator()
{
	_operator["!"] = true;
	_operator["&"] = true;
	_operator["~"] = true;
	_operator["^"] = true;
	_operator["*"] = true;
	_operator["/"] = true;
	_operator["%"] = true;
	_operator["+"] = true;
	_operator["-"] = true;
	_operator["<"] = true;
	_operator[">"] = true;
	_operator["="] = true;
	_operator["++"] = true;
	_operator["--"] = true;
	_operator["<="] = true;
	_operator[">="] = true;
	_operator["=="] = true;
	_operator["!="] = true;
	_operator["&&"] = true;
	_operator["||"] = true;
	_operator["+="] = true;
	_operator["-="] = true;
	_operator["*="] = true;
	_operator["/="] = true;
	_operator["^="] = true;
	_operator["&="] = true;
	_operator["~="] = true;
	_operator["%="] = true;
	_operator["|"] = true;
}

void initDelimiter()
{
	delimiter["{"] = true;
	delimiter["}"] = true;
	delimiter["("] = true;
	delimiter[")"] = true;
	delimiter["["] = true;
	delimiter["]"] = true;
	delimiter[""] = true;
	delimiter["'"] = true;
	delimiter[";"] = true;
	delimiter[","] = true;
}


class NFA
{
	public:
		int char_num, ter_num, notter_num;
		map<char,int> noterchar_id;
		map<int,char> id_noterchar;
		map<char,int> terchar_id;
		map<int,char> id_terchar;
		vector<int> edge[char_maxn][char_maxn];

		void nfa_Init(int choice)
		{
			char_num = ter_num = notter_num = 0;
			char buffer[buffer_maxn];
			ifstream file_in;
			switch(choice)
			{
				case 1:	file_in.open("const.txt");	break;
				case 2:	file_in.open("delimiter.txt");	break;
				case 3:	file_in.open("identifier.txt");	break;
				case 4:	file_in.open("keyword.txt");	break;
				case 5:	file_in.open("operator.txt");	break;
			}
	
			if (!file_in.is_open())
			{
				cout<<"打开文件错误\n";
				return;
			}

			terchar_id['@'] = char_num;
			id_terchar[char_num++] = '@';
			noterchar_id['$'] = char_num;
			id_noterchar[char_num++] = '$';

			while (!file_in.eof())
			{
				file_in.getline(buffer, buffer_maxn-1);
		//		cout<<buffer<<endl;
				int index = 0, len = strlen(buffer);
				if (buffer[0] == '\n')
					continue;
				while (!isUpper_letter(buffer[index]))
					index++;
				char left = buffer[index];

				if (noterchar_id.find(left) == noterchar_id.end())
				{
			//		cout<<left<<endl;
					noterchar_id[left] = char_num;
					id_noterchar[char_num++] = left;
				}
				for (index++; index < len;)
				{
					if (buffer[index] == ' ' || buffer[index] == '|' || 
						buffer[index] == '\n' || buffer[index] == '\t')
					{
						index++;
						continue;
					}
					if (index+1 < len && buffer[index] == '-' && buffer[index+1] == '>')
					{
						index += 2;
						continue;
					}
					if (!isUpper_letter(buffer[index]))
					{
						if (terchar_id.find(buffer[index]) == terchar_id.end())
						{
					//		cout<<buffer[index]<<endl;
							terchar_id[buffer[index]] = char_num;
							id_terchar[char_num++] = buffer[index];
						}
						if (index+1 < len && isUpper_letter(buffer[index+1]))
						{
							if (noterchar_id.find(buffer[index+1]) == noterchar_id.end())
							{
					//			cout<<buffer[index+1]<<endl;
								noterchar_id[buffer[index+1]] = char_num;
								id_noterchar[char_num++] = buffer[index+1];
							}
					//		cout<<left<<"->"<<buffer[index]<<buffer[index+1]<<endl;
							edge[noterchar_id[left]][terchar_id[buffer[index]]].push_back(noterchar_id[buffer[index+1]]);
							index += 2;
							continue;
						}
						else
						{
							edge[noterchar_id[left]][terchar_id[buffer[index]]].push_back(noterchar_id['$']);
					//		cout<<left<<"->"<<buffer[index]<<endl;
							index++;
							continue;
						}
					}
					else
					{
						if (noterchar_id.find(buffer[index]) == noterchar_id.end())
						{
					//		cout<<buffer[index]<<endl;
							noterchar_id[buffer[index]] = char_num;
							id_noterchar[char_num++] = buffer[index];
						}
					//	cout<<left<<"->"<<buffer[index]<<endl;
						edge[noterchar_id[left]][terchar_id['@']].push_back(noterchar_id[buffer[index]]);
						index++;
						continue;
					}
				}
			}
			file_in.close();
		}

		bool isUpper_letter(char ch)
		{
			if (ch >= 'A' && ch <= 'Z')	return true;
			else 	return false;
		}

		void printNFA()
		{
			for (int i = 0; i < char_num; i++)
				for (int j = 0; j < char_num; j++)
					for (int k = 0; k < edge[i][j].size(); k++)
						cout<<id_noterchar[i]<<"->"<<id_terchar[j]<<id_noterchar[edge[i][j][k]]<<endl;
						
		}

};


class DFA
{
	public:
		
		vector<int> dfa_state[char_maxn*2];
		int edge[char_maxn*2][char_maxn*2];
		bool is_terstate[char_maxn*2];
		bool is_ter_tmp;
		bool vis[char_maxn*2];
		int state_num;
		int tmp[char_maxn*2], num;

		DFA(NFA &nfa)
		{
			memset(is_terstate, false, sizeof(is_terstate));
			memset(edge, -1, sizeof(edge));
			state_num = 0;
			int front, rear;
			front = rear = num = 0;
			is_ter_tmp = false;
			tmp[num++] = nfa.noterchar_id['S'];
			
			getClosure(nfa.noterchar_id['S'], nfa);
			sort(tmp, tmp+num);
			
			for (int i = 0; i < num; i++)
				dfa_state[rear].push_back(tmp[i]);
			if (is_ter_tmp)		is_terstate[rear] = true;
			

			// cout<<"dfa:";
			// for (int i = 0; i < dfa_state[rear].size(); i++)
			// 	cout<<nfa.id_noterchar[dfa_state[rear][i]];
			// cout<<endl;

			rear++;
		
			map<char, int>::iterator iter1;
			while (front != rear)
			{
				for (iter1 = nfa.terchar_id.begin(); iter1 != nfa.terchar_id.end(); iter1++)
				{
					if (iter1->first == '@')
						continue;

					is_ter_tmp = false;
					num = 0;

			//		cout<<"iter:"<<nfa.id_terchar[iter1->second]<<endl;
					
					for (int i = 0; i < dfa_state[front].size(); i++)
					{
						int src = dfa_state[front][i];
						for (int j = 0; j < nfa.edge[src][iter1->second].size(); j++)
						{
							int to = nfa.edge[src][iter1->second][j];
							bool isfind = false;
							for (int k = 0; k < num; k++)
							{
								if (tmp[k] == to)
								{
									isfind = true;
									break;
								}
							}
							if (!isfind)	tmp[num++] = to;
							getClosure(to, nfa);						
						}
					}

					if (num == 0)	continue;
					sort(tmp, tmp+num);

					// cout<<"tmp:";
					// for (int i = 0; i < num; i++)
					// 	cout<<nfa.id_noterchar[tmp[i]];
					// cout<<endl;

					
					bool isnew = true;
					int state_x, state_y;
					
					for (state_x = 0; state_x < rear; state_x++)
					{
						if (dfa_state[state_x].size() == num)
						{
							for (state_y = 0; state_y < num; state_y++)
							{
								if (tmp[state_y] == dfa_state[state_x][state_y])	continue;
								else	break;
							}
							if (state_y >= num)
							{
								isnew = false;
								break;
							}
						}
					}

					if (isnew)
					{
					//	cout<<"dfa_state:";
						for (int i = 0; i < num; i++)
						{
						//	cout<<nfa.id_noterchar[tmp[i]];
							dfa_state[rear].push_back(tmp[i]);
						}
					//	cout<<endl;

						edge[front][iter1->second] = rear;
						if (is_ter_tmp)	is_terstate[rear] = true;
						rear++;
					}
					else
						edge[front][iter1->second] = state_x;

				}
				front++;
			}
			state_num = rear;

			/*
			cout<<"print:\n";
			for (int i = 0; i < state_num; i++)
			{
				for (iter1 = nfa.terchar_id.begin(); iter1 != nfa.terchar_id.end(); iter1++)
				{
					if (iter1->first == '@')
						continue;
					cout<<i<<"->"<<iter1->first<<edge[i][iter1->second]<<endl;
				}
			}
			*/
		}

		
		void getClosure(int state, NFA &nfa)
		{
			if (state == 1)
				is_ter_tmp = true;
			bool flag = false;
			for (int i = 0; i < num; i++)
			{
				if (tmp[i] == state)
				{
					flag = true;
					break;
				}
			}
			if (!flag)	tmp[num++] = state;
			for(int i = 0; i < nfa.edge[state][0].size(); i++)
				getClosure(nfa.edge[state][0][i], nfa);	
		}

		bool judge(string str, NFA &nfa)
		{
			bool isok = true;
			int len = str.length(), p = 0;
			for (int i = 0; i < len; i++)
			{
				p = edge[p][nfa.terchar_id[str[i]]];
				if (p == -1)
				{
					isok = false;
					break;
				}
			}
			if (!isok)	return false;
			if (!is_terstate[p])	return false;
			else	return true;
		}


};

int main()
{
	initOperator();
	initKeyword();
	initDelimiter();
	NFA const_nfa;
	const_nfa.nfa_Init(1);
	DFA const_dfa(const_nfa);
	NFA delimiter_nfa;
	delimiter_nfa.nfa_Init(2);
	DFA delimiter_dfa(delimiter_nfa);
	NFA identifier_nfa;
	identifier_nfa.nfa_Init(3);
	DFA identifier_dfa(identifier_nfa);

	freopen("source.cpp", "r", stdin);
	freopen("Lex.cpp", "w", stdout);
	int linenum = 0;
	string line, word;
	vector<string> tmp;
	bool flag = false;
	while (getline(cin, line))
	{
		linenum++;
		stringstream stringcin(line);
		while (stringcin >> word)
		{
			if (keyword.find(word) != keyword.end())
			{
				cout<<"keyword\t"<<word<<endl;
				continue;
			}
			if (_operator.find(word) != _operator.end())
			{
				cout<<"operator\t"<<word<<endl;
				continue;
			}
			
			if (const_dfa.judge(word, const_nfa))
			{
				cout<<"const\t"<<word<<endl;
				continue;
			}
			if (delimiter_dfa.judge(word, delimiter_nfa))
			{
				cout<<"delimiter\t"<<word<<endl;
				continue;
			}
			if (identifier_dfa.judge(word, identifier_nfa))
			{
				cout<<"identifier\t"<<word<<endl;
				continue;
			}
			if (!flag)
				cout<<linenum<<'\t'<<word<<"\terror\n";
		}
	}
	fclose(stdin);
	fclose(stdout);
	return 0;
}

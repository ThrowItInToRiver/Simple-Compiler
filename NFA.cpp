#include <iostream>
#include <cmath>
#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <set>
#include<bitset>
using namespace std;

int Aflag = 0;   //a用于标记NFA中的状态，例如：A0、A1
//vector<vector<string>> state;

class Edge {
public:
	Edge() {
		this->begin = "";
		this->thro = ' ';
		this->end = "";
	};

	Edge(string begin, char thro, string end) {
		this->begin = begin;
		this->thro = thro;
		this->end = end;
	}

	void setThro(char ch) {
		this->thro = ch;
	}

	char getThro() {
		return this->thro;
	}

	void setBegin(string ch) {
		this->begin = ch;
	}

	string getBegin() {
		return this->begin;
	}

	void setEnd(string ch) {
		this->end = ch;
	}

	string getEnd() {
		return this->end;
	}


private:
	char thro;
	string begin;
	string end;
};

set<int> e_closure(int start_v, vector<vector<string>> state)
{
    set<int> result;

    stack<int> closure;
    closure.push(start_v);

    while(!closure.empty())
    {
        int current_v = closure.top();
        closure.pop();
        result.insert(current_v);

        //cout << current_v << "-----\n";
        for(char c: state[current_v][2])
        {
            //cout << c-'0' << "-----\n";
            set<int>::iterator it = result.find(c-'0'); //防止重复闭包
            if(it==result.end())
                closure.push(c-'0');
        }  
    }

    /*for (const int& element : result) 
        cout << element << " ";*/
    
    return result;
}

int set_to_state(set<int> x)
{
    int binary_num = 0;
    for (int num : x) {
        // 使用位运算将对应位设置为1
        binary_num |= (1 << num);
    }

    // 使用 std::bitset 输出二进制形式
    bitset<sizeof(int) * 8> binaryRepresentation(binary_num);
    cout << "整数 " << binary_num << " 的二进制形式是: " << binaryRepresentation << endl;

    return binary_num;
}

void traverseBinary(int num) {  
    int mask = 1 << 31; // 设置掩码，只有最高位为1  
  
    // 从最高位开始遍历  
    for (int i = 31; i >= 0; i--) {  
        // 通过按位与运算得到每一位的值  
        int bit = (num & mask) != 0;  
   
        // 移动掩码，准备处理下一位  
        mask >>= 1;  
    }  
  
    cout << std::endl;  
}  

void trans(vector<vector<string>> state)
{
    queue<int> final_state;
	queue<int> u;
    
    set<int> initial_state = e_closure(4, state);
	u.push( set_to_state(initial_state));
    final_state.push( set_to_state(initial_state));  //最初的状态，开始状态的空闭包

	while(!u.empty())
	{
		set<int> x;
		set<int> new_state;  //x里点的闭包

		int num = u.front();
		int mask = 1 << 31; //设置掩码
		for(int i=31; i>=0; i--)
		{
			int bit = (num & mask) != 0;
			if(bit==1 && state[i][0]!="")
			{
				for(char c: state[i][0])
					x.insert(c-'0');
			}
		}
		for (const int& element : x)
		{
			//cout << element << " ";
			for(const int& extra_element : e_closure(element, state))
				new_state.insert(extra_element);
		}

		for (const int& element : new_state)
			cout << element << " ";

		int state_num = set_to_state(new_state);
		final_state.push(state_num);

		x.clear();
		new_state.clear();

		num = u.front();
		mask = 1 << 31; //设置掩码
		for(int i=31; i>=0; i--) 
		{
			int bit = (num & mask) != 0;
			if(bit==1 && state[i][1]!="")
			{
				for(char c: state[i][1])
					x.insert(c-'0');
			}
		}
		for (const int& element : x)  //计算x的闭包
		{
			//cout << element << " ";
			for(const int& extra_element : e_closure(element, state))
				new_state.insert(extra_element);
		}

		for (const int& element : new_state)
			cout << element << " ";
			
		state_num = set_to_state(new_state);
		final_state.push(state_num);

		u.pop();
	}
}

class RTN {
public:
	RTN() {};

	bool isletter(char ch) {
		if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {
			return true;
		}
		else {
			return false;
		}
	}

	bool islegal(string s) {
		int Lbracket = 0;
		int Rbracket = 0;
		RTN r;
		for (int i = 0; i < s.length(); i++) {
			char ch = s[i];
			if (ch == '*' || ch == '|' || ch == '.') {
				continue;
			}
			//数字不考虑判断
			else if (ch >= 0 && ch <= 9) {
				continue;
			}
			else if (r.isletter(ch)) {
				continue;
			}
			else if (ch == '(') {
				Lbracket++;
			}
			else if (ch == ')') {
				Rbracket++;
			}
			else {
				return false;
			}
		}
		if (Lbracket != Rbracket) {
			return false;
		}
		else {
			return true;
		}
	}

	//将输入的表达式中的连接符号用'.'补全
	string turnToConnect(string s) {
		string ns = s.substr(0, 1);
		for (int i = 1; i < s.length(); i++) {
			char prech = s[i - 1];
			char ch = s[i];
			//连续的字母之间需要添加上连接符号'.'
			if (isletter(prech) && isletter(ch)) {
				ns = ns + '.' + ch;
				//cout << endl << ns;
				continue;
			}
			//判断当前是字母，并且前面是右括号')'的情况
			else if (isletter(ch) && prech == ')') {
				ns = ns + '.' + ch;
				continue;
			}
			//判断当前是左括号'('，并且前面是右括号')'或者前面是字母的情况
			else if (ch == '(' && prech == ')' || ch == '(' && isletter(prech)) {
				ns = ns + '.' + ch;
				continue;
			}
			else {
				ns += ch;
				//cout << endl << ns;
				continue;
			}
		}
		return ns;
	}

	string ReversePolishType(string s) {
		stack<char> st;
		string ns = "";

		for (int i = 0; i < s.length(); i++) {
			char ch = s[i];
			if (isletter(ch)) {
				ns = ns + ch;
			}
			else if (ch == '(') {
				st.push(ch);
			}
			else if (ch == ')') {
				while (st.top() != '(') {
					ns = ns + st.top();
					st.pop();
				}
				st.pop();//将栈顶剩余的左括号弹出
			}
			else if (ch == '*') {
				ns = ns + ch;
				//'*'是单目运算符，碰到的时候直接弹出
				/*if (st.empty() || st.top() == '.' || st.top() == '|' || st.top() == '(') {
					st.push(ch);
				}
				else {
					while (st.top() == '*') {
						ns = ns + st.top();
						st.pop();
						if (st.empty()) break;
					}
					if (st.empty() || st.top() == '.' || st.top() == '|' || st.top() == '(') {
						st.push(ch);
					}
				}*/
			}
			else if (ch == '.') {
				if (st.empty() || st.top() == '|' || st.top() == '(') {
					st.push(ch);
				}
				else {
					while (st.top() == '*' || st.top() == '.') {
						ns = ns + st.top();
						st.pop();
						if (st.empty()) break;
					}
					if (st.empty() || st.top() == '|' || st.top() == '(') {
						st.push(ch);
					}
				}
			}
			else if (ch == '|') {
				if (st.empty() || st.top() == '(') {
					st.push(ch);
				}
				else {
					while (st.top() == '*' || st.top() == '.' || st.top() == '|') {
						ns = ns + st.top();
						st.pop();
						if (st.empty()) break;
					}
					if (st.empty() || st.top() == '(') {
						st.push(ch);
					}
				}
			}
		}
		while (!st.empty()) {
			ns = ns + st.top();
			st.pop();
		}

		return ns;
	}

	//处理 a|b
	Edge Unite(Edge Left, Edge Right) {
		Edge ed1(to_string(Aflag), '#', Left.getBegin());
		Edge ed2(to_string(Aflag), '#', Right.getBegin());
		Edge ed3(Left.getEnd(), '#', to_string(Aflag + 1));
		Edge ed4(Right.getEnd(), '#', to_string(Aflag + 1));
		q.push(ed1);
		q.push(ed2);
		q.push(ed3);
		q.push(ed4);
		if (Left.getThro() != ' ') {
			this->q.push(Left);
		}
		if (Right.getThro() != ' ') {
			this->q.push(Right);
		}

		Edge ed(to_string(Aflag), ' ', to_string(Aflag + 1));
		Aflag = Aflag + 2;
		return ed;
	}

	//处理 a.b
	Edge Join(Edge Left, Edge Right) {
		Right.setBegin(Left.getEnd());
		if (Left.getThro() != ' ') {
			q.push(Left);
		}
		if (Right.getThro() != ' ') {
			q.push(Right);
		}
		
		
		Edge ed(Left.getBegin(), ' ', Right.getEnd());
		//Edge ed(Left.getBegin(), Left.getThro() + '.' + Right.getThro(), Right.getEnd());
		return ed;
	}

	//处理 a*
	Edge Self(Edge edge) {
		Edge ed1(edge.getBegin(), '#', edge.getEnd());
		Edge ed2(edge.getEnd(), '#', edge.getBegin());
		this->q.push(ed1);
		this->q.push(ed2);
		/*Edge ed1(to_string(Aflag), '#', edge.getBegin());
		Edge ed2(edge.getEnd(), '#', to_string(Aflag + 1));
		Edge ed3(edge.getEnd(), '#', edge.getBegin());
		Edge ed4(to_string(Aflag), '#', to_string(Aflag+1));
		this->q.push(ed1);
		this->q.push(ed2);
		this->q.push(ed3);
		this->q.push(ed4);*/

		if (edge.getThro()!=' ') {
			this->q.push(edge);
		}
		
		Aflag = Aflag + 2;
		Edge ed(to_string(Aflag), ' ', to_string(Aflag + 1));
		return ed;
	}

	void PolishTypeToNFA(string s) {
		stack<Edge> st;
		Aflag = 0;//a用于标记NFA中的状态，例如：A0、A1
		for (int i = 0; i < s.length(); i++) {
			char ch = s[i];

			if (this->isletter(ch)) {
				Edge ed(to_string(Aflag), ch, to_string(Aflag + 1));
				Aflag = Aflag + 2;
				st.push(ed);
			}
			else if (ch == '*') {
				Edge ed = st.top();
				st.pop();
				st.push(this->Self(ed));
			}
			else if (ch == '.') {
				Edge ed1 = st.top();
				st.pop();
				Edge ed2 = st.top();
				st.pop();
				Edge ed = Join(ed2, ed1);
				st.push(ed);
			}
			else if (ch == '|') {
				Edge ed1 = st.top();
				st.pop();
				Edge ed2 = st.top();
				st.pop();
				st.push(this->Unite(ed1, ed2));
			}
		}

        Edge e("5",'#',"8");
        q.push(e);
        vector<vector<string>> state(q.size()+1, vector<string>(3,""));
        //vector<vector<string>> state;
		while (!this->q.empty())
		{
			Edge e = q.front();
			cout << endl << "开始结点为：" << e.getBegin() << "     转换过程为：" << e.getThro() << "     结束结点为：" << e.getEnd() ;
			q.pop();

            int num = stoi(e.getBegin());
            cout << num << endl;

            if(e.getThro()=='a')
                state[num][0] += e.getEnd();
            else if(e.getThro()=='b')
                state[num][1] += e.getEnd();
            else
                state[num][2] += e.getEnd();
            
		}
        cout << state.size()<<endl;
        for(int i=0; i<state.size(); i++)
        {
            for(int j=0; j<3; j++)
                cout << state[i][j] << "    ";
            cout << endl;
        }

        //e_closure(4, state);
        trans(state);
	}

private:
	queue<Edge> q;
};

int main() {
	RTN rt;
	cout << "有效的正规式的字母表∑={a-z,A-Z}，辅助字母表∑={'*','|','(',')','.'}" << endl;
	cout << "'*'   表示闭包"<< endl <<"'|'   表示联合" << endl << "'.'   表示连接" << endl;
	while (1) {
		
		cout << "\n请输入一个正规式:";
		string s = "";
		cin >> s;
		if (rt.islegal(s)) {
			cout << "输入的正规式合法！"<<endl;
			string ss = rt.turnToConnect(s);
			cout << "补缺省略的连接符号'.'之后为：" <<ss << endl;
			cout << "对应的后缀表达式（逆波兰式）为：" <<rt.ReversePolishType(ss);
			string rs = rt.ReversePolishType(ss);
			rt.PolishTypeToNFA(rs);
		}
		else {
			cout << "输入的正规式不合法！"<<endl<<"请重新输入：";
		}

        /*for(int i=0; i<state.size(); i++)
        {
            for(int j=0; j<3; j++)
                cout << state[i][j] << " ";
            cout << endl;
        }*/
            
	}

}





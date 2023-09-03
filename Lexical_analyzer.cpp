#include<iostream>
#include<fstream>
#include<cstring>
#include<vector>
#include<string>
using namespace std;

char Scanin[300], Scanout[300];        //extern char ``` 时报错？？

//下面定义保留字表，为简化程序，使用字符指针数组保存所有保留字
//如果想增加保留字，可继续添加，并修改保留字数目 keywordSum
#define keywordSum 10
string keyword[keywordSum] = {"if", "else", "for", "while", "do", "int", "read", "write"};
//下面定义纯单分界符
char singleword[50] = "+-*(){};,:";
//下面定义双分界符的首字符
char doubleword[10] = "><=!";
//存储非法字符
vector<string> errors;

enum TokenType 
{
    type_identifier = 0,
    type_keyword = 1,
    type_number = 2,
    type_singleword = 3,
    type_doubleword = 4,
    type_comment = 5,
};

class Token
{
private:
    TokenType type;
    string lexeme;
public:
    Token();
    Token(TokenType _type, string _lexeme) {type = _type; lexeme = _lexeme;}
    
    TokenType getType() {return this->type;}

    string getLexeme() {return this->lexeme;}

    bool operator==(const Token& other) const {return this->type == other.type && this->lexeme == other.lexeme;}

    bool operator!=(const Token& other) const {return this->type != other.type && this->lexeme != other.lexeme;}
};

//存储分析出的单词
vector<Token> signs;

string get_token_string(Token t)
{
    switch (t.getType())
    {
        case 0:
            return "ID";
        case 2:
            return "NUM";
        case 1:
        case 3:
        case 4:
        case 5:
            return t.getLexeme();
        default:
            return "UNKNOWN";
    }
};

//DFA
void stateTransition(int current_state, const char* contentPtr, string s)
{
    if (*contentPtr != '\0')
    {
        //cout << current_state<<"  "<<*contentPtr<<endl;
        switch (current_state)
        {
            case 0:
                if(isalpha(*contentPtr))    //字母
                {
                    s.push_back(*contentPtr);
                    stateTransition(1, ++ contentPtr, s);
                }

                else if(isdigit(*contentPtr))    //数字
                {
                    s.push_back(*contentPtr);
                    stateTransition(2, ++ contentPtr, s);
                }

                else if(*contentPtr==' ' || *contentPtr=='\n')  //空格，跳过
                    stateTransition(0, ++ contentPtr, s);
                
                else if(*contentPtr == '/')         
                    stateTransition(6, ++ contentPtr, s);
                
                else
                {
                    int flag = 0;
                    for(char c : singleword)   //纯单分符
                    {
                        if (*contentPtr == c) 
                        {
                            s.push_back(c);
                            //cout << s << "----" << endl;
                            stateTransition(3, contentPtr, s);   //注意，不需要++
                                
                            flag = 1;              // 找到了目标字符
                            break;
                        }
                    }

                    if(!flag)
                    {
                        for(char c : doubleword)   //可能是双分符
                        {
                            if (*contentPtr == c) 
                            {
                                s.push_back(c);
                                stateTransition(4, ++ contentPtr, s);  
                                    
                                flag = 1;              // 找到了目标字符
                                break;
                            }
                        }
                    }

                    if(!flag)
                        stateTransition(8, contentPtr, s);  //非法字符  
                }
                break;                          //注意break

            case 1:
                if(isalnum(*contentPtr))      //是字母/数字
                {
                    s.push_back(*contentPtr);
                    stateTransition(1, ++ contentPtr, s);
                }
                else
                {
                    int is_keyword = 0;

                    for (int i = 0; i < keywordSum; i++)  //判断是否是保留字
                    {
                        if (keyword[i] == s)     
                        {
                            Token t = Token(type_keyword, s);
                            signs.push_back(t);
                            is_keyword = 1;
                            break; 
                        }
                    }

                    if(!is_keyword)
                    {
                        Token t = Token(type_identifier, s);
                        signs.push_back(t);
                    }
                          
                    s =  "";
                    stateTransition(0, contentPtr, s);   //回到0状态
                }
                break;

            case 2:
                if(isdigit(*contentPtr))
                {
                    s.push_back(*contentPtr);
                    stateTransition(2, ++ contentPtr, s);
                }
                else
                {
                    Token t = Token(type_number, s);
                    signs.push_back(t);
                    s =  "";
                    stateTransition(0, contentPtr, s);
                }
                break;

            case 3:
                {                                         //注意要用大括号括起来，不然报错
                    Token t = Token(type_singleword, s);
                    signs.push_back(t);
                    s =  "";
                    stateTransition(0, ++ contentPtr, s);  
                }
                break;

            case 4:
                if(*contentPtr == '=')     //是双分界符，转到状态5
                {
                    s.push_back('=');
                    stateTransition(5, contentPtr, s);
                }
                else
                {
                    Token t = Token(type_singleword, s);
                    signs.push_back(t);
                    s = "";
                    s.push_back(*contentPtr);
                    stateTransition(0, contentPtr, s);
                }
                break;
            
            case 5:
                {
                    Token t = Token(type_doubleword, s);
                    signs.push_back(t);
                    s =  "";
                    stateTransition(0, ++ contentPtr, s);
                }
                break;
            
            case 6:
                if(*contentPtr != '*')
                {
                    s.push_back(*contentPtr);
                    Token t = Token(type_singleword, s);
                    signs.push_back(t);
                    s =  "";
                    stateTransition(0, contentPtr, s);
                }
                else              //是注释，忽视注释中的语句
                {
                    contentPtr ++;
                    const char * next_contentPtr = contentPtr + 1;
                 
                    while( !(*contentPtr=='*' && *next_contentPtr=='/') )
                    {
                        contentPtr ++;
                        next_contentPtr ++;
                    }

                    contentPtr += 2;
                    stateTransition(0, contentPtr, s);
                }
                break;
            
            case 8:               //不明字符，报错并跳过此字符
                {
                    string error_char;             //记录错误信息
                    error_char += "Error char: ";
                    error_char.push_back(*contentPtr);
                    errors.push_back(error_char);

                    stateTransition(0, ++ contentPtr, s);
                }
                break;
            }
        
    }
      
}

int TESTscan()
{
    cout << "请输入读取文件的地址: ";
    cin >> Scanin;              // 读取文件地址并存储在Scanin中
   
    ifstream inputFile(Scanin); 
    //ifstream inputFile("input.txt");
    if (!inputFile.is_open())
    {
        cerr << "无法打开输入文件：" << Scanin << endl;
        return 1;
    }
    
    // 打印文件内容
    //cout << "文件内容如下：" << endl;
    //cout << fileContent << endl;

    string fileContent;  // 用于存储文件内容的字符串
    string line;         // 用于读取文件中的每一行

    // 逐行读取文件内容并存储到fileContent中
    while (getline(inputFile, line)) 
        fileContent += line + '\n';           // 读取时保留换行符

    inputFile.close();


    const char* contentPtr = fileContent.c_str();   // 获取字符串的指针
    string t = "";

    stateTransition(0, contentPtr, t);        //开始进行状态机转换


    cout << "请输入输出文件的地址: ";
    cin >> Scanout;             
   
    ofstream outputFile(Scanout); 
    if (!outputFile.is_open())
    {
        cerr << "无法打开输出文件：" << Scanout << endl;
        return 2;
    }

    for (Token t : signs)                //写入信息
        outputFile << get_token_string(t) << " " <<t.getLexeme() << endl;
    
    if(errors.size()>0)           //有错误信息
    {
        for(string s : errors)
            outputFile << s << endl;

        outputFile.close();
        return 3;
    }

    outputFile.close();
    return 0;
}

int main()
{
    int es = 0;
    es = TESTscan();

    if(es > 0) 
        printf("词法分析有误，编译停止！");
    else 
        printf("词法分析成功！\n");

    /*for (Token t : signs) 
        cout << get_token_string(t) << " " <<t.getLexeme() << endl;*/
   
    return 0;
}
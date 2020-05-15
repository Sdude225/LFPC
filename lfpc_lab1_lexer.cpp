#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>
#include <map>
using namespace std;

enum class Token
{
    TOK_EOF = -1,
    TOK_NUMBERLITERAL = -2,
    TOK_STRINGLITERAL = -3,
    TOK_IDENTIFIER = -4,  
    TOK_KEYWORD = -5,
    TOK_OPERATOR = -6,
    TOK_DELIMITERS = -7

};

struct Tokens
{
    string TOKEN_NAME;
    int TOKEN_ID;
};


vector<string> keywords = {"int", "while", "for", "if", "else", "void", "string", "return", "float", "function", "fbegin", "fend"};

vector<Tokens> tokenizedSampleCode;


static Tokens curTok;
int z = -1;
static Tokens getNextToken()
{
    z++;
    return curTok = tokenizedSampleCode[z];
}

class ExprAST
{
    public:
    virtual ~ExprAST() {}
};

class varDeclaration : public ExprAST
{
    public:
        string type;
        string name;
        string value;

        varDeclaration(string x, string z)
        {
            type = x;
            name = z;
            cout << "New variable declared" << endl;
        }

        string getVarName()
        {
            return name;
        }

        string getVarType()
        {
            return type;
        }

};


vector<varDeclaration> declaredVariables;

class functionDeclaration : public ExprAST
{
    public:
        string name;
        vector<pair <string, string> > Args;
        
        functionDeclaration(string x, vector<pair <string, string> > z)
        {
            name = x;
            Args = z;
        }

        string getFuncName()
        {
            return name;
        }

        vector<pair<string, string>> getFuncArguments()
        {
            return Args;
        }

};

vector<functionDeclaration> declaredFunctions;

class ExpNode : public ExprAST
{
    public:

        virtual double value() = 0;
};

class ConstNode : public ExpNode
{
    double number;

    public:
        ConstNode(double val)
        {
            number = val;
        }

        double value()
        {
            return number;
        }
};

class BinOpNode : public ExpNode
{
    char op;
    ExpNode *left;
    ExpNode *right;

    public:

        BinOpNode(char oper, ExpNode *left, ExpNode *right)
        {
            this->op = oper;
            this->left = left;
            this->right = right;
        }

        double value()
        {
            double leftVal = left->value();
            double rightVal = right->value();
            switch (op)
            {
                case '+': return leftVal + rightVal;
                case '-': return leftVal - rightVal;
                case '*': return leftVal * rightVal;
                case '/': return leftVal / rightVal;
                default :
                    cout << "Unexpected token" << endl;
                    std::exit(1); 
                    return 1;
            }
        }
};

bool isOperator(char tmp)
{
    switch (tmp)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '=':
    case '!':
    case '?':
    case '>':
    case '<':
        return true;
    default:
        return false;
    }
}

void varDec()
{
    string type = curTok.TOKEN_NAME;
    string varName;

    getNextToken();

    if(curTok.TOKEN_ID == (int)Token::TOK_IDENTIFIER)
        varName = curTok.TOKEN_NAME;
    else 
    {
        cout << "Identifier expected at variable declaration" << endl;
        std::exit(1);
    }
    
    getNextToken();

    if(curTok.TOKEN_NAME.compare(";") == 0)
    {
        varDeclaration v1(type, varName);
        declaredVariables.push_back(v1);
    }
    else
    {
        cout << "';' expected at variable declaration" << endl;
        std::exit(1);
    }

    getNextToken();
}

void funcDec()
{
    string funcName;
    string argType;
    vector<pair <string, string> > arguments;

    getNextToken();
    if(curTok.TOKEN_ID != (int)Token::TOK_IDENTIFIER)
    {
        cout << "Invalid name for function" << endl;
        std::exit(1);
    }

    funcName = curTok.TOKEN_NAME;

    getNextToken();

    if(curTok.TOKEN_NAME.compare("(") != 0)
    {
        cout << "'(' expected at " << funcName << " function declaration" << endl;
        std::exit(1);
    }
    else getNextToken();

    while(curTok.TOKEN_NAME.compare(")") != 0)
    {

        if(curTok.TOKEN_NAME.compare("int") == 0 || curTok.TOKEN_NAME.compare("float") == 0 || curTok.TOKEN_NAME.compare("string") == 0)
        {
            argType = curTok.TOKEN_NAME;
            getNextToken();

            if(curTok.TOKEN_ID == (int)Token::TOK_IDENTIFIER)
            {
                arguments.push_back(make_pair(argType, curTok.TOKEN_NAME));
                getNextToken();

                if(curTok.TOKEN_NAME.compare(",") == 0)
                {
                    getNextToken();
                    if(curTok.TOKEN_NAME.compare(")") == 0)
                    {
                        cout << "New argument expected at " << funcName << " function declaration" << endl;
                        std::exit(1);
                    }
                    else continue;
                }
                else if(curTok.TOKEN_NAME.compare(")") == 0)
                {   
                    getNextToken();
                    break;
                }
                else
                {
                    cout << "Unknown token at " << funcName << " declaration" << endl;
                    std::exit(1);
                }
            }
            else
            {
                cout << "Identifier expected at " << funcName << " declaration" << endl;
                std::exit(1);
            }
        }
        else
        {
            cout << "Type for identifier expected for argument at " << funcName << " function declaration" << endl;
            std::exit(1);
        }

    }

    functionDeclaration fd(funcName, arguments);
    declaredFunctions.push_back(fd);

}

void binOp()
{
    string varName;
    bool flag = false;
    char oper;
    int j;

    varName = curTok.TOKEN_NAME;

    for(int i = 0; i < declaredVariables.size(); ++i)
    {
        if(varName.compare(declaredVariables[i].name) == 0)
        {
            j = i;
            flag = true;
            break;
        }
    }

    if(!flag)
    {
        cout << "Variable " << varName << " not declared" << endl;
        std::exit(1);
    }

    getNextToken();

    if(curTok.TOKEN_NAME.compare("=") != 0)
    {
        cout << "= expected" << endl;
        std::exit(1);
    }

    getNextToken();

    int operations = 0, operands = 0;
    vector<string> binexpr;

    while(curTok.TOKEN_NAME.compare(";") != 0)
    {
        if(curTok.TOKEN_ID == (int)Token::TOK_NUMBERLITERAL)
        {
            binexpr.push_back(curTok.TOKEN_NAME);
            operands++;
        }
        else if(curTok.TOKEN_ID == (int)Token::TOK_OPERATOR)
        {
            binexpr.push_back(curTok.TOKEN_NAME);
            operations++;
        }
        getNextToken();
    }
    
    if((operands - operations) != 1)
    {
        cout << "Invalid expression format" << endl;
        std::exit(1);
    }

    double result = 0;
    double mulresult = 0;
    double divresult = 0;

    while(binexpr.size() != 1)
    {
        auto itmult = find(binexpr.begin(), binexpr.end(), "*");
        auto itdiv = find(binexpr.begin(), binexpr.end(), "/");

        if(itmult != binexpr.end())
        {
            int index = distance(binexpr.begin(), itmult);
            ConstNode node1(stod(binexpr[index - 1]));
            ConstNode node2(stod(binexpr[index + 1]));

            BinOpNode binnode('*', &node1, &node2);

            mulresult = binnode.value();

            binexpr.erase(binexpr.begin() + index - 1, binexpr.begin() + index + 2);
            binexpr.insert(binexpr.begin() + index - 1, to_string(mulresult));

            continue;
        }
        else if(itdiv != binexpr.end())
        {
            int index = distance(binexpr.begin(), itdiv);
            ConstNode node1(stod(binexpr[index - 1]));
            ConstNode node2(stod(binexpr[index + 1]));

            BinOpNode binnode('/', &node1, &node2);

            divresult = binnode.value();

            binexpr.erase(binexpr.begin() + index - 1, binexpr.begin() + index + 2);
            binexpr.insert(binexpr.begin() + index - 1, to_string(divresult));

            continue;
        }
        else
        {
            ConstNode node1(stod(binexpr[0]));
            ConstNode node2(stod(binexpr[2]));

            BinOpNode binnode(binexpr[1][0], &node1, &node2);
            result = binnode.value();

            binexpr.erase(binexpr.begin(), binexpr.begin() + 3);
            binexpr.insert(binexpr.begin(), to_string(result));

            continue;
        }
    }

    declaredVariables[j].value = to_string(result);

    if(curTok.TOKEN_NAME.compare(";") != 0)
    {
        cout << "';' mising at the end of " << varName << " expression" << endl;
        std::exit(1);
    }
    getNextToken();
}

void functioncall()
{
    string varName;
    string funcName;
    bool flag = false;
    int j;

    varName = curTok.TOKEN_NAME;
    
    for(int i = 0; i < declaredVariables.size(); ++i)
    {
        if(varName == declaredVariables[i].name)
        {
            flag = true;
        }
    }

    if(!flag)
    {
        cout << "Variable " << varName << " not declared" << endl;
        std::exit(1);
    }

    getNextToken();

    if(curTok.TOKEN_NAME.compare("=") != 0)
    {
        cout << "= expected" << endl;
        std::exit(1);
    }

    getNextToken();

    funcName = curTok.TOKEN_NAME;
    flag = false;

    for(int i = 0; i < declaredFunctions.size(); ++i)
    {
        if(funcName == declaredFunctions[i].name)
        {
            j = i;
            flag = true;
        }
    }

    if(!flag)
    {
        cout << "Function " << funcName << " not declared" << endl;
        std::exit(1);
    }

    getNextToken();

    if(curTok.TOKEN_NAME.compare("(") != 0)
    {
        cout << "( expected" << endl;
        std::exit(1);
    }

    getNextToken();


    int arg = 0;
    int requiredarguments = declaredFunctions[j].Args.size();
    string varType;
    vector<string> callArguments;

    while(curTok.TOKEN_NAME.compare(")") != 0)
    {
        flag = false;
        if(curTok.TOKEN_ID == (int)Token::TOK_IDENTIFIER)
        {
            for(varDeclaration decVar : declaredVariables)  
            {
                if(curTok.TOKEN_NAME.compare(decVar.name) == 0)
                {
                    flag = true;
                    varType = decVar.type;
                    break;
                }
            }    

            if(!flag)
            {
                cout << "Variable " << curTok.TOKEN_NAME << " not declared";
                std::exit(1);
            }

            if(arg == requiredarguments)
            {
                cout << "Too many arguments at " << funcName << " declarations";
                std::exit(1);
            }

            if(varType.compare(declaredFunctions[j].Args[arg].first) == 0)
            {
                callArguments.push_back(curTok.TOKEN_NAME);
                arg++;
                getNextToken();
                if(curTok.TOKEN_NAME.compare(",") == 0)
                {
                    getNextToken();
                    if(curTok.TOKEN_NAME.compare(")") == 0)
                    {
                        cout << "Argument expected" << endl;
                        std::exit(1);
                    }
                    continue;
                }
            }
            else
            {
                cout << "Data types of function arguments don't match" << endl;
                std::exit(1);
            }
        }
        else 
        {
            cout << "Argument expected" << endl;
            std::exit(1);
        }
    }

    if(arg < requiredarguments)
    {
        cout << "Too few arguments at " << funcName << " call" << endl;
        std::exit(1);
    }

    getNextToken();
    if(curTok.TOKEN_NAME.compare(";") != 0)
    {
        cout << "; expected" << endl;
        std::exit(1);
    }

    getNextToken();

}

void valAssign()
{
    string varName;
    string value;
    string type;
    int index;
    bool flag = false;
    
    varName = curTok.TOKEN_NAME;

    for(int i = 0; i < declaredVariables.size(); ++i)
    {
        if(varName.compare(declaredVariables[i].name) == 0)
        {
            flag = true;
            index = i;
            type = declaredVariables[i].type;
            break;
        }
    }

    if(!flag)
    {
        cout << "Variable " << varName << " not declared" << endl;
        std::exit(1);
    }

    getNextToken();

    if(curTok.TOKEN_NAME.compare("=") != 0)
    {
        cout << "= expected" << endl;
        std::exit(1);
    }

    getNextToken();

    switch(curTok.TOKEN_ID)
    {
    case -2:
        if(count(curTok.TOKEN_NAME.begin(), curTok.TOKEN_NAME.end(), '.') == 0 && type == "int")
        {
            value = curTok.TOKEN_NAME;
            declaredVariables[index].value = value;
            getNextToken();
        } 
        else if(count(curTok.TOKEN_NAME.begin(), curTok.TOKEN_NAME.end(), '.') == 1 && type == "float")
        {
            value = curTok.TOKEN_NAME;
            declaredVariables[index].value = value;
            getNextToken();
        } 
        else
        {
            cout << "Valid number format expected at " << varName << " equation" << endl;
            std::exit(1);
        }
        break;
    
    case -3:
        if(type == "string")
        {
            value = curTok.TOKEN_NAME;
            declaredVariables[index].value = value;
            getNextToken();
        } 
        else
        {
            cout << "String expected" << endl;
            std::exit(1);
        }
        break;
    default:
        cout << "Unexpected token at " << varName << " equation" << endl;
        std::exit(1);
        break;
    }

    getNextToken();

}

void ParserMainLoop()
{
    getNextToken();
    while(curTok.TOKEN_ID != (int)Token::TOK_EOF)
    {
        if(curTok.TOKEN_NAME.compare("int") == 0 || curTok.TOKEN_NAME.compare("float") == 0 || curTok.TOKEN_NAME.compare("string") == 0)
        {
            varDec();
        }

        if(curTok.TOKEN_NAME.compare("function") == 0)
        {
            funcDec();
        }

        if(curTok.TOKEN_ID == (int)Token::TOK_IDENTIFIER)
        {
            if(isOperator(tokenizedSampleCode[z+3].TOKEN_NAME[0]))
                binOp();
            else if(tokenizedSampleCode[z+2].TOKEN_ID == (int)Token::TOK_IDENTIFIER)
                functioncall();
            else
                valAssign();
        }
    }

    cout << "Reached EOF, all tokens analyzed" << endl;
}

//READING FROM FILE
string readFromFile(string sampleCode)
{
    string tmp;
    ifstream fptr;
    fptr.open("C:\\Users\\HP\\Desktop\\sample_code.txt");
    getline(fptr,tmp);
    while(fptr)
    {
        sampleCode += tmp;
        getline(fptr, tmp);
    }
    
    return sampleCode;
}

//LEXER HELPING FUNCTIONS
bool isDigit(char tmp)
{
    switch (tmp)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return true;
    default:
        return false;
    }
}

bool isDelimiter(char tmp)
{
    switch (tmp)
    {
    case '{':
    case '}':
    case '(':
    case ')':
    case ',':
    case ';':
        return true;
    default:
        return false;
    }
}

void identChecker(vector<Tokens> &tokens, string sampleCode, int &i)
{
    string tmp;
    tmp += sampleCode[i];
    int j = i + 1;
    while(sampleCode[j] != EOF)
    {   
        if(sampleCode[j] == ' ' && tmp.length() == 0)
            continue;
        else if(sampleCode[j] == ' ' && tmp.length() != 0)
        {
            if(find(keywords.begin(), keywords.end(), tmp) != keywords.end())
            {
                tokens.push_back({tmp, (int)Token::TOK_KEYWORD});
                i = j - 1;
                break;
            }
            else
            {
                tokens.push_back({tmp, (int)Token::TOK_IDENTIFIER});
                i = j - 1;
                break;
            }
        }
        if(isalnum(sampleCode[j]))
        {
            tmp += sampleCode[j]; 
        }
        else if(isDelimiter(sampleCode[j]))
        {
            if(find(keywords.begin(), keywords.end(), tmp) != keywords.end())
            {
                tokens.push_back({tmp, (int)Token::TOK_KEYWORD});
                i = j - 1;
                break;
            }
            else
            {
                tokens.push_back({tmp, (int)Token::TOK_IDENTIFIER});
                i = j - 1;
                break;
            }
        }
        else if(isOperator(sampleCode[j]))
        {
            if(find(keywords.begin(), keywords.end(), tmp) != keywords.end())
            {
                tokens.push_back({tmp, (int)Token::TOK_KEYWORD});
                i = j - 1;
                break;
            }
            else
            {
                tokens.push_back({tmp, (int)Token::TOK_IDENTIFIER});
                i = j - 1;
                break;
            }
        }
        j++;
    }
}

void operatorChecker(vector<Tokens> &tokens, string sampleCode, int &i)
{
    string tmp;
    tmp += sampleCode[i];
    int j = i + 1;
    while(sampleCode[j] != EOF)
    {
        if(isOperator(sampleCode[j]))
        {
            tmp += sampleCode[j];
        }
        else
        {
            tokens.push_back({tmp, (int)Token::TOK_OPERATOR});
            i = j - 1;
            break;
        }
        j++;
    }
}

void numberChecker(vector<Tokens> &tokens, string sampleCode, int &i)
{
    string tmp;
    tmp += sampleCode[i];
    int j = i + 1;
    while(sampleCode[j] != EOF)
    {
        if(isDigit(sampleCode[j]) || (sampleCode[j] == '.'))
        {
            tmp += sampleCode[j];
        }
        else 
        {
            tokens.push_back({tmp, (int)Token::TOK_NUMBERLITERAL});
            i = j - 1;
            break;
        }
        j++;
    }
}

void stringChecker(vector<Tokens> &tokens, string sampleCode, int &i)
{
    string tmp;
    int j = i + 1;
    while(sampleCode[j] != '"')
    {
        tmp+=sampleCode[j]; 

        if(sampleCode[j] == '"') break;
        j++;
    }
        tokens.push_back({tmp, (int)Token::TOK_STRINGLITERAL});
        i = j;

}

//LEXER
void Tokenizer(string sampleCode, vector<Tokens> &tokens)
{
    string numbers, operators, delimiters;
    for(int i = 0; i < sampleCode.length(); ++i)
    {
        if(sampleCode[i] == ' ')
            continue;
        else if(sampleCode[i] == '"')
        {
            stringChecker(tokens, sampleCode, i);
            continue;
        }
        else if(isalpha(sampleCode[i]))
        {
            identChecker(tokens, sampleCode, i);
            continue;
        }
        else if(isDigit(sampleCode[i]))
        {
            numberChecker(tokens, sampleCode, i);
            continue;
        }
        else if(isOperator(sampleCode[i]))
        {
            operatorChecker(tokens, sampleCode, i);
            continue;
        }
        else if(isDelimiter(sampleCode[i]))
        {
            delimiters += sampleCode[i];
            tokens.push_back({delimiters, (int)Token::TOK_DELIMITERS});
            delimiters.clear();
            continue;
        }
    }

    tokens.push_back({"EOF", (int)Token::TOK_EOF});
}


int main()
{
    int i = 0;
    string sampleCode;
    sampleCode = readFromFile(sampleCode);
    Tokenizer(sampleCode, tokenizedSampleCode);

    ParserMainLoop();

    // for(Tokens t : tokenizedSampleCode)
    //     cout << t.TOKEN_NAME << "  " << t.TOKEN_ID << endl;

    for(varDeclaration var : declaredVariables)
        cout << var.getVarName() << "   " << var.getVarType() << "  " << var.value << endl;



    for(functionDeclaration fd : declaredFunctions)
    {
        cout << "function " << fd.getFuncName() << endl;
        for(pair <string, string> arg : fd.getFuncArguments())
            cout << arg.first << " " << arg.second << endl;
    }
    return 0;
}
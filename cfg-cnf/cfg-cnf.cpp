#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct  Production
{
    string left;
    vector<string> right;
};

vector<Production> productions;

int findIndex(string nonTerminal)
{
    for(int i = 0; i < productions.size(); ++i)
    {
        if(productions[i].left == nonTerminal)
        {
            return i;
        }
    }
    return -1;
}

void readFromFile()
{
    vector<string> prod;
    ifstream in("C:\\Users\\HP\\Desktop\\cfg-cnf.txt");

    string line;
    getline(in, line);

    stringstream nonTerminals(line);
    string tmp;

    while(nonTerminals >> tmp)
        prod.push_back(tmp);

    for(string s : prod)
    {
        Production pro;
        pro.left = s;
        productions.push_back(pro);
    }

    while(getline(in, line))
    {
        stringstream p(line);
        string tmp2;

        p >> tmp2;

        int j = findIndex(tmp2);
        
        while(p >> tmp2)
            productions[j].right.push_back(tmp2);
        
    }
}

void removeUselessAndEmpty()
{
    for(int i = 0; i < productions.size(); ++i)
    {
        if(productions[i].right.size() == 0)
        {
            productions.erase(productions.begin() + i);
        }
    }

    string uselessChecker;
    uselessChecker = productions[0].left;

    for(int i = 0; i < productions.size(); ++i)
    {
        for(int j = 0; j < productions[i].right.size(); ++j)
        {
            for(int z = 0; z < productions[i].right[j].size(); ++z)
            {
                if(productions[i].right[j][z] >= 65 && productions[i].right[j][z] <= 90 && uselessChecker.find(productions[i].right[j][z]) == string::npos)
                {
                    uselessChecker += productions[i].right[j][z];
                }
            }
        }
    }

    for(int i = 0; i < productions.size(); ++i)
    {
        if(uselessChecker.find(productions[i].left) != string::npos)
            continue;
        else
        {
            productions.erase(productions.begin() + i);
        }
    }
}

bool findEpsilon(string &epsNonTerminal, int &z, int &x)
{
    for(int i = 0; i < productions.size(); ++i)
    {
        for(int j = 0; j < productions[i].right.size(); ++j)
        {
            if(productions[i].right[j].compare("z") == 0)
            {
                epsNonTerminal = productions[i].left;
                z = i;
                x = j;
                return true;
            }
        }
    }
    return false;
}

void removeEpsilon()
{
    string epsNonTerminal;
    int z, x;

    while(findEpsilon(epsNonTerminal, z, x))
    {
        productions[z].right.erase(productions[z].right.begin() + x);

        for(int i = 0; i < productions.size(); ++i)
        {
            for(int j = 0; j < productions[i].right.size(); ++j)
            {
                if(productions[i].right[j].find(epsNonTerminal) != string::npos)
                {
                    string tmp = productions[i].right[j];
                    size_t index = tmp.find(epsNonTerminal);
                    tmp.erase(tmp.begin() + index);
                    if(!tmp.empty())
                    {
                        productions[i].right.push_back(tmp);
                    }
                    else
                    {
                        productions[i].right.push_back("z");
                    }
                }
            }
        }
    }
}

void addS1()
{
    bool flag = false;
    for(int i = 1; i < productions.size(); ++i)
    {
        for(int j = 0; j < productions[i].right.size(); ++j)
        {
            if(productions[i].right[j].find("S") != string::npos)
            {
                flag = true;
            }
        }
    }

    if(flag)
    {
        Production p;
        p.left = "S1";
        p.right.push_back("S");
        productions.push_back(p);
    }
}

bool findUnit(string &unitProduction, int &x, int &y)
{
    for(int i = 0; i < productions.size(); ++i)
    {
        for(int j = 0; j < productions[i].right.size(); ++j)
        {
            if(productions[i].right[j].size() == 1 && productions[i].right[j][0] >= 65 && productions[i].right[j][0] <= 90)
            {
                unitProduction = productions[i].right[j];
                x = i;
                y = j;
                return true;
            }
        }
    }
    return false;
}

void removeUnit()
{
    string unitProduction;
    int x, y;

    while(findUnit(unitProduction, x, y))
    {
        productions[x].right.erase(productions[x].right.begin() + y);

        for(int i = 0; i < productions[findIndex(unitProduction)].right.size(); ++i)
            productions[x].right.push_back(productions[findIndex(unitProduction)].right[i]);
    }
}

string findProduction(string terminals, char term)
{
    for(int i = 0; i < productions.size(); ++i)
    {
        if(productions[i].right.size() == 1 && productions[i].right[0][0] == term)
        {
            return productions[i].left;
        }
    }
    return "error";
}

void terminalReplacements()
{
    string terminals;

    for(int i = 0; i < productions.size(); ++i)
    {
        for(int j = 0; j < productions[i].right.size(); ++j)
        {
            for(int z = 0; z < productions[i].right[j].size(); ++z)
            {
                if(productions[i].right[j][z] >= 97 && productions[i].right[j][z] <= 121 && terminals.find(productions[i].right[j][z]) == string::npos)
                {
                    terminals += productions[i].right[j][z];
                }
            }
        }
    }

    for(int i = 0; i < terminals.size(); ++i)
    {
        Production p;
        p.left = terminals[i] - 32;
        p.left += "1";
        string tmp;
        tmp += terminals[i];
        p.right.push_back(tmp);
        productions.push_back(p);
    }

    for(int i = 0; i < productions.size(); ++i)
    {
        for(int j = 0; j < productions[i].right.size(); ++j)
        {
            for(int z = 0; z < productions[i].right[j].size(); ++z)
            {
                if(productions[i].right[j][z] >= 97 && productions[i].right[j][z] <= 121 && productions[i].right[j].size() != 1)
                {
                    string tmp;
                    tmp = findProduction(terminals, productions[i].right[j][z]);
                    size_t index = productions[i].right[j].find(productions[i].right[j][z]);
                    productions[i].right[j].replace(index, tmp.size() - 1, tmp);
                }
            }
        }
    }
}

bool findInvalidProductions(string &tmp)
{
    int n = 0;
    for(int i = 0; i < productions.size(); ++i)
    {
        for(int j = 0; j < productions[i].right.size(); ++j)
        {
            n = 0;
            for(int k = 0; k < productions[i].right[j].size(); ++k)
            {
                if(isalpha(productions[i].right[j][k]))
                    n++;
                else
                    continue;
                  
            }

            if(n > 2)
            {
                tmp = productions[i].right[j];
                return true;
            }
        }
    }
    return false;
}

bool checkifValid(int i, int j)
{
    int flag = 0;
    for(int k = 0; k < productions[i].right[j].size(); ++k)
    {
        if(isalpha(productions[i].right[j][k]))
            flag++;
        else
            continue;
    }

    if(flag > 2)
    {
        return true;
    }
    else
        return false;
}

void changeAllOccurences(string newProd, int x)
{
    Production p1;
    string newLeft = "X";
    char tmp = '0' + x;
    newLeft += tmp;
    p1.left = newLeft;
    p1.right.push_back(newProd);
    for(int i = 0; i < productions.size(); ++i)
    {
        for(int j = 0; j < productions[i].right.size(); ++j)
        {
            while(productions[i].right[j].find(newProd) != string::npos && checkifValid(i, j))
            {
                size_t index = productions[i].right[j].find(newProd);
                productions[i].right[j].replace(index, newProd.size(), newLeft);
            }
        }
    }
    productions.push_back(p1);
}

void newProductions()
{
    int x = 1;
    string tmp;
    string newProd;
    int flag = 0;

    while(findInvalidProductions(tmp))
    {
        for(int i = 0; i < tmp.size(); i++)
        {
            if(isalpha(tmp[i]))
            {
                newProd += tmp[i];
                flag++;
                int j = i + 1;
                while(isdigit(tmp[j]))
                {
                    newProd += tmp[j];
                    j++;
                }
            }
            
            if(flag == 2)
            {
                changeAllOccurences(newProd, x);
                newProd.clear();
                flag = 0;
                break;
            }
    
        }
        
        x++;
    }
}

int main()
{
    readFromFile();
    removeUselessAndEmpty();
    removeEpsilon();
    addS1();
    removeUnit();
    terminalReplacements();
    newProductions();

    for(Production p : productions)
    {
        cout << p.left << "  ";
        for(string s : p.right)
        {
            cout << s << "  ";
        }
        cout << endl;
    }

    return 0;
}
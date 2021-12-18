#include "utils.h"

void getCommand(const string &command, Option &opt, string &var) {
    var.clear();
    if (command[1] == 'I') {
        opt = ISBN;
        for (int i = 6, sz = command.size(); i < sz; ++i)
            var += command[i]; 
    } else if (command[1] == 'n') {
        opt = NAME;
        for (int i = 7, sz = command.size(); i < sz - 1; ++i)
            var += command[i];
    } else if (command[1] == 'a') {
        opt = AUTHOR;
        for (int i = 9, sz = command.size(); i < sz - 1; ++i)
            var += command[i];
    } else if (command[1] == 'k') {
        opt = KEYWORD;
        for (int i = 10, sz = command.size(); i < sz - 1; ++i)
            var += command[i];
    } else if (command[1] == 'p') {
        opt = PRICE;
        for (int i = 7; i < command.size(); ++i)
            var += command[i];
    }
}

void checkLen(const string &str, const int &maxLength) {
    if (str.size() > maxLength)
        throw Exception("len > maxlen");
}

bool getCommand(std::vector<string> &command) {
    command.clear();
    string line, tmp;
    tmp.clear();
    if (!getline(std::cin, line))
        return false;
    bool isquote = 0;
    for (auto ch : line) {
        if (ch == '"') isquote ^= 1;
        if ((isquote == 0 && ch == ' ') || ch == '\n') {
            if (!tmp.empty())
                command.push_back(tmp);
            tmp.clear();
        } else tmp += ch;
    }
    
    if (!tmp.empty())
        command.push_back(tmp);
    return 1;
}

void multiVarCheck(const std::vector<string>& var, Ull &book) {
    bool visisbn(0), visname(0), visauthor(0), viskeyword(0), visprice(0);
    for (auto command : var) {
        if (command[1] == 'I') {
            if (visisbn)
                throw Exception("modify: multi-ISBN");
            visisbn = 1;
            if (command.size() == 6)
                throw Exception("modify: -ISBN = NULL");
            string var;
            for (int i = 6, sz = command.size(); i < sz; ++i)
                var += command[i]; 
            std::vector<int> res;
            Varchar<20> isbn(var);
            book.query(isbn, res);
            if (res.size())
                throw Exception("modify: ISBN exists");
        } else if (command[1] == 'n') {
            if (visname)
                throw Exception("modify: multi-bookname");
            visname = 1;
            if (command.size() == 8)
                throw Exception("modify: -name = NULL");
            int cnt = 0;
            for (int i = 7, sz = command.size(); i < sz - 1; ++i, ++cnt)
                if (command[i] == '\"')
                    throw Exception("modify: -name \" is invalid"); 
            if (cnt > 60)
                throw Exception("modify: -name: string is too long");
        } else if (command[1] == 'a') {
            if (visauthor)
                throw Exception("modify: multi-author");
            visauthor = 1;
            if (command.size() == 10)
                throw Exception("-author = NULL");
            int cnt = 0;
            for (int i = 9, sz = command.size(); i < sz - 1; ++i, ++cnt)
                if (command[i] == '\"')
                    throw Exception("modify: -author \" is invalid"); 
            if (cnt > 60)
                throw Exception("modify: -author: string is too long");
        } else if (command[1] == 'k') {
            if (viskeyword)
                throw Exception("modify: multi-keyword");
            viskeyword = 1;
            if (command.size() == 11)
                throw Exception("-keyword = NULL");
            multiKeywordCheck(command);
            int cnt = 0;
            for (int i = 10, sz = command.size(); i < sz - 1; ++i, ++cnt)
                if (command[i] == '\"')
                    throw Exception("modify: -keyword \" is invalid"); 
            if (cnt > 60)
                throw Exception("modify: -keyword: string is too long");
        } else if (command[1] == 'p') {
            if (visprice)
                throw Exception("modify: multi-price");
            visprice=1;
            if (command.size() == 7) 
                throw Exception("-price = NULL");
            int cnt = 0;
            for (int i = 7; i < command.size(); ++i, ++cnt)
                if (command[i] != '.' && !isdigit(command[i]))
                    throw Exception("modify: -price: something strange appeared");
            if (cnt > 13)
                throw Exception("price string is too long");
        } else throw Exception("modify: - you mean what?");
    }
}

void multiKeywordCheck(const string &kw) {
    std::unordered_map<string, bool> vis;
    string tmp; tmp.clear();
    for (int i = 10, sz = kw.size(); i < sz - 1; ++i) {
        if (kw[i] == '|') {
            if (vis[tmp])
                throw Exception("modify -keyword: Keyword Reappeared");
            vis[tmp] = 1;
            tmp.clear();
        } else tmp += kw[i];
    }
    if (vis[tmp])
        throw Exception("keyword Reappeared");
}
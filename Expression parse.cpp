#include <bits/stdc++.h>
using namespace std;

struct Node {
    Node* left;
    Node* right;
    string val;

    Node(Node* nl, Node* nr, string nv) {
        left = nl;
        right = nr;
        val = nv;
    }

    bool is_leaf() const {
        return (left == nullptr && right == nullptr);
    }

    void print() const {
        if (!is_leaf())
            cout << '(';
        cout << val;
        if (left != nullptr) {
            cout << ",";
            left->print();
            cout << ",";
        }
        if (right != nullptr)
            right->print();
        if (!is_leaf())
            cout << ')';
    }
};

enum Token {
    VAR,
    IMP,
    OR,
    AND,
    NEG,
    OPB,
    CLB,
    DEF
};

int priority(Token v) { // lowest first
    switch (v) {
        case NEG:
            return 0;
        case AND:
            return 1;
        case OR:
            return 2;
        case IMP:
            return 3;
    }
}

string symb(Token v) {
    switch (v) {
        case NEG:
            return "!";
        case IMP:
            return "->";
        case OR:
            return "|";
        case AND:
            return "&";
    }
}

class Tokenizer {
public:
    Tokenizer(const string &text) {
        s = text;
        n = (int)text.size();
        i = 0;
        cur_var = "";
    }

    string poll_var() {
        string res = cur_var;
        cur_var = "";
        return res;
    }

    Token get_next_token() {
        if (i == n)
            return DEF;
        Token res = DEF;
        switch (s[i]) {
            case '-':
                res = IMP;
                i++;
                break;
            case '|':
                res = OR;
                break;
            case '&':
                res = AND;
                break;
            case '!':
                res = NEG;
                break;
            case '(':
                res = OPB;
                break;
            case ')':
                res = CLB;
                break;
            default:
                res = VAR;
                fill_var();
                i--;
                break;
        }
        i++;
        return res;
    }

private:
    string s, cur_var;
    int i, n;

    bool is_var_char(char c) {
        return ('A' <= c && c <= 'Z' || '0' <= c && c <= '9' || c == '\'');
    }

    void fill_var() {
        while (i < n && is_var_char(s[i])) {
            cur_var.push_back(s[i]);
            i++;
        }
    }
};

void build(stack <Node*> &arg, stack <Token> &op) {
    if (op.top() == NEG) {
        arg.top() = new Node(nullptr, arg.top(), symb(NEG));
    } else {
        Node* second = arg.top();
        arg.pop();
        Node* first = arg.top();
        arg.top() = new Node(first, second, symb(op.top()));
    }
    op.pop();
}

Node* parse(Tokenizer &t) {
    stack <Node*> arg;
    stack <Token> op;

    while (true) {
        Token cur = t.get_next_token();
        switch (cur) {
            case AND:
            case OR:
                while (!op.empty() && priority(op.top()) <= priority(cur)) {
                    build(arg, op);
                }
                op.push(cur);
                break;
            case IMP:
                while (!op.empty() && priority(op.top()) < priority(cur)) {
                    build(arg, op);
                }
                op.push(cur);
                break;
            case NEG:
                op.push(cur);
                break;
            case VAR:
                arg.push(new Node(nullptr, nullptr, t.poll_var()));
                break;
            case OPB:
                arg.push(parse(t));
                break;
            default:
                while (!op.empty()) {
                    build(arg, op);
                }
                return arg.top();
        }
    }
}

void rm_ws(string &s) {
    set<char> q;
    for (char c = 'A'; c <= 'Z'; c++)
        q.insert(c);
    for (char c = '0'; c <= '9'; c++)
        q.insert(c);
    q.insert('\'');
    q.insert('&');
    q.insert('|');
    q.insert('!');
    q.insert('-');
    q.insert('>');
    q.insert('(');
    q.insert(')');

    string res = "";
    for (int i = 0; i < (int)s.size(); i++) {
        if (q.count(s[i]))
            res.push_back(s[i]);
    }
    s = res;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    string s;
    getline(cin, s);
    rm_ws(s);
    Tokenizer t = Tokenizer(s);
    Node* root = parse(t);
    root->print();
    return 0;
}

#include <bits/stdc++.h>
using namespace std;

std::hash<string> str_hasher;

struct Node {
    Node* left;
    Node* right;
    string val;

    Node() = default;

    Node(Node* nl, Node* nr, string nv) {
        left = nl;
        right = nr;
        val = nv;
    }

    bool is_leaf() const {
        return (left == nullptr && right == nullptr);
    }

    bool is_full() const {
        return (left != nullptr && right != nullptr);
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

    string get_str() const {
        string res = "";
        if (!is_leaf())
            res.push_back('(');
        res += val;
        if (left != nullptr) {
            res.push_back(',');
            res += left->get_str();
            res.push_back(',');
        }
        if (right != nullptr)
            res += right->get_str();
        if (!is_leaf())
            res.push_back(')');
        return res;
    }

    string canonical_str() const {
        string res = "";
        if (is_full())
            res.push_back('(');
        if (left != nullptr) {
            res += left->canonical_str();
            res.push_back(' ');
        }
        res += val;
        if (right != nullptr) {
            if (val != "!")
                res.push_back(' ');
            res += right->canonical_str();
        }
        if (is_full())
            res.push_back(')');
        return res;
    }

    friend bool operator==(const Node& w, const Node& q);
};

bool operator==(const Node& w, const Node& q) {
    if (w.is_leaf() != q.is_leaf())
        return false;
    if (w.is_leaf())
        return w.val == q.val;
    if (w.val == q.val) {
        bool res = true;
        if (w.val != "!")
            res &= (*w.left == *q.left);
        res &= (*w.right == *q.right);
        return res;
    } else {
        return false;
    }
}

struct Hasher {
    size_t operator()(const Node& x) const {
        return str_hasher(x.get_str());
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

Node get_Node(string s) {
    rm_ws(s);
    Tokenizer t = Tokenizer(s);
    return *parse(t);
}

class Axiom {
public:
    Node get_axiom(int num) {
        return axiom[num];
    }

    int is_axiom(Node expr) {
        for (int i = 1; i <= 10; i++) {
            unordered_map<string, Node> var_match;
            if (match_ax(get_axiom(i), expr, var_match))
                return i;
        }
        return -1;
    }

private:
    unordered_map<int, Node> axiom = create_axioms();

    unordered_map<int, Node> create_axioms() {
        unordered_map<int, Node> res;
        res[1] = get_Node("A->(B->A)");
        res[2] = get_Node("(A->B)->(A->B->C)->(A->C)");
        res[3] = get_Node("A->B->A&B");
        res[4] = get_Node("A&B->A");
        res[5] = get_Node("A&B->B");
        res[6] = get_Node("A->A|B");
        res[7] = get_Node("B->A|B");
        res[8] = get_Node("(A->C)->(B->C)->(A|B->C)");
        res[9] = get_Node("(A->B)->(A->!B)->!A");
        res[10] = get_Node("!!A->A");
        return res;
    }

    bool match_ax(Node a, Node v, unordered_map<string, Node> &var_match) {
        if (a.is_leaf()) {
            if (var_match.count(a.val)) {
                return var_match[a.val] == v;
            } else {
                var_match[a.val] = v;
                return true;
            }
        }
        if (a.val == v.val) {
            bool res = true;
            if (a.val != "!")
                res &= match_ax(*a.left, *v.left, var_match);
            res &= match_ax(*a.right, *v.right, var_match);
            return res;
        } else {
            return false;
        }
    }
};

int main() {
    //freopen("input.txt", "r", stdin);
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    string s, prevs;
    getline(cin, s);

    unordered_map<Node, int, Hasher> hyp;
    string cur = "";
    int hyp_num = 1, res_num = -1;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == ',' || (s[i] == '|' && s[i + 1] == '-')) {
            if (!cur.empty()) {
                hyp[get_Node(cur)] = hyp_num++;
                cur = "";
            }
            if (s[i] == '|')
                i++;
            continue;
        }
        cur.push_back(s[i]);
    }

    Node result = get_Node(cur);

    int line_numeric = 0;
    Axiom ax_helper = Axiom();
    unordered_map<Node, int, Hasher> num;
    unordered_map<Node, pair<int, int>, Hasher> provably;
    unordered_map<Node, vector<int>, Hasher> impl;
    vector <pair<int, int> > from;
    vector <Node> expr_arr;

    while (getline(cin, s)) {
        prevs = s;
        bool norm = false;
        Node expr = get_Node(s);
        if (num.count(expr))
            continue;
        if (hyp.count(expr)) {
            from.push_back(make_pair(-1, hyp[expr]));
            norm = true;
        } else {
            int axnum = ax_helper.is_axiom(expr);
            if (axnum != -1) {
                from.push_back(make_pair(-2, axnum));
                norm = true;
            } else {
                if (provably.count(expr)) {
                    pair<int, int> cnums = provably[expr];
                    from.push_back(cnums);
                    norm = true;
                }
            }
        }
        if (!norm) {
            cout << "Proof is incorrect";
            return 0;
        }
        num[expr] = line_numeric;
        expr_arr.push_back(expr);
        if (expr.val == "->") {
            if (num.count(*expr.left)) {
                provably[*expr.right] = make_pair(line_numeric, num[*expr.left]);
            }
            impl[*expr.left].push_back(line_numeric);
        }
        while (!impl[expr].empty()) {
            provably[*expr_arr[impl[expr].back()].right] = make_pair(impl[expr].back(), line_numeric);
            impl[expr].pop_back();
        }
        if (expr == result)
            res_num = line_numeric;
        line_numeric++;
    }

    if (!(get_Node(prevs) == result)) {
        cout << "Proof is incorrect";
        return 0;
    }

    while ((int)expr_arr.size() > res_num + 1) {
        expr_arr.pop_back();
        from.pop_back();
    }

    vector <bool> used(expr_arr.size());
    used.back() = true;
    for (int i = (int)used.size() - 1; i >= 0; i--) {
        if (used[i] && from[i].first >= 0) {
            used[from[i].first] = true;
            used[from[i].second] = true;
        }
    }

    line_numeric = 1;
    vector <int> new_num(used.size(), -1);
    vector <pair<string, string>> arr;
    for (int i = 0; i < (int)used.size(); i++) {
        if (!used[i])
            continue;
        new_num[i] = line_numeric;
        if (from[i].first == -1) {
            arr.push_back(make_pair("[" + to_string(line_numeric) + ". Hypothesis " + to_string(from[i].second) + "]", expr_arr[i].canonical_str()));
        } else {
            if (from[i].first == -2) {
                arr.push_back(make_pair("[" + to_string(line_numeric) + ". Ax. sch. " + to_string(from[i].second) + "]", expr_arr[i].canonical_str()));
            } else {
                arr.push_back(make_pair("[" + to_string(line_numeric) + ". M.P. " + to_string(new_num[from[i].first]) + ", " + to_string(new_num[from[i].second]) + "]", expr_arr[i].canonical_str()));
            }
        }
        line_numeric++;
    }

    //------------------------------------------------------------------------------

    vector <Node> first_line((int)hyp.size());
    for (auto it = hyp.begin(); it != hyp.end(); it++) {
        first_line[it->second - 1] = it->first;
    }

    for (int i = 0; i < (int)first_line.size(); i++) {
        cout << first_line[i].canonical_str();
        if (i != (int)first_line.size() - 1)
            cout << ", ";
    }
    if (!first_line.empty())
        cout << " ";
    cout << "|- " << result.canonical_str() << '\n';

    for (int i = 0; i < (int)arr.size(); i++)
        cout << arr[i].first << " " << arr[i].second << '\n';


    return 0;
}

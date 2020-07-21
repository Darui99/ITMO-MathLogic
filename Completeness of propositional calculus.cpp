#include <iostream>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <string>
#include <bitset>
#include <ctime>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <complex>
#include <assert.h>
#pragma comment(linker, "/STACK:256000000")
using namespace std;

std::hash<string> str_hasher;

bool calc(bool a, bool b, string op) {
	if (op == "!")
		return !b;
	if (op == "|")
		return a | b;
	if (op == "&")
		return a & b;
	if (op == "->")
		return !a | b;
}

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

	Node get_copy() const {
		Node* L = nullptr;
		if (left != nullptr)
			L = new Node(left->get_copy());
		Node* R = nullptr;
		if (right != nullptr)
			R = new Node(right->get_copy());
		Node res = Node(L, R, val);
		res.evaluated = evaluated;
		return res;
	}

	void var_map(map<string, string> &f) {
		if (is_leaf()) {
			if (f.count(val))
				val = f[val];
			return;
		}
		if (left != nullptr)
			left->var_map(f);
		if (right != nullptr)
			right->var_map(f);
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

	bool eval(const int &mask) {
		if (evaluated.count(mask))
			return evaluated[mask];

		if (is_leaf()) {
			int p;
			if (val == "A")
				p = 0;
			if (val == "B")
				p = 1;
			if (val == "C")
				p = 2;
			evaluated[mask] = (1 << p) & mask;
			return evaluated[mask];
		}

		if (val == "!")
			evaluated[mask] = !right->eval(mask);
		else
			evaluated[mask] = calc(left->eval(mask), right->eval(mask), val);
		return evaluated[mask];
	}

	friend bool operator==(const Node& w, const Node& q);

private:
	unordered_map<int, bool> evaluated;
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
	}
	else {
		return false;
	}
}

static Node Empty = Node(nullptr, nullptr, "");

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
	}
	else {
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

struct Subs {
public:
	static Node safe_substitute(const vector<Node> &args, const Node &base) {
		Node res = base.get_copy();
		change_leafs(res, args);
		return res;
	}

	/*static void inplace_substitute(const vector<Node> &args, Node &base) {
		change_leafs(base, args);
	}*/

private:
	static void change_leafs(Node& v, const vector<Node> &args) {
		if (v.is_leaf()) {
			if ((int)(v.val[0] - 'A') < (int)args.size())
				v = args[(int)(v.val[0] - 'A')];
			return;
		}
		if (v.left != nullptr)
			change_leafs(*v.left, args);
		if (v.right != nullptr)
			change_leafs(*v.right, args);
	}
};

class Axiom {
public:
	static Node get_axiom(int num) {
		return axiom[num];
	}

	static int is_axiom(Node expr) {
		for (int i = 1; i <= 10; i++) {
			unordered_map<string, Node> var_match;
			if (match_ax(get_axiom(i), expr, var_match))
				return i;
		}
		return -1;
	}

	static Node subs(const vector<Node> &args, int num) {
		return Subs::safe_substitute(args, get_axiom(num));
	}

private:
	static vector<Node> create_axioms() {
		vector<Node> res(11);
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

	static vector<Node> axiom;

	static bool match_ax(const Node &a, const Node &v, unordered_map<string, Node> &var_match) {
		if (a.is_leaf()) {
			if (var_match.count(a.val)) {
				return var_match[a.val] == v;
			}
			else {
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
		}
		else {
			return false;
		}
	}
};

vector<Node> Axiom::axiom = Axiom::create_axioms();

Node wrap(const Node &left, const Node &right, Token op) {
	Node* L;
	if (op == NEG)
		L = nullptr;
	else
		L = new Node(left);
	return Node(L, new Node(right), symb(op));
}

Node wrap(const string &left, const string &right, Token op) {
	return wrap(get_Node(left), get_Node(right), op);
}

struct proof {

	proof() = default;

	proof(const unordered_set <Node, Hasher> &nh, const vector<Node> &nl) {
		hyps = nh;
		lines = nl;
	}

	proof(const vector <string> &nh, const vector<string> &nl) {
		for (auto it = nh.begin(); it != nh.end(); it++)
			hyps.insert(get_Node(*it));
		for (auto it = nl.begin(); it != nl.end(); it++)
			lines.push_back(get_Node(*it));
	}

	void merge(const proof &p) {
		/*if (hyps != p.hyps)
		throw 322;*/
		for (auto it = p.lines.begin(); it != p.lines.end(); it++)
			lines.push_back(*it);
	}

	Node get_result() const {
		return lines.back();
	}

	void print(const string &st) {
		int rem = (int)hyps.size();
		for (auto it = hyps.begin(); it != hyps.end(); it++) {
			cout << it->canonical_str();
			if (--rem > 0)
				cout << ',';
			cout << ' ';
		}
		cout << "|- " << st << '\n';
		for (int i = 0; i < (int)lines.size(); i++)
			cout << lines[i].canonical_str() << '\n';
	}

	void apply_deduction(const string &var) {
		Node a = get_Node(var);
		if (hyps.count(a) == 0)
			throw 322;
		hyps.erase(a);

		vector <Node> nl;
		unordered_map<Node, Node, Hasher> from;
		for (int i = 0; i < (int)lines.size(); i++) {
			if (Axiom::is_axiom(lines[i]) != -1 || hyps.count(lines[i])) {
				nl.push_back(lines[i]);
				nl.push_back(Axiom::subs({ lines[i], a }, 1));
			}
			else {
				if (lines[i] == a) {
					nl.push_back(Axiom::subs({ a, a }, 1));
					nl.push_back(Axiom::subs({ a, wrap(a, a, IMP), a }, 2));
					nl.push_back(*nl.back().right);
					nl.push_back(Axiom::subs({ a, wrap(a, a, IMP) }, 1));
				}
				else {
					if (from.count(lines[i]) == 0)
						throw 322;
					nl.push_back(Axiom::subs({ a, from[lines[i]], lines[i] }, 2));
					nl.push_back(*nl.back().right);
				}
			}
			if (lines[i].val == "->") {
				from[*lines[i].right] = *lines[i].left;
			}
			nl.push_back(wrap(a, lines[i], IMP));
		}
		lines = nl;
	}

	void subs(const vector<Node> &args) {
		unordered_set <Node, Hasher> nh;
		for (auto it = hyps.begin(); it != hyps.end(); it++) {
			nh.insert(Subs::safe_substitute(args, *it));
		}
		hyps = nh;
		for (int i = 0; i < (int)lines.size(); i++)
			//Subs::inplace_substitute(args, lines[i]);
			lines[i] = Subs::safe_substitute(args, lines[i]);
	}

	void text_subs(map<string, string> &args) {
		unordered_set <Node, Hasher> nh;
		for (auto it = hyps.begin(); it != hyps.end(); it++) {
			Node ch = *it;
			ch.var_map(args);
			nh.insert(ch);
		}
		hyps = nh;
		for (int i = 0; i < (int)lines.size(); i++)
			//Subs::inplace_substitute(args, lines[i]);
			//lines[i] = Subs::safe_substitute(args, lines[i]);
			lines[i].var_map(args);
	}

private:
	unordered_set <Node, Hasher> hyps;
	vector <Node> lines;
};

struct Counterposition {

	static proof get_counterposition(const Node &a, const Node &b) {
		proof res = get_base();
		res.apply_deduction("!B");
		res.apply_deduction("A -> B");
		res.subs({ a, b });
		return res;
	}

	static proof get_invCounterposition(const Node &a, const Node &b) {
		proof res = get_base();
		res.apply_deduction("A -> B");
		res.apply_deduction("!B");
		res.subs({ a, b });
		return res;
	}

	static proof get_revCounterposition(const Node &a, const Node &b) {
		proof res({ "!B -> !A", "A" },
			vector<string>({
			"(!B -> A) -> ((!B -> !A) -> !!B)",
			"A -> (!B -> A)",
			"A",
			"!B -> A",
			"(!B -> !A) -> !!B",
			"!B -> !A",
			"!!B",
			"!!B -> B",
			"B"
		}));
		res.apply_deduction("A");
		res.apply_deduction("!B -> !A");
		res.subs({ a, b });
		return res;
	}

private:
	static proof get_base() {
		return proof({ "A -> B", "!B" },
			vector<string>({
			"(A -> B) -> ((A -> !B) -> !A)",
			"A -> B",
			"(A -> !B) -> !A",
			"!B -> (A -> !B)",
			"!B",
			"A -> !B",
			"!A"
		}));
	}
};

proof AOrNotA(const Node &a) {
	proof res = Counterposition::get_counterposition(get_Node("A"), get_Node("A | !A"));
	res.merge(proof(vector<string>(),
		vector<string>({
		"A -> (A | !A)",
		"!(A | !A) -> !A"
	})));
	res.merge(Counterposition::get_counterposition(get_Node("!A"), get_Node("A | !A")));
	res.merge(proof(vector<string>(),
		vector<string>({
		"!A -> (A | !A)",
		"!(A | !A) -> !!A",
		"(!(A | !A) -> !A) -> (!(A | !A) -> !!A) -> !!(A | !A)",
		"(!(A | !A) -> !!A) -> !!(A | !A)",
		"!!(A | !A)",
		"!!(A | !A) -> (A | !A)",
		"A | !A"
	})));
	res.subs({ a });
	return res;
}

struct Bind {

	static proof get_bind(int num) {
		return binds[num];
	}

	static proof pick_bind(bool a, bool b, string op) {
		if (op == "!") {
			if (b)
				return get_bind(13);
			else
				return get_bind(14);
		}
		if (op == "&") {
			if (a && b)
				return get_bind(4);
			if (!a && b)
				return get_bind(2);
			if (a && !b)
				return get_bind(3);
			if (!a && !b)
				return get_bind(1);
		}
		if (op == "|") {
			if (a && b)
				return get_bind(8);
			if (!a && b)
				return get_bind(6);
			if (a && !b)
				return get_bind(7);
			if (!a && !b)
				return get_bind(5);
		}
		if (op == "->") {
			if (a && b)
				return get_bind(12);
			if (!a && b)
				return get_bind(10);
			if (a && !b)
				return get_bind(11);
			if (!a && !b)
				return get_bind(9);
		}
	}

private:
	static vector<proof> create_binds() {
		vector<proof> res(15);
		res[1] = bind1();
		res[2] = bind2();
		res[3] = bind3();
		res[4] = bind4();
		res[5] = bind5();
		res[6] = bind6();
		res[7] = bind7();
		res[8] = bind8();
		res[9] = bind9();
		res[10] = bind10();
		res[11] = bind11();
		res[12] = bind12();
		res[13] = bind13();
		res[14] = bind14();
		return res;
	}

	static proof bind1() {
		return proof({ "!A", "!B" },
			vector<string>({
			"!B",
			"!B -> ((A & B) -> !B)",
			"(A & B) -> !B",
			"(A & B) -> B",
			"((A & B) -> B) -> (((A & B) -> !B) -> !(A & B))",
			"((A & B) -> !B) -> !(A & B)",
			"!(A & B)"
		}));
	}

	static proof bind2() {
		return proof({ "!A", "B" },
			vector<string>({
			"!A",
			"!A -> ((A & B) -> !A)",
			"(A & B) -> !A",
			"(A & B) -> A",
			"((A & B) -> A) -> (((A & B) -> !A) -> !(A & B))",
			"((A & B) -> !A) -> !(A & B)",
			"!(A & B)"
		}));
	}

	static proof bind3() {
		return proof({ "A", "!B" },
			vector<string>({
			"!B",
			"!B -> ((A & B) -> !B)",
			"(A & B) -> !B",
			"(A & B) -> B",
			"((A & B) -> B) -> (((A & B) -> !B) -> !(A & B))",
			"((A & B) -> !B) -> !(A & B)",
			"!(A & B)"
		}));
	}

	static proof bind4() {
		return proof({ "A", "B" },
			vector<string>({
			"A->B->(A&B)",
			"A",
			"B->(A&B)",
			"B",
			"A&B"
		}));
	}

	static proof bind5() {
		proof res({ "!A", "!B" },
			vector<string>({
			"(A | B -> B) -> (A | B -> !B) -> !(A | B)",
			"(A -> B) -> (B -> B) -> (A | B -> B)"
		}));
		res.merge(bind9());
		res.merge(proof({ "!A", "!B" },
			vector<string>({
			"(B -> B) -> (A | B -> B)",
			"B -> (B -> B)",
			"(B -> (B -> B)) -> ((B -> ((B -> B) -> B)) -> (B -> B))",
			"(B -> ((B -> B) -> B)) -> (B -> B)",
			"B -> ((B -> B) -> B)",
			"B -> B",
			"A | B -> B",
			"(A | B -> !B) -> !(A | B)",
			"!B",
			"!B -> A | B -> !B",
			"A | B -> !B",
			"!(A | B)"
		})));
		return res;
	}

	static proof bind6() {
		return proof({ "!A", "B" },
			vector<string>({
			"B->(A|B)",
			"B",
			"A|B"
		}));
	}

	static proof bind7() {
		return proof({ "A", "!B" },
			vector<string>({
			"A->(A|B)",
			"A",
			"A|B"
		}));
	}

	static proof bind8() {
		return proof({ "A", "B" },
			vector<string>({
			"B->(A|B)",
			"B",
			"A|B"
		}));
	}

	static proof bind9() {
		proof res({ "!A", "!B" }, vector<string>());
		res.merge(Counterposition::get_revCounterposition(get_Node("A"), get_Node("B")));
		res.merge(proof({ "!A", "!B" },
			vector<string>({
			"!A",
			"!A -> (!B -> !A)",
			"!B -> !A",
			"A -> B"
		})));
		return res;
	}

	static proof bind10() {
		return proof({ "!A", "B" },
			vector<string>({
			"B->(A->B)",
			"B",
			"A->B"
		}));
	}

	static proof bind11() {
		proof res({ "A", "!B" },
			vector<string>({
			"((A -> B) -> A) -> ((A -> B) -> !A) -> !(A -> B)",
			"A -> ((A -> B) -> A)",
			"A",
			"(A -> B) -> A",
			"((A -> B) -> !A) -> !(A -> B)"
		}));
		res.merge(Counterposition::get_invCounterposition(get_Node("A"), get_Node("B")));
		res.merge(proof({ "A", "!B" },
			vector<string>({
			"!B",
			"(A -> B) -> !A",
			"!(A -> B)"
		})));
		return res;
	}

	static proof bind12() {
		return proof({ "A", "B" },
			vector<string>({
			"B->(A->B)",
			"B",
			"A->B"
		}));
	}

	static proof bind13() {
		return proof({ "A" },
			vector<string>({
			"(!A -> A) -> ((!A -> !A) -> !!A)",
			"A->(!A->A)",
			"!A -> (!A -> !A)",
			"(!A -> (!A -> !A)) -> ((!A -> ((!A -> !A) -> !A)) -> (!A -> !A))",
			"(!A -> ((!A -> !A) -> !A)) -> (!A -> !A)",
			"!A -> ((!A -> !A) -> !A)",
			"!A -> !A",
			"A",
			"!A->A",
			"(!A->!A)->!!A",
			"!!A"
		}));
	}

	static proof bind14() {
		return proof({ "!A" },
			vector<string>({
			"!A"
		}));
	}

	static vector<proof> binds;
};

vector<proof> Bind::binds = Bind::create_binds();

const int VAR_MAX = 3;

bool generate_proof(Node& v, int mask, proof &p) {
	if (v.is_leaf()) {
		return v.eval(mask);
	}
	bool Lb = false;
	if (v.left != nullptr)
		Lb = generate_proof(*v.left, mask, p);
	bool Rb = generate_proof(*v.right, mask, p);
	proof cur = Bind::pick_bind(Lb, Rb, v.val);

	if (v.val == "!")
		cur.subs({ *v.right });
	else
		cur.subs({ *v.left, *v.right });
	p.merge(cur);
	return calc(Lb, Rb, v.val);
}

int popcnt(int x) {
	int res = 0;
	while (x) {
		res++;
		x &= (x - 1);
	}
	return res;
}

char cvm = 'A';
map<string, string> vmon;
map<string, string> vmno;

void set_vm(Node &v) {
	if (v.is_leaf()) {
		if (vmon.count(v.val)) {
			v.val = vmon[v.val];
		}
		else {
			string cur = "";
			cur.push_back(cvm++);
			vmno[cur] = v.val;
			vmon[v.val] = cur;
			v.val = cur;
		}
		return;
	}
	if (v.left != nullptr)
		set_vm(*v.left);
	if (v.right != nullptr)
		set_vm(*v.right);
}

int main()
{
	//freopen("output.txt", "w", stdout);
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);
	cout.tie(nullptr);

	string expr;
	getline(cin, expr);
	Node tree = get_Node(expr);

	set_vm(tree);

	int hyp = -1, hypcnt = VAR_MAX + 1;
	for (int mask = 0; mask < (1 << VAR_MAX); mask++) {
		bool norm = true;
		for (int supermask = 0; supermask < (1 << VAR_MAX); supermask++) {
			if ((supermask & mask) != mask)
				continue;
			norm &= tree.eval(supermask);
		}
		int chc = popcnt(mask);
		if (norm && chc < hypcnt) {
			hyp = mask;
			hypcnt = chc;
		}
	}

	if (hyp != -1) {
		vector <string> all_hyps;
		for (int i = 0; i < VAR_MAX; i++) {
			if ((1 << i) & hyp) {
				all_hyps.push_back("");
				all_hyps.back().push_back('A' + i);
			}
		}
		proof all(all_hyps, vector<string>());
		vector <Node> arr((1 << VAR_MAX));
		for (int mask = 0; mask < (1 << VAR_MAX); mask++) {
			if ((mask & hyp) != hyp)
				continue;
			vector <string> str_hyps;
			for (int i = 0; i < VAR_MAX; i++) {
				str_hyps.push_back("");
				if (!((1 << i) & mask))
					str_hyps.back().push_back('!');
				str_hyps.back().push_back('A' + i);
			}
			proof cur(str_hyps, vector<string>());
			generate_proof(tree, mask, cur);
			for (int i = 0; i < VAR_MAX; i++) {
				if (!((1 << i) & hyp))
					cur.apply_deduction(str_hyps[i]);
			}
			arr[mask] = cur.get_result();
			all.merge(cur);
		}

		for (int i = 0; i < VAR_MAX; i++) {
			if (!((1 << i) & hyp)) {
				string cur = "";
				cur.push_back('A' + i);
				all.merge(AOrNotA(get_Node(cur)));
			}
		}

		for (int i = VAR_MAX - 1; i >= 0; i--) {
			if ((1 << i) & hyp)
				continue;
			string p = "", np = "!";
			p.push_back('A' + i);
			np.push_back('A' + i);
			for (int mask = 0; mask < (1 << VAR_MAX); mask++) {
				if ((mask & hyp) != hyp)
					continue;
				if ((1 << i) & mask)
					continue;
				Node a = *arr[mask].right;
				Node tmp = Axiom::subs({ get_Node(p), get_Node(np), a }, 8);
				proof cur(vector<string>(),
					vector<string>({
					tmp.canonical_str(),
					tmp.right->canonical_str(),
					tmp.right->right->canonical_str(),
					a.canonical_str()
				}));
				all.merge(cur);
				arr[mask] = arr[mask | (1 << i)] = a;
			}
			hyp |= (1 << i);
		}
		//all.subs({ get_Node("X"), get_Node("Y") });
		/*vector<Node> ov;
		for (auto it = vmno.begin(); it != vmno.end(); it++)
			ov.push_back(get_Node(it->second));
		all.subs(ov);*/
		all.text_subs(vmno);
		all.print(expr);
	}
	else {
		tree = wrap(Empty, tree, NEG);
		expr = "!(" + expr + ")";
		for (int mask = 0; mask < (1 << VAR_MAX); mask++) {
			bool norm = true;
			for (int supermask = 0; supermask < (1 << VAR_MAX); supermask++) {
				if ((supermask & mask) != mask)
					continue;
				norm &= tree.eval(~supermask);
			}
			int chc = popcnt(mask);
			if (norm && chc < hypcnt) {
				hyp = mask;
				hypcnt = chc;
			}
		}

		if (hyp == -1) {
			cout << ":(";
			return 0;
		}

		vector <string> all_hyps;
		for (int i = 0; i < VAR_MAX; i++) {
			if ((1 << i) & hyp) {
				all_hyps.push_back("!");
				all_hyps.back().push_back('A' + i);
			}
		}
		proof all(all_hyps, vector<string>());
		vector <Node> arr((1 << VAR_MAX));
		for (int mask = 0; mask < (1 << VAR_MAX); mask++) {
			if ((mask & hyp) != hyp)
				continue;
			vector <string> str_hyps;
			for (int i = 0; i < VAR_MAX; i++) {
				str_hyps.push_back("");
				if ((1 << i) & mask)
					str_hyps.back().push_back('!');
				str_hyps.back().push_back('A' + i);
			}
			proof cur(str_hyps, vector<string>());
			generate_proof(tree, ~mask, cur);
			for (int i = 0; i < VAR_MAX; i++) {
				if (!((1 << i) & hyp))
					cur.apply_deduction(str_hyps[i]);
			}
			arr[mask] = cur.get_result();
			all.merge(cur);
		}

		for (int i = 0; i < VAR_MAX; i++) {
			if (!((1 << i) & hyp)) {
				string cur = "";
				cur.push_back('A' + i);
				all.merge(AOrNotA(get_Node(cur)));
			}
		}

		for (int i = VAR_MAX - 1; i >= 0; i--) {
			if ((1 << i) & hyp)
				continue;
			string p = "", np = "!";
			p.push_back('A' + i);
			np.push_back('A' + i);
			for (int mask = 0; mask < (1 << VAR_MAX); mask++) {
				if ((mask & hyp) != hyp)
					continue;
				if ((1 << i) & mask)
					continue;
				Node a = *arr[mask].right;
				Node tmp = Axiom::subs({ get_Node(p), get_Node(np), a }, 8);
				proof cur(vector<string>(),
					vector<string>({
					tmp.canonical_str(),
					tmp.right->canonical_str(),
					tmp.right->right->canonical_str(),
					a.canonical_str()
				}));
				all.merge(cur);
				arr[mask] = arr[mask | (1 << i)] = a;
			}
			hyp |= (1 << i);
		}
		/*vector<Node> ov;
		for (auto it = vmno.begin(); it != vmno.end(); it++)
			ov.push_back(get_Node(it->second));
		all.subs(ov);*/
		all.text_subs(vmno);
		all.print(expr);
	}

	return 0;
}
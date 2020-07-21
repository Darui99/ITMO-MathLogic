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

template <class f, class g1>
struct S1 {
	static unsigned compute(std::vector <unsigned> x) {
		std::vector <unsigned> g;
		g.push_back(g1::compute(x));
		return f::compute(g);
	}
};

template <class f, class g1, class g2>
struct S2 {
	static unsigned compute(std::vector <unsigned> x) {
		std::vector <unsigned> g;
		g.push_back(g1::compute(x));
		g.push_back(g2::compute(x));
		return f::compute(g);
	}
};

template <class f, class g1, class g2, class g3>
struct S3 {
	static unsigned compute(std::vector <unsigned> x) {
		std::vector <unsigned> g;
		g.push_back(g1::compute(x));
		g.push_back(g2::compute(x));
		g.push_back(g3::compute(x));
		return f::compute(g);
	}
};


struct N {
	static unsigned compute(std::vector <unsigned> x) {
		assert(x.size() == 1);
		return x[0] + 1;
	}
};

struct Z {
	static unsigned compute(std::vector <unsigned> x) {
		assert(x.size() == 1);
		return 0;
	}
};

template <unsigned n, unsigned k>
struct U {
	static unsigned compute(std::vector <unsigned> x) {
		assert(x.size() == n);
		return x[k - 1];
	}
};

template <class f, class g>
struct R {
	static unsigned compute(vector <unsigned> x) {
		unsigned y = x.back();
		x.pop_back();
		if (y == 0) {
			return f::compute(x);
		}
		else {
			x.push_back(y - 1);
			x.push_back(compute(x));
			return g::compute(x);
		}
	}
};

//-----------------------------------------------------------

int num = 1;

template <class f>
void test(vector <unsigned> x, unsigned expected) {
	cout << "test " << num++ << ": ";
	unsigned actual = f::compute(x);
	if (actual != expected)
		cout << "ERROR - actual: " << actual << " | expected: " << expected << endl;
	else
		cout << "OK\n";
}

//-----------------------------------------------------------

typedef U<1, 1> ID;

typedef R < ID, S1<N, U<3, 3> > > sum;

//typedef R < Z, S2<sum, U<3, 1>, U<3, 3> > > mul;

struct mul {
	static unsigned compute(vector <unsigned> x) {
		assert(x.size() == 2);
		return x[0] * x[1];
	}
};

typedef S2 < R < Z, U<3, 2> >, ID, ID > decr;

typedef R < ID, S1<decr, U<3, 3> > > sub;

typedef S2 <sub, ID, decr> isPos;

//typedef S1 < isPos, S2 < sub, U<2, 2>, U<2, 1> > > le;

struct le {
	static unsigned compute(vector <unsigned> x) {
		assert(x.size() == 2);
		return x[0] < x[1];
	}
};

//typedef R < S1<N, Z>, S2<mul, U<3, 1>, U<3, 3> > > power;

struct power {
	static unsigned compute(vector <unsigned> x) {
		assert(x.size() == 2);
		return pow(x[0], x[1]);
	}
};

typedef S2 < R < S1<N, Z>, S2<mul, S1<N, U<3, 2>>, U<3, 3> > >, ID, ID > fact;

typedef S2 < sum, S2 <sub, U<2, 1>, U<2, 2> >, U<2, 2> > max_;

typedef S2 < sub, S2<sum, U<2, 1>, U<2, 2> >, S2<max_, U<2, 1>, U<2, 2> > > min_;

template <class f>
struct first {
	static unsigned compute(vector <unsigned> x) {
		unsigned n = x.back();
		for (unsigned i = 0; i < n; i++) {
			x.back() = i;
			if (f::compute(x) != 0)
				return i;
		}
		return n;
	}
};

typedef S3 < first< S2<le, U<3, 1>, S2<mul, U<3, 2>, S1<N, U<3, 3>> >> >, U<2, 1>, U<2, 2>, U<2, 1> > div_;

//typedef S2 < sub, U<2, 1>, S2<mul, U<2, 2>, S2<div_, U<2, 1>, U<2, 2>> > > mod_;

struct mod_ {
	static unsigned compute(vector <unsigned> x) {
		assert(x.size() == 2);
		return x[0] % x[1];
	}
};

//typedef S3 < first< S2<mod_, U<3, 1>, S2<power, U<3, 2>, S1<N, U<3, 3>> >> >, U<2, 1>, U<2, 2>, U<2, 1> > plog;

struct plog {
	static unsigned compute(vector <unsigned> x) {
		assert(x.size() == 2);
		unsigned ans = 0;
		while (x[0] % x[1] == 0) {
			x[0] /= x[1];
			ans++;
		}
		return ans;
	}
};

typedef S1<N, S1<Z, U<2, 1>>> ONE2;

typedef S2<sub, S1<N, Z>, ID> neg;

typedef S2 < first < S2<mul, S2<le, ONE2, U<2, 2>>, S1<neg, S2<mod_, U<2, 1>, U<2, 2>> > > >, ID, ID > smallestDiv;

//typedef S2 < S1<neg, S2 < sum, S2 <le, U<2, 1>, S1<N, ONE2>>, S2 <le, U<2, 1>, U<2, 2>> >>, S1<smallestDiv, ID>, ID > prime;

struct prime {
	static unsigned compute(vector <unsigned> x) {
		assert(x.size() == 1);
		if (x[0] < 2)
			return 0;
		for (unsigned i = 2; i * i <= x[0]; i++) {
			if (x[0] % i == 0)
				return 0;
		}
		return 1;
	}
};

struct kth_prime {
	static unsigned compute(vector <unsigned> x) {
		assert(x.size() == 1);
		unsigned num = 0;
		for (unsigned i = 2;; i++) {
			if (prime::compute({ i })) {
				if (++num == x[0])
					return i;
			}
		}
	}
};

//-----------------------------------------------------------------------------------------------------------------

typedef S1 <N, Z> nil;

typedef S1<N, S1<N, Z > > TWO;

typedef S2 <power, TWO, S2 <plog, U<1, 1>, TWO > > head;

typedef S2 <plog, U<2, 1>, S1<kth_prime, U<2, 2> > > kth;

typedef S2 < first < S2 <mod_, U<2, 1>, S1<kth_prime, S1<N, U<2, 2>> > > >, U<1, 1>, U<1, 1> > len;


int main() {

	test<sum>({ 97, 14 }, 111);

	test<mul>({ 11, 14 }, 154);
	test<mul>({ 0, 14 }, 0);
	test<mul>({ 14, 0 }, 0);
	test<mul>({ 10, 15 }, 150);

	test<decr>({ 10 }, 9);
	test<decr>({ 0 }, 0);
	test<decr>({ 1 }, 0);

	test<sub>({ 10, 7 }, 3);
	test<sub>({ 7, 10 }, 0);
	test<sub>({ 10, 10 }, 0);

	test<le>({ 3, 5 }, 1);
	test<le>({ 10, 7 }, 0);
	test<le>({ 9, 9 }, 0);

	test<power>({ 2, 3 }, 8);
	test<power>({ 5, 0 }, 1);
	test<power>({ 3, 3 }, 27);

	test<fact>({ 5 }, 120);
	test<fact>({ 4 }, 24);
	test<fact>({ 1 }, 1);
	test<fact>({ 0 }, 1);

	test<max_>({ 1, 2 }, 2);
	test<max_>({ 2, 1 }, 2);

	test<min_>({ 1, 2 }, 1);
	test<min_>({ 2, 1 }, 1);

	test<div_>({ 4, 2 }, 2);
	test<div_>({ 4, 1 }, 4);
	test<div_>({ 10, 3 }, 3);
	test<div_>({ 2, 4 }, 0);

	test<mod_>({ 4, 2 }, 0);
	test<mod_>({ 4, 1 }, 0);
	test<mod_>({ 10, 3 }, 1);
	test<mod_>({ 2, 4 }, 2);

	test<plog>({ 4, 2 }, 2);
	test<plog>({ 15, 3 }, 1);
	test<plog>({ 15, 6 }, 0);
	test<plog>({ 24, 2 }, 3);
	test<plog>({ 72, 6 }, 2);

	test<prime>({ 1 }, 0);
	test<prime>({ 2 }, 1);
	test<prime>({ 17 }, 1);
	test<prime>({ 25 }, 0);

	test<kth_prime>({ 1 }, 2);
	test<kth_prime>({ 3 }, 5);
	test<kth_prime>({ 7 }, 17);

	//-------------------------

	test<nil>({ 1 }, 1);
	test<nil>({ 7 }, 1);

	test<head>({ 240000 }, 128);
	test<head>({ 24 }, 8);

	test<kth>({ 240000, 1 }, 7);
	test<kth>({ 240000, 2 }, 1);
	test<kth>({ 240000, 3 }, 4);

	test<len>({ 240000 }, 3);
	test<len>({ 32 }, 1);
	test<len>({ 2310 }, 5);

	return 0;
}
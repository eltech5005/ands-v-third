// demo1STL.cpp: упражнения с ДДП/вектором указателей.
//(c)lgn, 28.04/11.05.16/27.01.17
#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <set>
#include <ctime>
#include <iterator>
#include <chrono>
#include <vector>

typedef std::multiset<int> MySet;
typedef std::multiset<int>::iterator MyIt;
typedef std::vector<MyIt> MySeq;

const int lim = 1000; //ОГРАНИЧИТЕЛЬ для множества ключей
class MyCont {
	int power;
	char tag;
	MySet A;
	MySeq sA;
	MyCont& operator = (const MyCont &);
	MyCont& operator = (MyCont &&);
public:
	MyCont ( int, char );
	MyCont (const MyCont &);
	MyCont (MyCont &&);
	MyCont& operator |= (const MyCont &);
	MyCont operator | (const MyCont & rgt) const
	 { MyCont result(*this); return (result |= rgt); }
	MyCont& operator &= (const MyCont &);
	MyCont operator & (const MyCont & rgt) const
	 { MyCont result(*this); return (result &= rgt); }
	MyCont& operator -= (const MyCont &);
	MyCont operator - (const MyCont & rgt) const
	 { MyCont result(*this); return (result -= rgt); }
	void Merge (const MyCont &);
	void Concat (const MyCont &);
	void Mul (int);
	void Erase (size_t, size_t);
	void Excl (const MyCont &);
	void Subst (const MyCont &, size_t);
	void Change (const MyCont &, size_t);
	void Show ( ) const;
	size_t Power( ) const { return sA.size( ); }
	void PrepareExcl(const MyCont& ); //Подготовка excl
friend	void PrepareAnd(MyCont &, MyCont&, const int); //подготовка and и sub
};

MyCont::MyCont( int p = 0, char t = 'R')
	: power(p), tag(t) {
	for(int i = 0; i < power; ++i) { sA.push_back(A.insert(std::rand( )%lim)); }
}

MyCont::MyCont (MyCont && source) //Конструктор копии "с переносом"
	: power(source.power), tag(source.tag),
	  A(source.A), sA(source.sA) { }

MyCont::MyCont (const MyCont & source) //Конструктор копии
	: power(source.power), tag(source.tag) {
	for (auto x : source.A) sA.push_back(A.insert(x));
}

void MyCont::Show( ) const {
	using std::cout;
	cout << "\n" << tag << ": ";
/*	unsigned n = A.bucket_count( ); //Вариант: выдача содержимого для ХТ
	for (unsigned i = 0; i < n; ++i) 
	    if (A.bucket_size(i)) 
		{
			cout << "\n" <<  i << "("<< A.bucket_size(i) << "):" ;
	//		auto it0 = A.begin(i), it1 = A.end(i);
			for (auto it = A.begin(i); it != A.end(i); ++it) cout << " " << *it; 
	} */
	for(auto x : A) cout << x << " "; //Выдача множества
	cout << "\n < ";
	for(auto x : sA) cout << *x << " "; //Выдача последовательности
	cout << ">";
}

void PrepareAnd(MyCont & first, MyCont& second, const int quantity) {
	for (int i = 0; i < quantity; ++i) { //Подготовка пересечения:
		int x = rand( )%lim;		// добавление общих эл-тов
		first.sA.push_back(first.A.insert(x));
		second.sA.push_back(second.A.insert(x));
	}
}

MyCont& MyCont::operator -= (const MyCont & rgt){ //Разность множеств
	MySet temp;
	MySeq stemp; 
//	for (auto x : A) if(find(rgt.A.begin( ), rgt.A.end( ), x) == A.end( )) stemp.insert(x);-- ОБЩИЙ find расточителен!
///	for (auto x : A) if(rgt.A.find(x) == rgt.A.end( )) temp.insert(x);
	for (auto x : A)
		if(rgt.A.find(x) == rgt.A.end( ))
			stemp.push_back(temp.insert(x));
///	for (auto x : temp) stemp.push_back(&*temp.find(x));
	temp.swap(A);
	stemp.swap(sA);
	return *this;
}

MyCont& MyCont::operator &= (const MyCont & rgt){ //Пересечение
	MySet temp;
	MySeq stemp; 
//	for (auto x : A) if(find(rgt.A.begin( ), rgt.A.end( ), x) != A.end( )) stemp.insert(x); -- ОБЩИЙ find расточителен!
///	for (auto x : A) if(rgt.A.find(x) != rgt.A.end( )) temp.insert(x);
///	for (auto x : temp) stemp.push_back(&*temp.find(x));
	for (auto x : A) if(rgt.A.find(x) != rgt.A.end( )) stemp.push_back(temp.insert(x));
	temp.swap(A);
	stemp.swap(sA);
	return *this;
}

MyCont& MyCont::operator |= (const MyCont & rgt) { //Объединение
	for (auto x : rgt.A) sA.push_back(A.insert(x));
	return *this;
}

void MyCont::Erase (size_t p, size_t q) { //Исключение фрагмента от p до q
	using std::min;
	size_t r(Power( ));
	p = min(p, r); q = min(q+1, r);
	if(p <= q) {
		MySet temp;
		MySeq stemp;
		for(size_t i = 0; i < p; ++i) 
			stemp.push_back(temp.insert(*sA[i]));
		for(size_t i = q; i < r; ++i) 
			stemp.push_back(temp.insert(*sA[i]));
 		A.swap(temp);
		sA.swap(stemp);
	}
}

void MyCont::Mul(int k) { //Размножение (не более чем в 5 раз)
	auto p = sA.begin( ), q = sA.end( ); 
	if(p != q && (k = k%5) > 1) { //Пропуск, если множество пусто или k < 2
       	std::vector<int> temp(A.begin( ), A.end( ));
	   MySeq res(sA);
	   for(int i = 0; i < k-1; ++i) {
		std::copy(p, q, back_inserter(res));
	       	A.insert(temp.begin( ), temp.end( ));
	   }
	   sA.swap(res);
	}
}

void MyCont::Merge(const MyCont & rgt) { //Слияние
	using std::sort;
	MySeq temp(rgt.sA), res;
	auto le = [ ] (MyIt a, MyIt b)->bool { return *a < *b; };//Критерий
	sort(sA.begin( ), sA.end( ), le);
	sort(temp.begin( ), temp.end( ), le);
	std::merge(sA.begin( ), sA.end( ), temp.begin( ), temp.end( ), std::back_inserter(res), le);		//Слияние для последовательностей...
	A.insert(rgt.A.begin( ), rgt.A.end( )); //... и объединение множеств
	sA.swap(res);
}

void MyCont::PrepareExcl( const MyCont& rgt ){ 
   //Подготовка объекта исключения в пустом контейнере...
	int a = rand( )%rgt.Power( ), b = rand( )%rgt.Power( ); 
   //... из случайного [a, b] отрезка rgt
	if (b>a) {
		for (int x = a; x <= b; ++x)  {
			int y =*(rgt.sA[x]); sA.push_back(A.insert(y));
		}
	}
}

void MyCont::Excl (const MyCont & rgt) { //Исключение подпоследовательности
 	size_t n(Power( )), m(rgt.Power( ));
	if(m) for (size_t p = 0; p < n; ++p) { //Поиск первого элемента цепочки
		bool f(true);
//		int a(*sA[p]), b(*rgt.sA[0]); //ОТЛАДКА
 		if(*sA[p] == *rgt.sA[0]) { //Проверка всей цепочки
			size_t q(p), r(0); 
			if (m > 1) do {
				++q, ++r; 
				size_t c(*sA[q]), d(*rgt.sA[r]);
				f &= c == d;
			} while ((r<m-1) && f); 
			if(f) {//Цепочки совпали, удаляем
				MySet temp;
				MySeq stemp;
				for(size_t i = 0; i < p; ++i) stemp.push_back(temp.insert(*sA[i]));
				for(size_t i = p+m; i < Power( ); ++i) stemp.push_back(temp.insert(*sA[i]));
 				A.swap(temp);
				sA.swap(stemp);
				break;
			}
		}
	}
}

void MyCont::Concat(const MyCont & rgt) { //Сцепление
//std::copy(rgt.sA.begin( ), rgt.sA.end( ), back_inserter(sA)); "Простой" вариант
	    for(auto x : rgt.sA) sA.push_back(A.insert(*x));
}

void MyCont::Subst (const MyCont & rgt, size_t p) { //Подстановка
	if(p >= Power( )) Concat(rgt);
	else {
		MySeq stemp(sA.begin( ), sA.begin( ) + p);	//Начало
		std::copy(rgt.sA.begin( ), rgt.sA.end( ), back_inserter(stemp)); //Вставка
		std::copy(sA.begin( )+p, sA.end( ), back_inserter(stemp)); //Окончание
		MySet temp;
		sA.clear( );
		for (auto x : stemp) sA.push_back(temp.insert(*x));
		A.swap(temp);
	}
}
void MyCont::Change (const MyCont & rgt, size_t p) { //Замена
	if(p >= Power( )) Concat(rgt);
	else { 
		MySeq stemp(sA.begin( ), sA.begin( ) + p);	//Начало
		std::copy(rgt.sA.begin( ), rgt.sA.end( ), back_inserter(stemp)); //Замена
		size_t q = p + rgt.Power( );
		if (q < Power( ))
		  std::copy(sA.begin( )+q, sA.end( ), back_inserter(stemp)); //Окончание
		MySet temp;
		sA.clear( );
		for (auto x : stemp) sA.push_back(temp.insert(*x));
		A.swap(temp);
	}
}

int _tmain( )   
{  using std::cout;
   using namespace std::chrono;
	setlocale(LC_ALL, "Russian");
    srand((unsigned int)7); //Пока здесь константа, данные повторяются
//    srand((unsigned int)time(nullptr)); //Разблокировать для случайных данных
	bool debug = true; //false, чтобы запретить отладочный вывод
	auto MaxMul = 5;
	int middle_power = 0, set_count = 0;
	auto Used = [&] (MyCont & t){ middle_power += t.Power( ); ++set_count; };
	auto DebOut = [debug] (MyCont & t) { if(debug) { t.Show( ); system("Pause");}};
	auto rand = [ ] (int d) { return std::rand( )%d; };
	int p = rand(20) + 1;
	//=== Данные ===
	MyCont A(p, 'A');
    	MyCont B(p, 'B'); 
    	MyCont C(p, 'C');
	MyCont D(p, 'D'); 	
	MyCont E(p, 'E'); 	
    	MyCont F(0, 'F');   //Пустая заготовка для Excl
    	MyCont G(p, 'G'); 
	MyCont H(p, 'H'); 	
	MyCont R(p);   
	int q_and(rand(MaxMul) + 1);  
	PrepareAnd(A, R, q_and);
	if (debug) A.Show( ); Used(A);
	if (debug) R.Show( ); Used(R);
	//=== Цепочка операций === (Операция пропускается (skipped!), если аргументы некоректны)
	//Идёт суммирование мощностей множеств и подсчёт их количества, измеряется время выполнения цепочки
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now( );
	if (debug) cout << "\n=== R&=A ===(" << q_and << ") ";
	R&=A;   DebOut(R); Used(R);

	if (debug) B.Show( ); Used(B);
	if (debug) cout << "\n=== R|=B ===";
	R|=B;  DebOut(R); Used(R);

	int e = rand(R.Power( ));
	if (debug) cout << "\n=== R.Change (H, " << e << ") ===";
	H.Show( ); Used(H);
	R.Change(H, e);   DebOut(R);	Used(R);

	int q_sub(rand(MaxMul) + 1);
	PrepareAnd(C, R, q_sub);
        if (debug) R.Show( ), C.Show( ); middle_power += q_sub; Used(C);
	if (debug) cout << "\n=== R-=C ===(" << q_sub << ") ";
	R-=C;  DebOut(R); Used(R);

	int a = rand(R.Power( )), b = rand(R.Power( ));
	if (debug) cout << "\n=== R.Erase (" << a << "," << b << ")===";
	if (a>b) cout << "(skipped!)";
	R.Erase(a, b);   DebOut(R);	Used(R);

	if (debug) cout << "\n=== R.Concat(D) ===";
	D.Show( ); Used(D);
	R.Concat(D);   DebOut(R);	Used(R);

	if (debug) cout << "\n=== R.Merge(E) ===";
	E.Show( ); Used(E);
	R.Merge(E);   DebOut(R);	Used(R);

	if (debug) cout << "\n=== R.Excl(F) ===";
	F.PrepareExcl(R); 	
	if(debug && !F.Power( )) cout << "(skipped)!";
	F.Show( );  Used(F);
	R.Excl(F);   DebOut(R);	Used(R);

	int d = rand(R.Power( ));
	if (debug) cout << "\n=== R.Subst (G, " << d << ") ===";
	G.Show( ); Used(G);
	R.Subst(G, d);   DebOut(R);	Used(R);

	int c = rand(MaxMul);
	if (debug) cout << "\n=== R.Mul(" << c << ")===";
	if (c < 2) cout << "(skipped!)";
	R.Mul(c);   DebOut(R);	Used(R);
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now( );
		auto dt = duration_cast<duration<double>>(t2-t1);
	middle_power /= set_count;
	cout << "\n=== Конец === (" << p << " : " << set_count << " * " << middle_power << " DT=" <<		(dt.count()) <<")\n";
	system("Pause");
  //  return 0;
}

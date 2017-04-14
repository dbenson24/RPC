#include <iostream>
#include <string>

using namespace std;
#include "lotsofstuff.idl"

void func1() {
    cout << "func1!";
}
void func2() {
    cout << "func2!";
};
void func3() {
    cout << "func3!";
};

int sqrt(int x, int y) {
    return x/y;
}

int takesTwoArrays(int x[24], int y[24]) {
    return x[1] + y[1];
}

int showsArraysofArrays(int x[24], int y[24][15], int z[24][15]) {
    return x[1] + y[1][1] + z[1][1];
}

string upcase(string s) {
    return s;
};
/*
struct Person {string firstname; string lastname; int age;};

struct ThreePeople {
  Person p1;
  Person p2;
  Person p3;
};
*/
Person findPerson(ThreePeople tp) {
    return tp.p2;
}

float multiply(float x, float y) {
    return x * y;
}
/*
struct StructWithArrays {
  int aNumber;
  Person people[10];
};

struct rectangle {int x; int y;};
*/

int area(rectangle r) {
    return r.x * r.y;
}

void searchRectangles(rectangle rects[200]) {
    cout << "searchRectangles!";
};
/*
struct MT {};




struct s {
  int m1[4];
  int m2[4][10];
  int m3[4][10][100];
};
*/


#include "string"

using namespace std;

#include "structs.idl"

Person findPerson(ThreePeople tp) {
    return tp.p2;
}

int area(rectangle r) {
    return r.x * r.y;
}

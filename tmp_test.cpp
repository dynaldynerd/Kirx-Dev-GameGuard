class A { public: int f(int x){return x;} static A* Instance(){return nullptr;} };
int main(){ A* a=A::Instance(); return A::f(a,1); }

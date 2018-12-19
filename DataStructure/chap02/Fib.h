class Fib
{
private:
    int f, g;
public:
    Fib(int n);
    int get();
    void prev();
    void next();
};

Fib::Fib(int n)
{
    //f = fib(k-1)
    //g = fib(k)
    f = 1; g = 0;
    while(g < n)
    {
        next();
    }
}

int Fib::get()
{
    return g;
}

void Fib::prev()
{
    f = g-f;
    g = g-f;

}

void Fib::next()
{
    g += f;
    f = g-f;
}

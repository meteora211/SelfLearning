

class NQueen{
public:
    int x, y;
    Queen(int n = 4): _size(n) {};
    Queen(int xx = 0, int yy = 0): x(xx), y(yy) {};

    solve();
private:
    int _size = 4;
};

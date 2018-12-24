#include "Stack.h"

bool paren(const char* expr)
{
    Stack<char> s;
    int i = 0;
    while(expr[i])
    {
        switch (expr[i]){
            case '(': case '[': case '{': s.push(expr[i]); break;
            case ')': 
                if (s.empty() || s.top() != '(') 
                {
                    return false;
                }
                else
                {
                    s.pop();
                    break;
                }
            case ']': 
                if (s.empty() || s.top() != '[') 
                {
                    return false;
                }
                else
                {
                    s.pop();
                    break;
                }
            case '}':
                if (s.empty() || s.top() != '{') 
                {
                    return false;
                }
                else
                {
                    s.pop();
                    break;
                }
            default:
                break;
        }
        ++i;
    }
    return s.empty();
}

int main(int args, char* argv[])
{
    const char* input1 = "[s(a) + s(b)]/{a + [(b - c)(c + d)](s1 - s2)}";
    const char* input2 = "[s(a){]/{a + [(b - c)(c + d)](s1 - s2)}}";
    paren(input1) ? printf("true\n") : printf("false\n");
    paren(input2) ? printf("true\n") : printf("false\n");
    return 0;
}

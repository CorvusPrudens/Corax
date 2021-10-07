
int main()
{

  int a;
  int b = 20;

  // inner scope
  // TODO -- this produces an unnecessary stack allocation
  {
    // second declaration of a with more locality
    b = b + 10 / 2 - 37;
    int a = b;
    a++;
  }

  // TODO -- the register originally holding b is swapped to a in the inner scope,
  // resulting in an erroneous read here
  a = b; 
  return a;
}

void func(int a, int b)
{
  a = a + b;
  // no return here
}
int global = 100;

int identifiers(int a, int b)
{
  a += 3;
  b += a + global;
  return b;
}
#define STDIN_FD  0
#define STDOUT_FD 1

int read(int __fd, const void *__buf, int __n){
    int ret_val;
  __asm__ __volatile__(
    "mv a0, %1           # file descriptor\n"
    "mv a1, %2           # buffer \n"
    "mv a2, %3           # size \n"
    "li a7, 63           # syscall write code (63) \n"
    "ecall               # invoke syscall \n"
    "mv %0, a0           # move return value to ret_val\n"
    : "=r"(ret_val)  // Output list
    : "r"(__fd), "r"(__buf), "r"(__n)    // Input list
    : "a0", "a1", "a2", "a7"
  );
  return ret_val;
}

void write(int __fd, const void *__buf, int __n)
{
  __asm__ __volatile__(
    "mv a0, %0           # file descriptor\n"
    "mv a1, %1           # buffer \n"
    "mv a2, %2           # size \n"
    "li a7, 64           # syscall write (64) \n"
    "ecall"
    :   // Output list
    :"r"(__fd), "r"(__buf), "r"(__n)    // Input list
    : "a0", "a1", "a2", "a7"
  );
}

void exit(int code)
{
  __asm__ __volatile__(
    "mv a0, %0           # return code\n"
    "li a7, 93           # syscall exit (64) \n"
    "ecall"
    :   // Output list
    :"r"(code)    // Input list
    : "a0", "a7"
  );
}

int add(int a, int b)
{
    int soma;
    int carry;

    // Repete enquanto houver carry

    while (b != 0)
    {
        // Soma os bits (desconsidera carry)

        soma = a ^ b;

        // Atualiza o carry

        carry = (a & b) << 1;

        a = soma;
        b = carry;
    }

    return a;
}

int subtracao(int a, int b){
  return add(a, add(~b, 1));
}

unsigned int multiply(unsigned int a, unsigned int b)
{
  unsigned int result = 0;
  
  for(int i = 0; i < 32; i++)
    if((b >> i) & 1){
      result = add(result, a << i);
  }
  return result;
}

unsigned int divide(unsigned int dividend, unsigned int divisor)
{ 
  unsigned int remainder;
  unsigned int quotient = remainder = 0;
  
  if(divisor == 0){
    return 0;
  }

  for(int i = 31; i >= 0; i--){
    remainder = (remainder << 1) | ((dividend >> i) & 1);
   
  if(remainder >= divisor){
    quotient = (quotient << 1) | 1;
    remainder = subtracao(remainder, divisor);
  }
  else{
    quotient = quotient << 1;
    }
  }
  return quotient;
}

unsigned int remainder(unsigned int dividend, unsigned int divisor)
{
  unsigned int remainder;
  unsigned int quotient = remainder = 0;
  
  if(divisor == 0){
    return 0;
  }
  
  for(int i = 31; i >= 0; i--){
    remainder = (remainder << 1) | ((dividend >> i) & 1);
   
  if(remainder >= divisor){
    quotient = (quotient << 1) | 1;
    remainder = subtracao(remainder, divisor);
  }
  else{
    quotient = quotient << 1;
    }
  }
  return remainder;
}

int main()
{  
  char buffer[10];

  int n = read(STDIN_FD, (void*) buffer, 10);

  unsigned int result;
  
  unsigned int a = 0;
  for(int i = 0; i < 3; i++){
    a = add(multiply(a, 10), subtracao(buffer[i], '0'));
  }
  unsigned int b = 0;
  for(int i = 6; i < 9; i++){
    b = add(multiply(b, 10), subtracao(buffer[i], '0'));
  }

  if(buffer[4] == '*'){
    result = multiply(a, b);
  }
  else if(buffer[4] == '/'){
    result = divide(a, b);
  }
  else{
    result = remainder(a, b);
  }

  buffer[7] = '\n';
  for(int i = 0; i < 7;i++){
    buffer[6 - i] = add(remainder(result, 10), '0');
    result = divide(result, 10);
  }

  write(STDOUT_FD, (void*) buffer, 8);

  return 0;
}

void _start()
{
  int ret_code = main();
  exit(ret_code);
}   
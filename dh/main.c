#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <time.h>

mpz_t prime;
mpz_t base;
mpz_t private_key_1;
mpz_t private_key_2;
mpz_t public_key;
mpz_t key;
FILE *urand;

void init_numbers(void);
void clear_numbers(void);
void init_prime(size_t sz);
void generate_random(size_t sz, unsigned char *random_bytes);
void generate_key(size_t sz, mpz_t result);

int main(void)
{
    clock_t start, end;

    init_numbers();

    urand = fopen("/dev/urandom","r");
    if(NULL == urand)
    {
        fprintf(stderr, "Failed to init randomization\n");
        exit(1);
    }

    start = clock();
    init_prime(64);
    generate_key(64, private_key_1);
    generate_key(64, private_key_2);
    mpz_powm(public_key, base, private_key_1, prime);
    mpz_powm(key, public_key, private_key_2, prime);
    end = clock();
    gmp_printf ("Prime: %Zd\nBase: %Zd\nPrivate key a: %Zd\nPrivate key b: %Zd\nPublic key A: %Zd\nSecret key: %Zd\n", prime,base,private_key_1, private_key_2, public_key, key);
    printf("Done in %.2f seconds\n", difftime(end, start)/CLOCKS_PER_SEC);
    fclose(urand);
    clear_numbers();

    return 0;
}

void generate_key(size_t sz, mpz_t result)
{
    unsigned char* random_bytes = NULL;

    if(0 == sz || NULL == result)
    {
        fprintf(stderr, "Wrong params for generate_key()\n");
        exit(1);
    }
    random_bytes = (unsigned char *)malloc(sz*sizeof(unsigned char));
    if(NULL == random_bytes)
    {
        fprintf(stderr, "Failed to allocate random bytes array for the private key\n");
        exit(1);
    }
    generate_random(sz, random_bytes);
    mpz_import(result, 1, -1, sz, -1, 0, (const void*)random_bytes);
    free(random_bytes);
}

void generate_random(size_t sz, unsigned char* random_bytes)
{
    unsigned int init_random = 0;
    size_t i = 0;

    if(0 == sz || NULL == random_bytes)
    {
        fprintf(stderr, "Wrong params for generate_random()\n");
        exit(1);
    }
    init_random ^= ((unsigned int)fgetc(urand) << 8)|fgetc(urand);
    srand(init_random);
    for(i = 0; i < sz; i++)
    {
        random_bytes[i] =  rand();
    }
}

void init_prime(size_t sz)
{
    mpz_t tmp;
    mpz_t x;
    unsigned char* random_bytes = NULL;


    if(0 == sz)
    {
        fprintf(stderr, "Wrong params for init_prime()\n");
        exit(1);
    }

    mpz_init(tmp);
    mpz_init(x);

    random_bytes = (unsigned char *)malloc(sz*sizeof(unsigned char));
    if(NULL == random_bytes)
    {
        fprintf(stderr, "Failed to allocate random bytes array for the prime number\n");
        exit(1);
    }
    while (1)
    {
        generate_random(sz, random_bytes);
        random_bytes[sz-1] |= (unsigned char)0x40;
        random_bytes[0] |= (unsigned char)1;

        mpz_import(tmp, 1, -1, sz, -1, 0, (const void*)random_bytes);
        while (0 == mpz_probab_prime_p(tmp, 10))
        {
            mpz_add_ui(tmp, tmp, 2);
        }
        mpz_mul_ui(prime, tmp, 2);
        mpz_add_ui(prime, prime, 1);
        if (0 != mpz_probab_prime_p(prime, 10))
        {
            break;
        }
    }
    free(random_bytes);

    mpz_set_ui(base, (unsigned long)2);

    mpz_clear(tmp);
    mpz_clear(x);
}



void init_numbers(void)
{
    mpz_init(key);
    mpz_init(prime);
    mpz_init(base);
    mpz_init(private_key_1);
    mpz_init(private_key_2);
    mpz_init(public_key);
}

void clear_numbers(void)
{
    mpz_clear(key);
    mpz_clear(prime);
    mpz_clear(base);
    mpz_clear(private_key_1);
    mpz_clear(private_key_2);
    mpz_init(public_key);
}

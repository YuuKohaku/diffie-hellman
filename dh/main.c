#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>

mpz_t prime;
mpz_t base;
mpz_t private_key_1;
mpz_t private_key_2;
mpz_t public_key;
FILE *urand;

void init_numbers(void);
void clear_numbers(void);
void init_prime(size_t sz);
void generate_random(size_t sz, unsigned char *random_bytes);
void generate_key(size_t sz, mpz_t result);

int main(void)
{
    mpz_t key;

    init_numbers();
    mpz_init(key);
    urand = fopen("/dev/urandom","r");
    if(NULL == urand)
    {
        fprintf(stderr, "Failed to init randomization\n");
        exit(1);
    }


    init_prime(64);
    gmp_printf ("Prime: %Zd\n", prime);
    gmp_printf ("Base: %Zd\n", base);
    generate_key(64, private_key_1);
    gmp_printf ("Private key a: %Zd\n", private_key_1);
    generate_key(64, private_key_2);
    gmp_printf ("Private key b: %Zd\n", private_key_2);
    mpz_powm(public_key, base, private_key_1, prime);
    gmp_printf ("Public key A: %Zd\n", public_key);
    mpz_powm(key, public_key, private_key_2, prime);
    gmp_printf ("Secret key: %Zd\n", key);

    fclose(urand);
    mpz_clear(key);
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

    mpz_set_ui(base, (unsigned long)3);
    while(mpz_cmp(prime, base) < 0 || mpz_cmp_ui(base, 3) < 0)
    {
        mpz_powm(x, base, tmp, prime);
        if (0 != mpz_cmp_ui(x, 1))
        {
            break;
        }

        mpz_add_ui(base, base, 1);
    }
    mpz_clear(tmp);
    mpz_clear(x);
}



void init_numbers(void)
{
    mpz_init(prime);
    mpz_init(base);
    mpz_init(private_key_1);
    mpz_init(private_key_2);
    mpz_init(public_key);
}

void clear_numbers(void)
{
    mpz_clear(prime);
    mpz_clear(base);
    mpz_clear(private_key_1);
    mpz_clear(private_key_2);
    mpz_init(public_key);
}

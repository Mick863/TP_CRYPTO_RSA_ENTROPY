#ifndef RSA_CRT
#define RSA_CRT

// API de la librairie multiprécision pour le calcul RSA.
//
// Cette librairie manipule des entiers naturels de taille arbitraire (bignum)
// Un tel entier est constitué d'une table de chiffres et du nombre de chiffres.
// La base de numération des 256. Un chiffre est donné par un octet.
// n = c_0 + c_1x256 + c_2*256^2 + ... + c_{s-1}*256^{s-1}
// Le nombre de chiffres est s et la table des chiffres est [c_0 ... c_{s-1}]
// La librairie suppose que le chiffre c_{s-1} est toujours non nuls.
// 0 est le seul entier de taille nulle.


// taille maximale des entiers
#define MAX		64
// taille des mots
#define SIZE_WORD	8

#include <inttypes.h>

// calule la somme de A=(sa,a) et B=(sb,b)
// Écrit les chiffres de la somme dans r et renvoie le nombre de chiffres
int  LLAdd(uint8_t*r,int sa,uint8_t*a,int sb,uint8_t*b);

// calule la différence de A=(sa,a) et B=(sb,b)
// Écrit les chiffres de la différence dans r et renvoie le nombre de chiffres
// Cette fonction suppose que la soustraction est possible, c'est-à-dire A >= B
int LLSub(uint8_t*r,uint8_t sa,uint8_t*a,uint8_t sb,uint8_t*b);

// Calcul le produit de A=(sa,a) par B=(sb,b)
// Écrit les chiffres dans r et renvoie le nombre de chiffres
// Le pointeur r doit impérativement être différent de a et de b
int LLMul(uint8_t*r,uint8_t sa, uint8_t*a,uint8_t sb, uint8_t*b);

// recopie O=(so,o) dans D=(sd,d), renvoie sd, égal à so
int LCopy(uint8_t*d,int so,uint8_t*o);

// compare A=(sa, a) et B=(sb,b)
// renvoie un entier négatif si A<B
//         0 si A = B
//         un entier positif si A>B
int Compare(int sa,uint8_t*a,int sb,uint8_t*b);

// Remplace physiquement A par le reste de la division de A par B
// psa entrée et sortie : adresse de sa où est écrite la taille du reste
//   a entrée et sortie : table des chiffres de A et où est écrit le reste
//   b entrée : table des chiffres de A
void Modulo(int*psa,uint8_t*a,int sb,uint8_t*b);

// Multiplication de A=(sa,a) par B=(sb,b) modulo N=(sn,n)  A <- A * B mod N
// Écrit les chiffres de la somme dans a et renvoie le nombre de chiffres
int LLMulMod(uint8_t sa, uint8_t*a, uint8_t sb, uint8_t*b,uint8_t sn, uint8_t*n);

// calcule dans r l'inverse de A=(sa, a) modulo N=(sn,n)
// renvoie sr le nombre de chiffres de R=(sr,r)
// ecrit les chiffres dans r
int ll_inv_mod(uint8_t* r, uint8_t sa, uint8_t* a, uint8_t sn, uint8_t* n);

// Élévation de X=(sx,x) à la puissance E=(se,e) modulo N=(sn,n)
// Écrit le résultat dans r et renvoie le nombre de chiffres
int LLExpMod(uint8_t*r, uint8_t sx, uint8_t*x, uint8_t se, uint8_t*e, uint8_t sn, uint8_t*n);

#endif


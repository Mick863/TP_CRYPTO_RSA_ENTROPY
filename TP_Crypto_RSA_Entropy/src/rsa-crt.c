#include <inttypes.h>
#include "rsa-crt.h"


// RSA avec réduction par division euclidienne
// Opérations en mode octet.
// Utilisation du théorème chinois des restes
// Un mot est un octet uint8_t, un mot double est un uint16_t


// multiplication courte avec deux accumulations a x b + c + carry
// le résultat intermédiaire est un mot double
// rend le poids faible et affecte le poids fort à *carry
// NB. a,b,c,carry <= 2^n - 1 donc
// a x b + c + carry < 2^2n -2.2^n + 1 + 2^n - 1 + 2^n - 1 = 2^2n - 1
// il n'y a donc pas de débordement sur des mots doubles
//////////////////////////////////////////////////////////////////

static uint8_t SMul_a_a(uint8_t a, uint8_t b, uint8_t c, uint8_t*carry)
{
	uint16_t p;
	p=(uint16_t)a*(uint16_t)b+(uint16_t)*carry+(uint16_t)c;
	*carry=p>>8;
	return p;
}

// division courte de (carry, x) par y
// algorithme binaire
// le diviseur y doit être supérieur à carry
// rend le quotient et affecte le reste à *carry
//--------------------
static uint8_t SDiv(uint8_t x,uint8_t y,uint8_t*carry)
{
	int i;		// index de boucle
	uint8_t c;	// retenue
	uint8_t r;	// reste local
	r=*carry;	// (r,x)
	for (i=8;i>0;--i)
	{	// (c7,r,x) <-- (r,x) * 2 (décalage)
		c= r & 0x80; // retenue = 1er chiffre de (r,x)
		r=(r<<1)+(x>>(8-1));
		x<<=1;
		if ( (c!=0) || (r>=y) )
		{	// test (c7,r,x) >= (y,0)
			++x;	// le chiffre du quotient est inséré à droite de x
			r-=y;
		}
	}
	*carry=r;
	return x;
}

/////////////////////////////////////////////////////////
// procédures multi-précision
// Un entier long est défini par
// - une taille sx = nombre de chiffres en base B
// - une table  x de chiffres en convention little endian
//   le poids suit les adresses croissantes.
// Les procédures supposent que le chiffre significatif de poids
// fort est non nul, c'est-à-d x[sx-1]!=0
// zéro est le seul entier de taille nulle
/////////////////////////////////////////////////////////

// copie "o" de taille"so" dans "d"
// ne gère pas le recouvrement des origines et destination
////////////////////////////////////////
int LCopy(uint8_t*d,int so,uint8_t*o)
{
	int	s;
	s=so;
	while (s--)
	{
		*d++=*o++;
	}
	return so;
}

// multiplication de deux longs
// affecte a "r" le produit de "a" de taille "sa" et de "b" de taille "sb"
// rend la taille du resultat
///////////////////////////////////////////////////////////////////////////
int LLMul(uint8_t*r,uint8_t sa, uint8_t*a,uint8_t sb, uint8_t*b)
{
	uint8_t i;
	uint8_t j;
	uint8_t carry;
	uint8_t x;
	if ( (sa==0) || (sb==0) )
	{ // si l'un des operandes est nul, le resultat l'est aussi
		return 0;
	}
	carry=0;
	// multiplication par le premier chiffre de b
	// le résultat a*b[0] est affecté au résultat
	for (i=0;i<sa;i++)
	{
		r[i]=SMul_a_a(a[i],b[0],0,&carry);
	}
	r[sa]=carry;
	// les produits par les autres chiffres de b, a*b[j]
	// sont ajoutés au résultat
	for (j=1;j<sb;++j)
	{
		r++;  // écriture décalée dans le résultat
		carry=0;
		x=b[j]; // le chiffre de b par lequel il faut multiplier a
		for (i=0;i<sa;++i)
		{
			r[i]=SMul_a_a(a[i],x,r[i],&carry);
		}
		r[sa]=carry;
	}    
	// calcul de la taille du résultat selon la dernière retenue
	if (carry) return sa+sb;
	else return sa+sb-1;
}

// élévation au carré
static int LSqr(uint8_t*r, uint8_t s, uint8_t*d)
{
	int i,j;
	uint8_t c;
	uint16_t w;
	uint32_t t;

	uint8_t*x;
	uint8_t*z;
	// premère passe, affectation
	x=d;
	z=r;
	c=*x++;
	w=(uint16_t)c*(uint16_t)c;
	*z++=w;
	t=w>>8;
	for (j=s-1;j>0;--j)
	{
		w=(uint16_t) c*(uint16_t)(*x++);
		t+=(uint32_t)w+(uint32_t)w;
		*z++=t;
		t>>=8;
	}
	*z=t;
	// autres passes, ajout
	i=s;
	while(--i)
	{
		t>>=8;
		*++z=t;
		x-=i;
		z-=i;
		c=*x++;
		w=(uint16_t)c*(uint16_t)c+(uint16_t)*z;
		*z++=w;
		t=w>>8;
		for (j=i-1;j>0;--j)
		{
			w=(uint16_t)c *(uint16_t) (*x++);
			t+=(uint32_t)w+(uint32_t)w+(uint32_t)*z;
			*z++=t;
			t>>=8;
		}
		t+=(uint32_t)*z;
		*z=t;
	}
	s+=s;
	if (t==0) s--;
	return s;
}



// rend le numero du premier 1 de x a partir de la gauche
// ne doit être appelé que si x est non nul.

#define first_one __builtin_clz
/*
static int first_one(int x)
{
	int count;
	count=7;
	while (x<0x80) { --count; x<<=1; }
	return count;
}

*/

// décalage à gauche long de "n" rangs, "n" étant inférieur à 8
static void LShl(int sx, uint8_t*x, int n)
{
	int i;
	for (i=sx-1;i!=0;--i)
	{
		x[i] = (x[i]<<n) + ( x[i-1]>>(8-n) ) ;
	}
	x[0]<<=n;
}

// décalage à droite long de "n" rangs, "n" étant inférieur à 8
static void LShr(int sx, uint8_t*x, int n)
{
	int i;
	for(i=1;i<sx;i++)
	{
		x[i-1]=(x[i-1]>>n) + ( x[i]<<(8-n) );
	}
	x[i-1]>>=n;
}


// division euclidienne d'un entier long par un entier long
// divise "a" (taille "*psa") par "b" (taille "sb")
// le reste est ecrit dans *psa (taille) et a  (chiffres)
// le quotient est ignoré
// "b" doit avoir au moins deux chiffres (taille "sb" >=2, donc non nul !)
// et "a" doit etre superieur a "b";
//
void Modulo(int*psa,uint8_t*a,int sb,uint8_t*b)
{
	int	count;   // decalage de normalisation
	int	i,k;
	int	sa;
	uint8_t	qp;
	uint8_t	qc[2];
	uint8_t	rc[2];
	uint8_t	t;
	int	sq;
	uint8_t	ah;// poids fort de a
	uint8_t	rem;
	uint8_t	carry;

	sa=*psa;
	if (sa<sb) return;

	// determiner le decalage de normalisation
	count=8-1-first_one(b[sb-1]);
	if (count>0)
	{
		// normaliser le diviseur, c'est-à-dire faire en sorte que
		// le bit de poids fort du premier chiffre de b soit 1
		LShl(sb,b,count);
		// normaliser le dividende
		ah=a[sa-1]>>(8-count);
		LShl(sa,a,count);
	}
	else ah=0;
	++sa;
	// gain d'une iteration
	if ( (ah==0) && (a[sa-2]<b[sb-1]) && (sa>3) )
	{
		ah=a[--sa-1];
	}
	sq=sa-sb; // taille du quotient
	for (k=sq;k;--k) // boucle principale
	{
		// estimation du quotient partiel
		if (ah==b[sb-1])
		{
			qp=0xff;
			rem=ah+a[sa-2];
			if (rem<ah) goto soustraire;
		}
		else
		{
			rem=ah;
			qp=SDiv(a[sa-2],b[sb-1],&rem);
		}
		// correction
		rc[0]=a[sa-3];
		rc[1]=rem;
		carry=0;
		qc[0]=SMul_a_a(b[sb-2],qp,0,&carry);
		qc[1]=carry;
		while ( (qc[1] > rc[1]) || ( (qc[1] == rc[1]) && (qc[0] > rc[0]) ) )
		{
			--qp;
			if ( qc[0]<b[sb-2] ) --qc[1];
			qc[0]-=b[sb-2];
			rc[1]+=b[sb-1];
			if (rc[1]<b[sb-1])
				break; // debordement ?
		}
		// soustraction
		if (qp)
		{
		soustraire:
			carry=0;
			for (i=0;i<sb;i++)
			{
				t=SMul_a_a(qp,b[i],0,&carry);
				if (t>a[k+i-1])
					++carry;
				a[k+i-1]-=t;
			}
			// derniere correction si nécessaire
			if (carry>ah)
			{
				qp--;
				carry=0;
				for (i=0;i<sb;i++)
				{
					t=a[k+i-1]+carry;
					if (t>=carry)
						carry=0; // sinon, elle reste à 1
					t+=b[i];
					if (t<b[i])
						carry=1; // ++carry;
					a[k+i-1]=t;
				}
			}
		}
		sa--;
		ah=a[sa-1];
	}
	while ( (sa>0) && (a[sa-1]==0) ) sa--;
	// denormalisation
	if (count>0)
	{	// du reste
		LShr(sa,a,count);
		// du dividende pour qu'il soit égal à ce qu'il était lors de l'appel
		LShr(sb,b,count);
	}
	// affectation taille du reste
	*psa=sa;
	if (sa!=0)
	{
		if (a[sa-1]==0)
			--*psa;
	}
}


// Multiplication modulo n = multiplication suivi d'une division Euclidienne
// a = a*b mod n
// Le modulo est la variable globale (sn,n)
int LLMulMod(uint8_t sa, uint8_t*a, uint8_t sb, uint8_t*b,uint8_t sn, uint8_t*n)
{
	int sp;
	uint8_t p[2*MAX]; // là où est calculé le produit
	sp=LLMul(p,sa,a,sb,b);
	Modulo(&sp,p,sn,n);
	LCopy(a,sp,p);
	return sp;
}

// Carré modulo n : carré suivi d'une réduction modulo
// a=a*a
static int LSqrMod(uint8_t*r, uint8_t sa, uint8_t*a, uint8_t sn, uint8_t*n)
{
	int sp;
	uint8_t p[2*MAX];
	sp=LSqr(p,sa,a);
	Modulo(&sp,p,sn,n);
	LCopy(r,sp,p);
	return sp;
}

// Elévation de x à la puissance e, modulo n (variable globale) résultat dans r
// rend la taille du résultat.
int LLExpMod(uint8_t*r, uint8_t sx, uint8_t*x, uint8_t se, uint8_t*e, uint8_t sn, uint8_t*n)
{
	uint8_t sr;	// taille du résultat
	uint8_t flag;	// initialisé à 0 et mis à 1 quand le résultat est différent de 1
	uint8_t t;	// chiffre courant de l'exposant
	uint8_t msk;	// masque du bit de l'exposant
	// algorithme avec règle de Horner
	flag=0;
	sr=1;
	r[0]=1; // initialisation par défaut r <-- 1
	while(se!=0)
	{ // boucle sur les bits de n du poids fort vers le poids vaible
		t=e[--se];
		for (msk=0x80;msk!=0;msk>>=1)
		{
			if (flag!=0)
			{
				sr=LSqrMod(r,sr,r,sn,n);
			}
			if ((t&msk)!=0)
			{
				sr=LLMulMod(sr,r,sx,x,sn,n);
				flag=1;	// maintenant, il faut élever au carré
			}
		}
	}
	return sr;
}


//======================================================================

int Compare(int sa,uint8_t*a,int sb,uint8_t*b)
{
	// d'abord, comparaison des tailles
	if (sa<sb) return -1;
	if (sa>sb) return  1;
	// ensuite, si les tailles sont égales,
	// comparaison des chiffres depuis le poids fort
	while (sa!=0)  // si "sa" est nul, les deux le sont
	{	// "sa" sert d'index de boucle
		--sa;
		if (a[sa]<b[sa]) return -1; // pas en temps constant !
		if (a[sa]>b[sa]) return 1;
	}
	return 0;
}

// décalage à droite de 1 rang
int LShr1(int sx, uint8_t* x)
{
	int i;
	for(i=1;i<sx;i++)
	{
		x[i-1]=(x[i-1]>>1) + ( x[i]<<(8-1) );
	}
	x[i-1]>>=1;
	if (x[i-1]==0) return sx-1;
	return sx;
}

// addition longue
// affecte a "r" la somme de "a" de taille "sa" et de "b" de taille "sb"
// rend la taille du resultat
int  LLAdd(uint8_t*r,int sa,uint8_t*a,int sb,uint8_t*b)
{
	uint8_t	t;	// pour les resultats intermediaires
	int	i;	// index de boucle
	uint8_t	c;	// retenue locale
	if (sa<=sb)
	{	// b a plus de chiffre que a
		c=0;
		for(i=0;i<sa;i++)
		{	// "t" <- "*a" + "*b" + retenue
			t=a[i]+c; c=((t<a[i])?1:0);
			t+=b[i];  if (t<b[i]) ++c;
			r[i]=t;
		}
		for(;i<sb;i++)
		{
			r[i]=b[i]+c; c=(r[i]<c)?1:0;
		}
		if (c) { r[i]=1; return sb+1; }
		return sb;
	}
	else
	{	// a a plus de chiffres que b
		c=0;
		for(i=0;i<sb;i++)
		{
			t=a[i]+c; c=(t<a[i])?1:0;
			t+=b[i];  if (t<b[i]) ++c;
			r[i]=t;
		}
		for(;i<sa;i++)
		{
			r[i]=a[i]+c; c=(r[i]<c)?1:0;
		}
		if (c) { r[i]=1; return sa+1; }
		return sa;
	}
}


// Soustraction longue "r" <-- "a" taille "sa" - "b" taille "sb"
// rend la taille de "r"
// "a" est assume superieur a "b"
int LLSub(uint8_t*r,uint8_t sa,uint8_t*a,uint8_t sb,uint8_t*b)
{
	int	i;	// index de boucle
	uint8_t	t;
	uint8_t	c;
	c=0;
	for(i=0;i<sb;i++)
	{
		t=a[i]-c;
		c=((t>a[i])?1:0)+((t<b[i])?1:0);
		r[i]=t-b[i];
	}
	if (i<sa)
	{
		while (1)
		{ // dernier mot ecrit si necessaire seulement
			t=a[i]-c;
			if (i<sa-1)
			{
				c=(t>a[i])?1:0;
				r[i]=t;
			}
			else	// i==sa-1
			{
				if (t) { r[i]=t; return sa; } else { --i; break; }
			}
			++i;
		}
	}
	else --i;
	// ajustement de la taille -- ici, on a i==sa-1
	while ( (i>=0) && (r[i]==0) ) --i;
	return i+1;
}


// inverse modulo un nombre impair
// Algorithme d'Euclide binaire
// inverse de p modulo q
// Hypothèse : q est impair !
// invariant de boucle
// (1)   a x p == b ( modulo q)
// (2)   c x p == d (modulo q)
// initialisation
// a=0, b=q
// c=1, d=p
// itérations
// rendre b et d impairs en divisant par 2 les membres des équations
// (1) et (2)
// la division de b et d par 2 sont un décalage car ne survient
// que si b et d sont pairs
// les divisions de a et c sont "modulo q", c-à-d si a (resp. c) est 
// impair, ajouter q
// ensuite, remplacer une équation par leur différence

static int LLInvModOdd(uint8_t*r, uint8_t sp, uint8_t*p, uint8_t sq, uint8_t* q)
{
	int	sa;
	uint8_t	a[MAX];
	int	sb;
	uint8_t	b[MAX];
	int	sc;
	uint8_t	c[MAX];
	int	sd;
	uint8_t	d[MAX];
	sa=0;
	sb=sq; LCopy(b,sq,q);
	sc=1; c[0]=1;
	sd=sp; LCopy(d,sp,p);
	for (;;)
	{

		while ((b[0]&1)==0)
		{
			sb=LShr1(sb,b);
			if ((a[0]&1)!=0) sa=LLAdd(a,sa,a,sq,q);
			sa=LShr1(sa,a);
		}
		while ((d[0]&1)==0)
		{
			sd=LShr1(sd,d);
			if ((c[0]&1)!=0) sc=LLAdd(c,sc,c,sq,q);
			sc=LShr1(sc,c);
		}
		if ((sb==1)&&(b[0]==1))
		{
			LCopy(r,sa,a);
			return sa;
		}
		if ((sd==1)&&(d[0]==1))
		{
			LCopy(r,sc,c);
			return sc;
		}
		switch (Compare(sb,b,sd,d))
		{
		case +1:
			sb=LLSub(b,sb,b,sd,d);
			if (Compare(sa,a,sc,c)==-1) sa=LLAdd(a,sa,a,sq,q);
			sa=LLSub(a,sa,a,sc,c);
			break;
		case -1:
			sd=LLSub(d,sd,d,sb,b);
			if (Compare(sc,c,sa,a)==-1) sc=LLAdd(c,sc,c,sq,q);
			sc=LLSub(c,sc,c,sa,a);
			break;
		default:
			return 0; // cas d'erreur : pgcd(p,q) n'est pas 1
		}
	}
} 

// calcul de l'inverse de x modulo 2^n
// x doit être impair
static uint8_t invmod2n(uint8_t a, int n)
{
	uint8_t m;
	uint8_t b;
	uint8_t c;	// partie haute de b * x
	int i;

	m=2; // masque pour parcourir les chiffres binaires de x
	b=1;
	c=a>>1;	// c est la partie haute du produit de a par b
		// la partie basse est 0...01
	for (i=1;i<n;i++,m<<=1)
	{
		if ((c&1)!=0)	// partie haute impaire, c-à-d ne convient pas au rang suivant
		{
			b|=m;	// changer le chiffre de b
			c+=a;	// corriger le produit
		}
		c>>=1;		// nouvelle partie haute
	}

	return b;
}


// rend le numéro du dernier 1 de x
// -1 si x est nul
// 0 si x est impair
// 1 si x est pair sans être multiple de 4, etc.

int last_one(uint8_t x)
{
	if (x==0) return -1;
	return __builtin_ctz(x);
	/*
	int e;
	if (x==0) return -1;
	e=0;
	while ((x&1)==0)
	{
		e++;
		x>>=1;
	}
	return e;
	*/
}


// idem pour un long
int l_last_one(uint8_t sx, uint8_t* x)
{
	int e;
	
	if (sx==0)
		return -1;
	e=0;
	while (x[e]==0)
	{
		e++;
	}
	return e*SIZE_WORD + last_one(x[e]);
}

// inverse de a modulo n où n n'est pas forcément impair :
// écrire n = p * 2^e avec p impair
// inverser modulo p et modulo 2^e, puis reconstruire avec la formule de gardner
// ne marche que si la valuation dyadique est < 8 (mode octet)
int ll_inv_mod(uint8_t* r, uint8_t sa, uint8_t* a, uint8_t sn, uint8_t* n)
{
	int e;		// valuation dyadique de n
	uint8_t dpe;	// 2^e /!\ doit tenir sur un octet
	uint8_t ie;	// inverse modulo dpe
	uint8_t u;
	uint8_t sr;
	uint8_t sx;
	uint8_t x[MAX];
	uint8_t b;
	uint8_t me;

	e=l_last_one(sn,n);
	if (e==0)
	{
		// si n est impair, algo binaire
		return LLInvModOdd(r,sa,a,sn,n);
	}
	LShr(sn,n,e);	// 2^e * n <-- n
	dpe=1<<e;
	me=dpe-1;
	ie=invmod2n(a[0],e);
	sr=LLInvModOdd(r,sa,a,sn,n);
	u=invmod2n(n[0],e);
	b=r[0]&me;
	if (b>ie)
	{
		ie+=dpe;
	}
	u=((ie-b)*u)&me;
	sx=LLMul(x,sn,n,1,&u);
	sr=LLAdd(r,sx,x,sr,r);
	// restauration de n
	LShl(sn,n,e);	// n <- 2^e * n
	
	return sr;
}



#include <io.h>
#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "rsa-crt.h"

//------------------------------------------------------
// Carte RSA 
//
// Commandes
// 82 00 00 00 04 -> "1.00"	version du programme*
// /!\ Introduire les cl�s dans cet ordre.
// 82 01 00 00 nn <- e0 ... en	intro exposant public
// 82 02 00 00 nn <- p1 ... pn  intro facteur p
// 82 03 00 00 nn <- q1 ... qn  intro facteur q et pr�calcul
// 82 04 00 00 nn -> n1 ... nn  sortie modulo public 
// 82 10 00 00 nn <- m1 ... mn	chiffrement
// 82 11 00 00 nn <- c1 ... cn	d�chiffrement
// 82 c0 00 00 nn -> d1 ... dn	get response
//
//------------------------------------------------------


// d�claration des fonctions d'entr�e/sortie d�finies dans "io.c"
void sendbytet0(uint8_t b);
uint8_t recbytet0(void);

// variables globales en static ram
uint8_t cla, ins, p1, p2, p3;	// ent�te de commande
uint8_t sw1, sw2;		// status word


// Proc�dure qui renvoie l'ATR
void atr(uint8_t n, char* hist)
{
    	sendbytet0(0x3b);	// d�finition du protocole
    	sendbytet0(n);		// nombre d'octets d'historique
    	while(n--)		// Boucle d'envoi des octets d'historique
    	{
        	sendbytet0(*hist++);
    	}
}


// �mission de la version
// t est la taille de la cha�ne sv
void version(int t, char* sv)
{
    	int i;
    	// v�rification de la taille
    	if (p3!=t)
    	{
        	sw1=0x6c;	// taille incorrecte
        	sw2=t;		// taille attendue
        	return;
    	}
	sendbytet0(ins);	// acquittement
	// �mission des donn�es
	for(i=0;i<p3;i++)
    	{
        	sendbytet0(sv[i]);
    	}
    	sw1=0x90;
}

// d�finir rsa-crt comme un module linkable ne fonctionne pas
// d� � un bug de gcc.
#include "rsa-crt.c"



// dimension
// /!\ Il semble y avoir un probl�me d'alignement dans l'eeprom
// Comme la taille (1 octet) pr�c�de la table, il semble
// n�cessaire que la taille de la table soit impaire.

// Taille des facteurs
#define TAILLE_PQ	33
// Taille du modulo
#define TAILLE_N	65	
// Taille de l'exposant public
#define TAILLE_E	4

uint8_t ee_e[TAILLE_E] EEMEM={0};	// exposant public
uint8_t	ee_se EEMEM=0;
uint8_t ee_p[TAILLE_PQ] EEMEM={0};	// premier facteur
uint8_t	ee_sp EEMEM=0;
uint8_t ee_q[TAILLE_PQ] EEMEM={0};	// deuxi�me facteur
uint8_t	ee_sq EEMEM=0;
uint8_t ee_n[TAILLE_N] EEMEM;	// modulo public
uint8_t	ee_sn EEMEM=0;
uint8_t ee_dp[TAILLE_PQ] EEMEM; // exposant priv� pour P
uint8_t ee_sdp EEMEM=0;
uint8_t ee_dq[TAILLE_PQ] EEMEM; // exposant priv� pour q
uint8_t ee_sdq EEMEM=0;
uint8_t ee_u[TAILLE_PQ] EEMEM;	// inverse de p modulo q
uint8_t ee_su EEMEM=0;


void intro_cle(uint8_t* dest, uint8_t*ps, int smax)
{
	int	i;
	uint8_t x[TAILLE_N];
	
	if ((p3>smax)||(p3==0))
	{
		sw1=0x6c;
		sw2=smax;
		return;
	}
	sendbytet0(ins);
	for (i=p3-1;i>=0;i--)
	{
		x[i]=recbytet0();
	}
	eeprom_write_block(x,dest,p3);
	eeprom_write_byte(ps,p3);
	sw1=0x90;
}

// effectue des pr�calculs lorsque les facteurs sont introduits
void precalculs()
{

	int	se;
	int	sp;
	int	sq;
	int	sn;
	uint8_t e[TAILLE_E];
	uint8_t n[TAILLE_N];
	uint8_t p[TAILLE_PQ];
	uint8_t q[TAILLE_PQ];
	int	sx;
	uint8_t x[TAILLE_PQ];


	se=eeprom_read_byte(&ee_se);
	sp=eeprom_read_byte(&ee_sp);
	sq=eeprom_read_byte(&ee_sq);
	if ((se==0)||(sp==0)||(sq==0))
	{
		return;
	}
	// lecture des valeurs de e, p et q dans l'eeprom
	eeprom_read_block(e,ee_e,se);
	eeprom_read_block(p,ee_p,sp);
	eeprom_read_block(q,ee_q,sq);
	// �changer p et q si p>q
	if (Compare(sp,p,sq,q)>=0)
	{
		eeprom_write_byte(&ee_sp,sq);
		eeprom_write_block(q,&ee_p,sq);
		eeprom_write_byte(&ee_sq,sp);
		eeprom_write_block(p,&ee_q,sp);
		sx=LCopy(x,sp,p);
		sp=LCopy(p,sq,q);
		sq=LCopy(q,sx,x);
		sw2++;
	}
	// calcul modulo public
	sn=LLMul(n,sp,p,sq,q);
	eeprom_write_byte(&ee_sn,sn);
	eeprom_write_block(n,ee_n,sn);
	// calcul inverse de p modulo q
	sx=ll_inv_mod(x,sp,p,sq,q);
	eeprom_write_byte(&ee_su,sx);
	eeprom_write_block(x,ee_u,sx);
	// calcul dp
	p[0]--;
	sx=ll_inv_mod(x,se,e,sp,p);
	p[0]++;
	eeprom_write_byte(&ee_sdp,sx);
	eeprom_write_block(x,ee_dp,sx);
	// calcul dq
	q[0]--;
	sx=ll_inv_mod(x,se,e,sq,q);
	q[0]++;
	eeprom_write_byte(&ee_sdq,sx);
	eeprom_write_block(x,ee_dq,sx);
}


void lire_cle(uint8_t*k, uint8_t*sk)
{
	int	sn;
	int	i;
	sn=eeprom_read_byte(sk);
	if ((sn==0)&& (p3!=1))
	{
		sw1=0x6c;
		sw2=1;
		return;
	}
	if ((sn!=0) && (p3!=sn))
	{
		sw1=0x6c;
		sw2=sn;
		return;
	}
	sendbytet0(ins);
	for (i=p3-1;i>=0;i--)
	{
		sendbytet0(eeprom_read_byte(k+i));
	}
	sw1=0x90;
}


int srep;
uint8_t response[TAILLE_N];

void chiffre()
{
	int	i;
	uint8_t x[TAILLE_N];
	uint8_t n[TAILLE_N];
	int	sn;
	int	se;
	uint8_t e[TAILLE_E];

	sn=eeprom_read_byte(&ee_sn);
	if (sn==0) // pas de cl� publique pr�sente
	{
		sw1=0x62;
		sw2=0;
		return;
	}
	if (p3>TAILLE_N)
	{	// message trop long
		sw1=0x6c;
		sw2=TAILLE_N;
		return;
	}
	sendbytet0(ins);
	for (i=p3-1;i>=0;i--)
	{	// lecture au format big endian
		x[i]=recbytet0();
	}
	eeprom_read_block(n,ee_n,sn);	// lecture cl� publique
	se=eeprom_read_byte(&ee_se);
	eeprom_read_block(e,ee_e,se);
	srep=LLExpMod(response,p3,x,se,e,sn,n);	// chiffrement rsa
	sw1=0x90;

}

void get_response()
{
	int	sr;
	int	i;
	sr=srep;
	if (sr==0)
	{
		sr=1;
		response[0]=0;
	}
	if (p3!=sr)
	{
		sw1=0x6c;
		sw2=sr;
		return;
	}
	sendbytet0(ins);
	for(i=0;i<sr;i++)
	{
		sendbytet0(response[sr-i-1]);
	}
	sw1=0x90;
}

///////////////////////////////////////////////////////////////////
// Avec l'horloge interne � 8 MHz (t = 125 nS), le compteur en mode
// CK/1024 s'incr�mente toutes les 128 uS.
// Un tour des 256 valeurs du compteur fait 32,768 ms.
//  15 x 32.768 ms = 491,52 ms
//  16 x 32,768 ms = 524,288 ms 

// nb tours de 32,768 ms avant d'envoyer le 0x60
#define	facteur_32ms	16
// valeur haute du compteur 0, incr�ment� sur interruption
// de d�bordement de CNT0
uint8_t count0_hi;

// fonction d'interruption sur d�bordement de CNT0
ISR(TIMER0_OVF_vect)
{
	count0_hi--;
	if (count0_hi==0)
	{	
		sendbytet0(0x60);
		count0_hi=facteur_32ms;
	}	
}

void demarre_60()
{
	count0_hi=facteur_32ms;
	TCCR0A=0;	
	TIMSK0=1;	// valide l'interruption sur overflow du compteur 0
	TCNT0=0;	// Initialise la valeur du compteur
	TCCR0B=5;	// lance le compteur CNT0 avec pr�division par 1024
	sei();		// l�ve le drapeau d'interruptions
}

void fin_60()
{
	cli();		// abaisse le drapeau d'interruptions
	TIMSK0=0;	// invalide l'interruption sur overflow de CNT0
	TCCR0B=0;	// arr�te le compteur 0
}


//////////////////////////////////////////

void dechiffre()
{
	int	i;
	int	sp;
	uint8_t p[TAILLE_PQ];
	int	sq;
	uint8_t q[TAILLE_PQ];
	int	sx;
	uint8_t x[TAILLE_N];
	int	sdp;
	uint8_t dp[TAILLE_PQ];
	int	sdq;
	uint8_t dq[TAILLE_PQ];
	int	smp;
	uint8_t mp[TAILLE_PQ];
	int	smq;
	uint8_t mq[TAILLE_PQ];



	if (p3>TAILLE_N)
	{
		sw1=0x6c;
		sw2=TAILLE_N;
		return;
	}
	sendbytet0(ins);
	// r�ception du cryptogramme dans response
	srep=p3;
	for (i=srep-1;i>=0;i--)
	{
		response[i]=recbytet0();
	}
	demarre_60();
	// chargement p et q
	sp=eeprom_read_byte(&ee_sp);
	eeprom_read_block(p,ee_p,sp);
	sq=eeprom_read_byte(&ee_sq);
	eeprom_read_block(q,ee_q,sq);
	// chargement dp et dq
	sdp=eeprom_read_byte(&ee_sdp);
	eeprom_read_block(dp,ee_dp,sdp);
	sdq=eeprom_read_byte(&ee_sdq);
	eeprom_read_block(dq,ee_dq,sdq);
	// cryptogramme modulo p
	sx=LCopy(x,srep,response);
	Modulo(&sx,x,sp,p);	
	// message modulo p
	smp=LLExpMod(mp,sx,x,sdp,dp,sp,p);
	// cryptogramme modulo q
	Modulo(&srep,response,sq,q);
	// message modulo q
	smq=LLExpMod(mq,srep,response,sdq,dq,sq,q);

	// reconstitution
	// y <- mq - mp (mod q)
	if (Compare(smq,mq,smp,mp)<=0)
	{
		smq=LLAdd(mq,smq,mq,sq,q);
	}	
	srep=LLSub(response,smq,mq,smp,mp);

	// chargement de u
	sx=eeprom_read_byte(&ee_su);
	eeprom_read_block(x,ee_u,sx);
	// m <- mp + u x q
	srep=LLMulMod(srep,response,sx,x,sq,q);
	sx=LLMul(x,srep,response,sp,p);
	srep=LLAdd(response,sx,x,smp,mp);
	fin_60();
	sw1=0x90;
}

// Programme principal
//--------------------
int main(void)
{
  	// initialisation des ports
	ACSR=0x80;
	PORTB=0xff;
	DDRB=0xff;
	DDRC=0xff;
	DDRD=0;
	PORTC=0xff;
	PORTD=0xff;
	ASSR=(1<<EXCLK)+(1<<AS2);
	PRR=0x87;


	// ATR
  	atr(5,"rsa !");

	sw2=0;		// pour �viter de le r�p�ter dans toutes les commandes
  	// boucle de traitement des commandes
  	for(;;)
  	{
    		// lecture de l'ent�te
    		cla=recbytet0();
    		ins=recbytet0();
    		p1=recbytet0();
	    	p2=recbytet0();
    		p3=recbytet0();
	    	sw2=0;
		switch (cla)
		{
	  	case 0x82:
		    	switch(ins)
			{
			case 0:
				version(4,"1.00");
				break;
			case 1:
				intro_cle(ee_e,&ee_se,TAILLE_E);
				//precalculs();
				break;
			case 2:
				intro_cle(ee_p,&ee_sp,TAILLE_PQ);
				//precalculs();
				break;
			case 3:
				intro_cle(ee_q,&ee_sq,TAILLE_PQ);
				precalculs();
				break;
			case 4:
				lire_cle(ee_e,&ee_se);
				break;
			case 5:
				lire_cle(ee_p,&ee_sp);
				break;
			case 6:
				lire_cle(ee_q,&ee_sq);
				break;
			case 7:
				lire_cle(ee_n,&ee_sn);
				break;
			case 16:
				chiffre();
				break;
			case 17:
				dechiffre();
				break;
			case 0xc0:
				get_response();
				break;
            		default:
		    		sw1=0x6d; // code erreur ins inconnu
        		}
			break;
      		default:
        		sw1=0x6e; // code erreur classe inconnue
		}
		sendbytet0(sw1); // envoi du status word
		sendbytet0(sw2);
  	}
  	return 0;
}


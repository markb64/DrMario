#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "drmario.h"

/*
 * Biesso Marco
 * N° matricola 870940
 */

/*FUNZIONI AUSILIARIE*/

/*Semplice funzione che calcola le potenze di interi*/
int potenza(int b, int e){
	int res=1;
	while(e>0){
		res=res*b;
		e=e-1;
	}
	return res;
}

/*Calcola il punteggio generato dall'eliminazione di n virus moltiplicato per la combo*/
int calcola_punteggio(int n, int combo){
	int i=1,punteggio=0;
	while(i<=n){
		punteggio=punteggio+(100*potenza(2,i));
		i=i+1;
	}
	return punteggio*combo;
}

/*Conta quanti virus sono nel campo in questo momento*/
int conta_virus(struct gioco *gioco){
	int i,j,n_virus;
	n_virus=0;
	for(i=0;i<RIGHE;i++){
		for(j=0;j<COLONNE;j++){
			if(gioco->campo[i][j].tipo == MOSTRO)
				n_virus=n_virus+1;
		}
	}
	return n_virus;
}

/*Prepara il campo vuoto per la partita*/
void inizializza_campo(struct gioco *gioco){
	int i,j;
	for(i=0;i<RIGHE;i++){
		for(j=0;j<COLONNE;j++){
			gioco->campo[i][j].id=-1;
			gioco->campo[i][j].tipo=VUOTO;
		}
	}
}

/*Controlla se ci sono pastiglie (sia intere che pezzi) che devono cadere nelle celle sottostanti
  Ritorna 1 se è caduta almeno una pastiglia, 0 altrimenti*/
int controlla_pastiglie_sospese(struct gioco *gioco){
	int i,j,esci;
	esci=0;
	/*Scorre l'intera matrice a ritroso cominciando dalla penultima riga*/
	for(i=RIGHE-2;i>=0;i--){
		for(j=COLONNE-1;j>=0;j--){
			/*Se la cella è un pezzo di pastiglia con una cella vuota sotto...*/
			if(gioco->campo[i][j].tipo == PASTIGLIA && gioco->campo[i+1][j].tipo == VUOTO){
			/*Sotto-casi in cui cade:*/
				/*Pastiglia intera orizzontale (controlla che sia vuoto anche sotto l'altro pezzo)*/
				if(j>0 && gioco->campo[i][j].id == gioco->campo[i][j-1].id && gioco->campo[i+1][j-1].tipo == VUOTO){
					gioco->campo[i+1][j]=gioco->campo[i][j];
					gioco->campo[i+1][j-1]=gioco->campo[i][j-1];
					gioco->campo[i][j].tipo=VUOTO;
					gioco->campo[i][j-1].tipo=VUOTO;
					gioco->campo[i][j].id=-1;
					gioco->campo[i][j-1].id=-1;
					esci=1;
				}
				/*Pastiglia intera verticale*/
				else if(i>0 && gioco->campo[i][j].id == gioco->campo[i-1][j].id){
					gioco->campo[i+1][j]=gioco->campo[i][j];
					gioco->campo[i][j]=gioco->campo[i-1][j];
					gioco->campo[i-1][j].tipo=VUOTO;
					gioco->campo[i-1][j].id=-1;
					esci=1;
				}
				/*Pezzo di pastiglia in mezzo al campo*/
				else if(j>0 && j<(COLONNE-1) && gioco->campo[i][j-1].id != gioco->campo[i][j].id && gioco->campo[i][j+1].id != gioco->campo[i][j].id){
					gioco->campo[i+1][j]=gioco->campo[i][j];
					gioco->campo[i][j].tipo=VUOTO;
					gioco->campo[i][j].id=-1;
					esci=1;
				}
				/*Pezzo di pastiglia lungo la parete destra*/
				else if(j==(COLONNE-1) && i>0 && gioco->campo[i-1][j].id != gioco->campo[i][j].id && gioco->campo[i][j-1].id != gioco->campo[i][j].id){
					gioco->campo[i+1][j]=gioco->campo[i][j];
					gioco->campo[i][j].tipo=VUOTO;
					gioco->campo[i][j].id=-1;
					esci=1;
				}
				/*Pezzo di pastiglia lungo la parete sinistra*/
				else if(j==0 && i>0 && gioco->campo[i-1][j].id != gioco->campo[i][j].id && gioco->campo[i][j+1].id != gioco->campo[i][j].id){
					gioco->campo[i+1][j]=gioco->campo[i][j];
					gioco->campo[i][j].tipo=VUOTO;
					gioco->campo[i][j].id=-1;
					esci=1;
				}
				/*Pezzo di pastiglia nell'angolo in alto a destra*/
				else if(i==0 && j==(COLONNE-1) && gioco->campo[i][j-1].id != gioco->campo[i][j].id){
					gioco->campo[i+1][j]=gioco->campo[i][j];
					gioco->campo[i][j].tipo=VUOTO;
					gioco->campo[i][j].id=-1;
					esci=1;
				}
				/*Pezzo di pastiglia nell'angolo in alto a sinistra*/
				else if(i==0 && j==0 && gioco->campo[i][j+1].id != gioco->campo[i][j].id){
					gioco->campo[i+1][j]=gioco->campo[i][j];
					gioco->campo[i][j].tipo=VUOTO;
					gioco->campo[i][j].id=-1;
					esci=1;
				}
			}
		}
	}
	return esci;
}

/*Controlla se la pastiglia attiva (dev'essere intera per forza) in questione può cadere, se sì ritorna 1, altrimenti 0*/
int caduta_pastiglia(struct gioco *gioco, int i, int j){
	/*Caso 1: pastiglia verticale con vuoto sotto*/
	if(i<(RIGHE-2) && gioco->campo[i+1][j].id == gioco->campo[i][j].id && gioco->campo[i+2][j].tipo == VUOTO){
		gioco->campo[i+2][j]=gioco->campo[i+1][j];
		gioco->campo[i+1][j]=gioco->campo[i][j];
		gioco->campo[i][j].tipo=VUOTO;
		gioco->campo[i][j].id=-1;
		return 1;
	}
	/*Caso 2: pastiglia orizzontale con vuoto sotto*/
	else if(i<(RIGHE-1) && j<(COLONNE-1) && gioco->campo[i][j+1].id == gioco->campo[i][j].id && gioco->campo[i+1][j].tipo == VUOTO && gioco->campo[i+1][j+1].tipo == VUOTO){
		gioco->campo[i+1][j]=gioco->campo[i][j];
		gioco->campo[i+1][j+1]=gioco->campo[i][j+1];
		gioco->campo[i][j].tipo=VUOTO;
		gioco->campo[i][j+1].tipo=VUOTO;
		gioco->campo[i][j].id=-1;
		gioco->campo[i][j+1].id=-1;
		return 1;
	}
	else
		return 0;
}

/*Controlla occorrenze di almeno 4 tra pastiglie e virus ed eventualmente le rimuove, aggiornando inoltre il punteggio generato
  Ritorna 1 se è stata eliminata almeno 1 combo, 0 se non ci sono state combo da eliminare*/
int rimuovi_combo(struct gioco *gioco, int i, int j){
	int i2=0,j2=0,pastiglia=0,virus=0,combo_vert=0,combo_oriz=0;
	/*Inizia a scorrere in verticale a partire dalla cella data e incrementa il contatore per ogni virus o pastiglia consecutivi
	  dello stesso colore della prima cella, controllando che in mezzo ci sia almeno una pastiglia*/
	while((gioco->campo[i+i2][j].tipo == PASTIGLIA || gioco->campo[i+i2][j].tipo == MOSTRO) && gioco->campo[i+i2][j].colore == gioco->campo[i][j].colore){
		if(gioco->campo[i+i2][j].tipo == PASTIGLIA)
			pastiglia=1;
		/*Conta quanti virus ci sono per calcolare il punteggio*/
		if(gioco->campo[i+i2][j].tipo == MOSTRO)
			virus=virus+1;
		i2=i2+1;
	}
	/*Se il conteggio è di almeno 4 e contiene almeno una pastiglia, si possono "eliminare" quelle celle ed uscire*/
	if(i2>3 && pastiglia){
		while(i2>0){
			i2=i2-1;
			/*Le celle vengono "segnate" ma non eliminate, in modo da verificare in seguito eventuali combo incrociate*/
			gioco->campo[i+i2][j].id=-2;
		}
		combo_vert=1;
		gioco->punti=gioco->punti+calcola_punteggio(virus,gioco->combo);
	}
	pastiglia=0;
	virus=0;
	/*Stesso procedimento di sopra, ma controllando in orizzontale*/
	while((gioco->campo[i][j+j2].tipo == PASTIGLIA || gioco->campo[i][j+j2].tipo == MOSTRO) && gioco->campo[i][j+j2].colore == gioco->campo[i][j].colore){
		if(gioco->campo[i][j+j2].tipo == PASTIGLIA)
			pastiglia=1;
		if(gioco->campo[i][j+j2].tipo == MOSTRO)
			virus=virus+1;
		j2=j2+1;
	}
	if(j2>3 && pastiglia){
		while(j2>0){
			j2=j2-1;
			gioco->campo[i][j+j2].id=-2;
		}
		combo_oriz=1;
		gioco->punti=gioco->punti+calcola_punteggio(virus,gioco->combo);
	}
	return (combo_vert || combo_oriz);
}

/*FUNZIONI PRINCIPALI*/

void carica_campo(struct gioco *gioco, char *percorso) {
	int i,j,k;
	char c[200];
    	FILE *fptr;
    	/*Apre il file nel percorso dato in modalità lettura*/
	fptr=fopen(percorso,"r");
	/*Controlla se il file esiste*/
    	if(fptr == NULL){
		printf("Errore durante la lettura del file");
		exit(1);
    	}
	else{
		inizializza_campo(gioco);
	   	/*Copia il testo carattere per carattere nell'array c*/
	  	fscanf(fptr,"%[^eof]",c);
		i=0;
		j=0;
		/*Finchè il valore è diverso dal carattere di fine stringa va avanti e controlla ogni carattere*/
	   	for(k=0;c[k]!='\0';k++){
			/*Se c'è uno spazio, la cella rimane vuota*/
			if(c[k] == ' ')
				j=j+1;
			/*Se c'è il carattere a capo, inizia una nuova riga*/
			else if(c[k] == '\n'){
				i=i+1;
				j=0;
			}
			/*Se c'è una lettera, crea un mostro del colore corrispondente nella cella*/
			else if(c[k] == 'R'){
				gioco->campo[i][j].tipo=MOSTRO;
			   	gioco->campo[i][j].colore=ROSSO;
				j=j+1;
			}
			else if(c[k] == 'G'){
				gioco->campo[i][j].tipo=MOSTRO;
			   	gioco->campo[i][j].colore=GIALLO;
				j=j+1;
			}
			else if(c[k] == 'B'){
				gioco->campo[i][j].tipo=MOSTRO;
			   	gioco->campo[i][j].colore=BLU;
				j=j+1;
			}
	   	}
	}
	/*Chiude il file*/
	fclose(fptr);
}

void riempi_campo(struct gioco *gioco, int difficolta) {
	int i,j,n_virus;
	n_virus=4*(difficolta+1);
	inizializza_campo(gioco);
	srand(time(NULL));
	/*Riempie tutto il campo con mostri di colore casuale*/
	for(i=5;i<RIGHE;i++){
		for(j=0;j<COLONNE;j++){
			gioco->campo[i][j].tipo=MOSTRO;
			gioco->campo[i][j].colore=rand()%3;
			/*Controlla prima in verticale e poi in orizzontale*/
			if(i>1 && gioco->campo[i-1][j].colore == gioco->campo[i][j].colore && gioco->campo[i-2][j].colore == gioco->campo[i][j].colore){
				/*Se il colore è "sbagliato", viene cambiato di un valore*/
				gioco->campo[i][j].colore=(gioco->campo[i][j].colore+1)%3;
				if(j>1 && gioco->campo[i][j-1].colore == gioco->campo[i][j].colore && gioco->campo[i][j-2].colore == gioco->campo[i][j].colore){
					gioco->campo[i][j].colore=(gioco->campo[i][j].colore+1)%3;
				}
			}
			/*Controlla prima in orizzontale e poi in verticale*/
			else if(j>1 && gioco->campo[i][j-1].colore == gioco->campo[i][j].colore && gioco->campo[i][j-2].colore == gioco->campo[i][j].colore){
				gioco->campo[i][j].colore=(gioco->campo[i][j].colore+1)%3;
				if(i>1 && gioco->campo[i-1][j].colore == gioco->campo[i][j].colore && gioco->campo[i-2][j].colore == gioco->campo[i][j].colore){
					gioco->campo[i][j].colore=(gioco->campo[i][j].colore+1)%3;
				}
			}
		}
	}
	/*Finché il numero di virus del campo è maggiore di quello voluto, viene scelta una cella a caso e svuotata*/
	while(conta_virus(gioco)>n_virus){
		i=rand()%(RIGHE-5)+5;
		j=rand()%COLONNE;
		gioco->campo[i][j].tipo=VUOTO;
	}
}

void step(struct gioco *gioco, enum mossa comando) {
	int i,j,n_virus,esci=0,iniziato=0;
	/*Se il campo è senza pastiglie (esempio: la partita è appena iniziata), il conteggio dell'id si azzera*/
	for(i=0;i<RIGHE;i++)
		for(j=0;j<COLONNE;j++)
			if(gioco->campo[i][j].tipo == PASTIGLIA)
				iniziato=1;
	if(!iniziato){
		gioco->active_id=0;
		gioco->combo=1;
	}
	/*La variabile di controllo "esci" serve a dare una priorità alle azioni compiute ad ogni step
	  In questa funzione, la dicitura "if(!esci)" significa "se non è stata eseguita nessun'altra azione,..."*/

	/*Comincia a scorrere l'intera matrice*/
	for(i=0;i<RIGHE;i++){
		for(j=0;j<COLONNE;j++){
			/*Controlla che la cella contenga una pastiglia attiva e la muove in base al comando dato*/
			if(!esci && gioco->campo[i][j].tipo == PASTIGLIA && gioco->campo[i][j].id == gioco->active_id){
				if(comando == NONE){
					/*Scende in basso di una posizione*/
					if(caduta_pastiglia(gioco, i, j))
						esci=1;
				}
				else if(comando == SINISTRA){
					/*Pastiglia verticale*/
					if(i<(RIGHE-1) && j>0 && gioco->campo[i+1][j].id == gioco->campo[i][j].id && gioco->campo[i][j-1].tipo == VUOTO && gioco->campo[i+1][j-1].tipo == VUOTO){
						gioco->campo[i][j-1]=gioco->campo[i][j];
						gioco->campo[i+1][j-1]=gioco->campo[i+1][j];
						gioco->campo[i][j].tipo=VUOTO;
						gioco->campo[i+1][j].tipo=VUOTO;
						gioco->campo[i][j].id=-1;
						gioco->campo[i+1][j].id=-1;
						esci=1;
					}
					/*Pastiglia orizzontale*/
					else if(j>0 && j<(COLONNE-1) && gioco->campo[i][j+1].id == gioco->campo[i][j].id && gioco->campo[i][j-1].tipo == VUOTO){
						gioco->campo[i][j-1]=gioco->campo[i][j];
						gioco->campo[i][j]=gioco->campo[i][j+1];
						gioco->campo[i][j+1].tipo=VUOTO;
						gioco->campo[i][j+1].id=-1;
						esci=1;
					}
					/*Caso in cui la mossa non è possibile, quindi scende semplicemente di una posizione*/
					else if(caduta_pastiglia(gioco, i, j))
						esci=1;
				}
				else if(comando == DESTRA){
					/*Pastiglia verticale*/
					if(i<(RIGHE-1) && j<(COLONNE-1) && gioco->campo[i+1][j].id == gioco->campo[i][j].id && gioco->campo[i][j+1].tipo == VUOTO && gioco->campo[i+1][j+1].tipo == VUOTO){
						gioco->campo[i][j+1]=gioco->campo[i][j];
						gioco->campo[i+1][j+1]=gioco->campo[i+1][j];
						gioco->campo[i][j].tipo=VUOTO;
						gioco->campo[i+1][j].tipo=VUOTO;
						gioco->campo[i][j].id=-1;
						gioco->campo[i+1][j].id=-1;
						esci=1;
					}
					/*Pastiglia orizzontale*/
					else if(j<(COLONNE-2) && gioco->campo[i][j+1].id == gioco->campo[i][j].id && gioco->campo[i][j+2].tipo == VUOTO){
						gioco->campo[i][j+2]=gioco->campo[i][j+1];
						gioco->campo[i][j+1]=gioco->campo[i][j];
						gioco->campo[i][j].tipo=VUOTO;
						gioco->campo[i][j].id=-1;
						esci=1;
					}
					/*Mossa non possibile*/
					else if(caduta_pastiglia(gioco, i, j))
						esci=1;
				}
				else if(comando == GIU){
					/*Ripete la funzione di caduta riga per riga finché non giunge alla fine*/
					while(caduta_pastiglia(gioco, i, j)){
						i=i+1;
						esci=1;
					}
				}
				else if(comando == ROT_SX){
					/*Pastiglia verticale*/
					if(i<(RIGHE-1) && j<(COLONNE-1) && gioco->campo[i+1][j].id == gioco->campo[i][j].id && gioco->campo[i+1][j+1].tipo == VUOTO){
						gioco->campo[i+1][j+1]=gioco->campo[i+1][j];
						gioco->campo[i+1][j]=gioco->campo[i][j];
						gioco->campo[i][j].tipo=VUOTO;
						gioco->campo[i][j].id=-1;
						esci=1;
					}
					/*Pastiglia verticale lungo la parete destra*/
					if(i<(RIGHE-1) && j==(COLONNE-1) && gioco->campo[i+1][j].id == gioco->campo[i][j].id && gioco->campo[i+1][j-1].tipo == VUOTO){
						gioco->campo[i+1][j-1]=gioco->campo[i][j];
						gioco->campo[i][j].tipo=VUOTO;
						gioco->campo[i][j].id=-1;
						esci=1;
					}
					/*Pastiglia orizzontale*/
					else if(i>0 && j<(COLONNE-1) && gioco->campo[i][j+1].id == gioco->campo[i][j].id && gioco->campo[i-1][j].tipo == VUOTO){
						gioco->campo[i-1][j]=gioco->campo[i][j+1];
						gioco->campo[i][j+1].tipo=VUOTO;
						gioco->campo[i][j+1].id=-1;
						esci=1;
					}
					/*Mossa non possibile*/
					else if(caduta_pastiglia(gioco, i, j))
						esci=1;
				}
				else if(comando == ROT_DX){
					/*Pastiglia verticale*/
					if(i<(RIGHE-1) && j<(COLONNE-1) && gioco->campo[i+1][j].id == gioco->campo[i][j].id && gioco->campo[i+1][j+1].tipo == VUOTO){
						gioco->campo[i+1][j+1]=gioco->campo[i][j];
						gioco->campo[i][j].tipo=VUOTO;
						gioco->campo[i][j].id=-1;
						esci=1;
					}
					/*Pastiglia verticale lungo la parete destra*/
					if(i<(RIGHE-1) && j==(COLONNE-1) && gioco->campo[i+1][j].id == gioco->campo[i][j].id && gioco->campo[i+1][j-1].tipo == VUOTO){
						gioco->campo[i+1][j-1]=gioco->campo[i+1][j];
						gioco->campo[i+1][j]=gioco->campo[i][j];
						gioco->campo[i][j].tipo=VUOTO;
						gioco->campo[i][j].id=-1;
						esci=1;
					}
					/*Pastiglia orizzontale*/
					else if(i>0 && j<(COLONNE-1) && gioco->campo[i][j+1].id == gioco->campo[i][j].id && gioco->campo[i-1][j].tipo == VUOTO){
						gioco->campo[i-1][j]=gioco->campo[i][j];
						gioco->campo[i][j]=gioco->campo[i][j+1];
						gioco->campo[i][j+1].tipo=VUOTO;
						gioco->campo[i][j+1].id=-1;
						esci=1;
					}
					/*Mossa non possibile*/
					else if(caduta_pastiglia(gioco, i, j))
						esci=1;
				}
			}
		}
	}
	/*Se non è stato possibile eseguire nessuna mossa, vuol dire che la pastiglia non è più attiva (perché ha raggiunto il fondo o un 	   ostacolo), quindi deve essere aggiornato l'id per la prossima pastiglia che entrerà*/
	if(!esci)
		gioco->active_id=gioco->active_id+1;
	n_virus=conta_virus(gioco);
	/*Scorre tutto il campo controllando ed eliminando le combo*/
	if(!esci){
		for(i=0;i<RIGHE;i++){
			for(j=0;j<COLONNE;j++){
				if(gioco->campo[i][j].tipo == PASTIGLIA || gioco->campo[i][j].tipo == MOSTRO)
					if(rimuovi_combo(gioco, i, j))
						esci=1;
			}
		}
		/*Effettiva eliminazione delle celle delle combo*/
		for(i=0;i<RIGHE;i++){
			for(j=0;j<COLONNE;j++){
				if(gioco->campo[i][j].id==-2){
					gioco->campo[i][j].tipo=VUOTO;
					gioco->campo[i][j].id=-1;
				}
			}
		}
		/*Se nella combo sono stati eliminati anche virus, deve essere aumentato il moltiplicatore*/
		if(conta_virus(gioco)<n_virus)
			gioco->combo=gioco->combo*2;
	}
	/*Controlla se ci sono pastiglie pendenti da far scendere*/
	if(!esci)
		if(controlla_pastiglie_sospese(gioco))
			esci=1;
	if(!esci){
		/*Crea una nuova pastiglia con colori random, azzera le combo e aggiorna l'id*/
		srand(time(NULL));
		gioco->combo=1;
		gioco->campo[0][COLONNE/2-1].tipo=PASTIGLIA;
		gioco->campo[0][COLONNE/2].tipo=PASTIGLIA;
		gioco->campo[0][COLONNE/2-1].colore=rand()%3;
		gioco->campo[0][COLONNE/2].colore=rand()%3;
		gioco->campo[0][COLONNE/2-1].id=gioco->active_id;
		gioco->campo[0][COLONNE/2].id=gioco->active_id;
	}
}

enum stato vittoria(struct gioco *gioco) {
	int i=0,j=0,vittoria=1;
	/*Per prima cosa controlla se è un game over: le due celle al centro della prima riga devono essere libere*/
	if((gioco->campo[0][COLONNE/2].tipo != VUOTO && gioco->campo[1][COLONNE/2].tipo != VUOTO && gioco->campo[1][COLONNE/2].id != gioco->campo[0][COLONNE/2].id) || (gioco->campo[0][COLONNE/2-1].tipo != VUOTO && gioco->campo[1][COLONNE/2-1].tipo != VUOTO && gioco->campo[1][COLONNE/2-1].id != gioco->campo[0][COLONNE/2-1].id)){
		return SCONFITTA;
	}
	else{
		/*Se non è rimasto nessun virus la partita è vinta*/
		for(i=0;i<RIGHE;i++){
			for(j=0;j<COLONNE;j++){
				if(gioco->campo[i][j].tipo == MOSTRO)
					vittoria=0;
			}
		}
		if(vittoria)
			return VITTORIA;
		else
			return IN_CORSO;
	}
}

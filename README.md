# DrMario
Implementazione in C del videogioco per NES "Dr. Mario". Progetto finale per il corso di Programmazione 2017/2018, facoltà di Informatica, Università Ca' Foscari.

## Come compilarlo ed eseguirlo
Per la componente grafica e la gestione dell'input, il programma necessita della libreria SDL 2.0.
Su Linux il comando per installarla è:
```
sudo apt install libsdl2-dev
```
Per compilare il programma basta eseguire il makefile digitando il comando
```
make
```
ed infine
```
./drmario
```
per eseguire il programma.

## Parametri
-s : modifica la velocità di caduta delle pastiglie (in secondi).
-d : specifica il livello di difficoltà (0-15).
-f : specifica il nome del file da cui caricare il campo di gioco.
-h : mostra l'help.

-d e -f sono mutualmente esclusivi.

### Esempi
```
./drmario -s 0.5
```
genera una partita con velocità di 0.5 secondi per step.
```
./drmario -d 8
```
genera una partita di difficoltà 8.
```
./drmario -f campo.txt
```
genera una partita con il campo specificato.

Il campo di gioco è una matrice di 16 righe x 8 colonne.
Quindi il file .txt deve contenere 16 righe, e ogni riga deve contenere 8 caratteri:
'R' per un virus rosso, 'G' per un virus giallo, 'B' per un virus blu, ' ' per una cella vuota.

In assenza di parametri, viene generata una partita di difficoltà 5 e velocità di 0.3 secondi.

## Comandi
→ : Sposta la pillola verso destra
← : Sposta la pillola verso sinistra
↓ : Spinge la pillola in basso
Z : Ruota la pillola in senso antiorario
X : Ruota la pillola in senso orario
Q : Esce dal gioco

## Note
Parte grafica, gestione input e funzione main (file "drmario_main.c", "game.c") NON SONO scritti da me, ma dal tutor del corso.
Io mi sono occupato delle funzioni di logica del gioco (file "drmario.c").

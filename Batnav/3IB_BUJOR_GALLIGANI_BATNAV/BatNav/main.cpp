#include <iostream>
#include <fstream>
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>
#include "iocolor.h"

using namespace std;

#define MAX 30
#define VAL 10

struct Suoni {
    bool sottofondo=true;
    bool speciali=true;
};

struct Flotta {
    int lung=0;
    int nave[2][5]= {{0,0,0,0,0},{0,0,0,0,0}};
    char nome[15];
    bool isColpito[5]={false,false,false,false,false};
    bool isAffondato=false;
    int verso=0;
};

struct Statistiche {
    char nome[16];
    int celle=0;
    int celleTot=0;
    int tempo=0;
    int totNavi=0;
    Flotta navi[VAL];
};

struct Dim {
    int r;
    int c;
};

Suoni menuIniziale(Suoni impost);
void inputGiocatori(Statistiche gioc[], Statistiche vincitori[]);
void coutGriglia(Statistiche gioc[], int turno, Dim lung, int griglia[][20][20], int caso);
void cambioTurno(Statistiche gioc[], int p);
void posizNavi(Dim lung, int mat[][20][20], Statistiche gioc[], int p);
Dim cinGriglia();
void coutElementi (int turno, int griglia[][20][20], Dim lung, int a, Statistiche gioc[], int caso);
void gestNavi (Statistiche gioc[], Dim lung);
void creaFile();
Dim attacco(Dim lung, int mat[][20][20], int p, Statistiche gioc[], Suoni impost, Dim colpo);
bool checkWin(Statistiche gioc[], Statistiche vincitori[], int t, int totG);
void inizMat(int griglia[][20][20]);
void coda(Suoni impost);
int verificaColpo(Statistiche gioc[], int p, Dim colpo, Suoni impost);
void schermFin();
int coutLog(Dim lung, Dim colpo, Statistiche gioc[], int p, int stato, char log[][20][20], int n, bool isAttacco);
void nave();
void clearPoz(int l);
void classifica(Statistiche vincitori[], int totG);

int main() {
    system("mode con: cols=160 lines=100");
    Statistiche gioc[2], vincitori[10];
    Dim lung, colpo;
    int turno, risp, griglia[2][20][20], totG=0, stato, n;
    bool win=false, isAttacco=false;
    char log[2][20][20];
    Suoni impost;
    setColor(0,15);
    creaFile();
    sndPlaySound("suoni\\menu.wav", SND_LOOP | SND_ASYNC);
    impost=menuIniziale(impost);
    do {
        n=0;
        inizMat(griglia);
        inputGiocatori(gioc, vincitori);
        lung=cinGriglia();
        gestNavi (gioc, lung);
        if (impost.sottofondo) {
            sndPlaySound("suoni\\silenzio.wav", SND_SYNC);
            sndPlaySound("suoni\\sottofondo2.wav", SND_LOOP | SND_ASYNC);
        }
        for (turno=0; turno<2; turno++) {
            cambioTurno(gioc, turno);
            coutGriglia(gioc, turno, lung, griglia, 3);
            posizNavi(lung, griglia, gioc, turno);
        }
        while (!win) {
            for (turno=0; turno<2; turno++) {
                cambioTurno(gioc, turno);
                coutGriglia(gioc, turno, lung, griglia, 0);
                n=coutLog(lung, colpo, gioc, turno, stato, log, n, isAttacco);
                isAttacco=true;
                colpo=attacco(lung, griglia, turno, gioc, impost, colpo);
                stato=verificaColpo(gioc, turno, colpo, impost);
                coutGriglia(gioc, turno, lung, griglia, 0);
                n=coutLog(lung, colpo, gioc, turno, stato, log, n, isAttacco);
                isAttacco=false;
                if (impost.speciali) {
                    if (stato==0) {
                        sndPlaySound("suoni\\mancato.wav", SND_SYNC);
                    } else if (stato==1) {
                        sndPlaySound("suoni\\colpito.wav", SND_SYNC);
                    } else if (stato==2) {
                        sndPlaySound("suoni\\affondato.wav", SND_SYNC);
                    }
                    if (impost.sottofondo) {
                        sndPlaySound("suoni\\silenzio.wav", SND_SYNC);
                        sndPlaySound("suoni\\sottofondo2.wav", SND_LOOP | SND_ASYNC);
                    }
                }
                gioc[turno].tempo++;
                win=checkWin(gioc, vincitori, turno, totG);
                if (win) {
                    turno=0;
                    coutGriglia(gioc, turno, lung, griglia, 1);
                    turno=2;
                }
            }
        }
        totG++;
        win=false;
        gotoXY(55, lung.r*4+13);
        cout << "Premi 1 per fare un'altra partita, 2 per uscire...";
        do {
            gotoXY(55, lung.r*4+15);
            cout << " ";
            gotoXY(55, lung.r*4+15);
            cin >> risp;
        } while (risp!=1&&risp!=2);
    } while(risp==1);
    classifica(vincitori, totG);
    coda(impost);
    schermFin();
}

void inizMat(int griglia[][20][20]) {
    int i, j, l;
    for (i=0; i<2; i++) {
        for (j=0; j<20; j++) {
            for (l=0; l<20; l++) {
                griglia[i][j][l]=0;
            }
        }
    }
}

void creaFile() {
    ofstream fFile;
    fFile.open("predef\\4da5.dat", ios::binary);
    fFile << 5 << endl << 5 << endl << 5 << endl << 5 << endl;
    fFile.close();
    fFile.open("predef\\Classico.dat", ios::binary);
    fFile << 1 << endl << 1 << endl << 1 << endl << 1 << endl << 2 << endl << 2 << endl << 2 << endl << 3 << endl << 3 << endl << 4 << endl;
    fFile.close();
    fFile.open("predef\\Prova.dat", ios::binary);
    fFile << 1 << endl << 2 << endl;
    fFile.close();
    fFile.open("predef\\TutteCorte.dat", ios::binary);
    fFile << 1 << endl << 1 << endl << 1 << endl << 1 << endl << 1 << endl << 2 << endl << 2 << endl << 2 << endl << 2 << endl << 2 << endl;
    fFile.close();
}

Suoni menuIniziale(Suoni impost) {
    int menu;
    bool uscita=false;
    do {
        system ("Cls");
        cout << "Che cosa vuoi fare?" << endl;
        cout << "\nGioca[1]\nRegolamento[2]\nSuoni[3]\n";
        cin >> menu;
        switch (menu){
        case 3:
            do{
                gotoXY(0,9);
                cout << " ";
                gotoXY(0,6);
                if (impost.sottofondo){
                    cout << "Sottofondi[1]: ";
                    cout << "ON " << endl;
                } else{
                    cout << "Sottofondi[1]: ";
                    cout << "OFF" << endl;
                }
                if (impost.speciali){
                    cout << "Speciali[2]: ";
                    cout << "ON " << endl;
                } else{
                    cout << "Speciali[2]: ";
                    cout << "OFF" << endl;
                }
                cout << "Esci[3]" << endl;
                cin >> menu;
                switch (menu){
                case 1:
                    if (impost.sottofondo==true){
                        impost.sottofondo=false;
                        sndPlaySound(NULL, SND_ASYNC);
                    }
                    else{
                        impost.sottofondo=true;
                        sndPlaySound("suoni\\menu.wav", SND_ASYNC | SND_LOOP);
                    }
                    break;
                case 2:
                    if (impost.speciali==true){
                        impost.speciali=false;
                    }
                    else{
                        impost.speciali=true;
                    }
                    break;
                case 3:
                    uscita=true;
                }
            }while(!uscita);
            uscita=false;
            break;
        case 2:
            do {
                gotoXY(0,10);
                cout << " ";
                gotoXY(0,12);
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                cout << "                                                                                  " << endl;
                gotoXY(0,6);
                cout << "Info basilari[1]\nNavi[2]\nGriglia[3]\nEsci[4]" << endl;
                cin >> menu;
                gotoXY(0,12);
                switch (menu){
                case 1:
                    cout << "Giocatori: 2\nDurata media:<5-30 minuti>" << endl;
                    cout << "\nLo scopo del gioco e' di individuare e distruggere le navi dell'avversario.";
                    cout << "\nLe navi vengono piazzate da entrambi i giocatori all'inizio della partita." << endl;
                    cout << "\nA turno, ogni giocatore sceglie una cella dove colpire, e sapra' se il colpo\ne' andato a vuoto o meno.";
                    break;
                case 2:
                    cout << "\nLe navi sono di dimensione variabile tra 1 e 5 celle e sono le seguenti";
                    cout << "\n\t1 cella: Caravella";
                    cout << "\n\t2 celle: Nave d'assalto";
                    cout << "\n\t3 celle: Sottomarino";
                    cout << "\n\t4 celle: Corazzata";
                    cout << "\n\t5 celle: Portaerei" << endl;
                    cout << "Le navi possono essere messe in orizzontale o in verticale,\nma NON in diagonale.";
                    cout << "E' possibie scegliere un elenco di navi dai predefiniti\no crearne uno personalizzato,";
                    cout << "ma entrambi i giocatori useranno le stesse navi." << endl;
                    break;
                case 3:
                    cout << "\nSulla griglia, le zone colpite saranno indicate con il rosso se era presente\nuna nave, con il blu se c'era solo acqua,";
                    cout << " con il ciano se c'e' acqua non ancora\ncolpita e con il verde se c'e' una nave non colpita. Questi ultimi due casi non\nsaranno";
                    cout << "visibili sul radar, ma solo sulla propria griglia" << endl;
                    cout << "\nNella classifica finale saranno mostrati solo i giocatori\nche hanno vinto una partita" << endl;
                    break;
                case 4:
                    uscita=true;
                }
                system("Pause");
            } while (!uscita);
            uscita=false;
            break;
        case 1:
            uscita=true;
        }
    } while (uscita==false);
    return impost;
}

void inputGiocatori(Statistiche gioc[], Statistiche vincitori[]) {
    int c, t, i;
    bool uguale;
    system("cls");
    do {
        uguale=false;
        do {
            t=3;
            for (c=0; c<2; c++) {
                gotoXY(12,c+t+1);
                cout<< "                                  	";
                gotoXY(20,c+t+3);
                cout<< "                                  	";
                t++;
                gotoXY(12,c+t);
                fflush(stdin);
                cout << "Nome G" << c+1 << ": ";
                cin.getline(gioc[c].nome,16);
            }
        }
        while(!strcmp(gioc[0].nome,gioc[1].nome));
        for(c=0; c<2; c++) {
            for(i=0; i<10; i++) {
                if(!strcmp(gioc[c].nome,vincitori[i].nome)) {
                    uguale=true;
                }
            }
        }
    } while(uguale);
}

void cambioTurno(Statistiche gioc[], int p) {
    system("cls");
    gotoXY(60,20);
    cout << "Tocca a " << gioc[p].nome <<endl;
    gotoXY(60,30);
    system("pause");
}

Dim cinGriglia() {
    Dim lung;
    do {
        system("cls");
        cout << "\n\n\tInserisci le dimensioni della griglia (da 10 a 20)\n\tRighe: ";
        cin >> lung.r;
        cout << "\tColonne: ";
        cin >> lung.c;
    }
    while (lung.r<10||lung.r>20||lung.c<10||lung.c>20);
    return lung;
}

void coutGriglia (Statistiche gioc[], int turno, Dim lung, int griglia[][20][20], int caso) {
    int i, j, r, a, cost=2;
    system("cls");
    for (a=0; a<caso+1; a++) {
        for (r=0; r<cost; r++) {
            if (caso==3) {
                caso=0;
                cost=1;
            }
            if (a==0) {
                if (r==1) {
                    gotoXY(93+3*lung.c/2, 2);
                    cout << "RADAR";
                    if (caso==1) {
                        gotoXY(80-strlen(gioc[0].nome)/2,4);
                        cout << gioc[0].nome;
                    }
                } else {
                    if (cost==1) {
                        gotoXY(74, 2);
                        cout << "LE TUE NAVI";
                    } else {
                        gotoXY(70-(lung.c+1)*3+lung.c, 2);
                        cout << "LE TUE NAVI";
                    }
                }
            } else {
                if (r==1) {
                    gotoXY(93+3*lung.c/2, lung.r*2+7);
                    cout << "RADAR";
                    gotoXY(80-strlen(gioc[1].nome)/2,lung.r*2+9);
                    cout << gioc[1].nome;
                } else {
                    gotoXY(70-(lung.c+1)*3+lung.c, lung.r*2+7);
                    cout << "LE TUE NAVI";
                }
            }
            setColor(0,15);
            if (a==0) {
                if (r==1) {
                    gotoXY(96, 4);
                } else if (cost==1) {
                    gotoXY(80-(lung.c+1)*3/2, 4);
                } else {
                    gotoXY(70-(lung.c+1)*3, 4);
                }
            } else {
                if (r==1) {
                    gotoXY(96, (lung.r+1)*2+7);
                } else {
                    gotoXY(70-(lung.c+1)*3, (lung.r+1)*2+7);
                }
            }
            cout << " ";
            for (i=0; i<lung.c; i++) {
                if (i<9) {
                    cout << "0";
                }
                cout << i+1 << " ";
            }
            if (a==0) {
                if (r==1) {
                    gotoXY(96, 5);
                }
                else {
                    if (cost==1) {
                        gotoXY(80-(lung.c+1)*3/2, 5);
                    } else {
                        gotoXY(70-(lung.c+1)*3, 5);
                    }
                }
            } else {
                if (r==1) {
                    gotoXY(96, lung.r*2+10);
                }
                else {
                    gotoXY(70-(lung.c+1)*3, lung.r*2+10);
                }
            }
            for (i=0; i<lung.r*2+1; i++) {
                for (j=0; j<lung.c; j++) {
                    if (i==0) {
                        if (j==0) {
                            putchar(218);
                        } else if (j==lung.c-1) {
                            putchar(196);
                            putchar(196);
                            putchar(194);
                            putchar(196);
                            putchar(196);
                            putchar(191);
                        } else {
                            putchar(196);
                            putchar(196);
                            putchar(194);
                        }
                    } else if (i==lung.r*2) {
                        if (j==0) {
                            putchar(192);
                        } else if (j==lung.c-1) {
                            putchar(196);
                            putchar(196);
                            putchar(193);
                            putchar(196);
                            putchar(196);
                            putchar(217);
                        } else {
                            putchar(196);
                            putchar(196);
                            putchar(193);
                        }
                    } else if (i%2==0) {
                        if (j==0) {
                            putchar(195);
                        } else if (j==lung.c-1) {
                            if (r==0) {
                                setColor(3,15);
                            }
                            putchar(196);
                            putchar(196);
                            putchar(197);
                            putchar(196);
                            putchar(196);
                            setColor(0,15);
                            putchar(180);
                        } else {
                            if (r==0) {
                                setColor(3,15);
                            }
                            putchar(196);
                            putchar(196);
                            putchar(197);
                            setColor(0,15);
                        }
                    } else {
                        if (j==0) {
                            putchar(179);
                        } else if (j==lung.c-1) {
                            cout << "  ";
                            if (r==0) {
                                setColor(3,15);
                            }
                            putchar(179);
                            setColor(0,15);
                            cout << "  ";
                            putchar(179);
                        } else {
                            cout << "  ";
                            if (r==0) {
                                setColor(3,15);
                            }
                            putchar(179);
                            setColor(0,15);
                        }
                    }
                }
                cout << endl << " ";
                if (i%2==0&&i<lung.r*2) {
                    if (a==0) {
                        if (r==1) {
                            gotoXY(93, i+6);
                        } else if (r==0) {
                            if (cost==1) {
                                gotoXY(80-(lung.c+2)*3/2, i+6);
                            } else {
                                gotoXY(70-(lung.c+2)*3, i+6);
                            }
                        }
                    } else {
                        if (r==1) {
                            gotoXY(93, i+lung.r*2+11);
                        } else if (r==0) {
                            gotoXY(70-(lung.c+2)*3, i+lung.r*2+11);
                        }
                    }
                    putchar (i/2+65);
                    cout << " ";
                } else if (r==0) {
                    cout << "  ";
                }
                if (a==0) {
                    if (r==1) {
                        gotoXY(96, i+6);
                    } else if (r==0) {
                        if (cost==1) {
                            gotoXY(80-(lung.c+1)*3/2, i+6);
                        } else {
                            gotoXY(70-(lung.c+1)*3, i+6);
                        }
                    }
                } else {
                    if (r==1) {
                        gotoXY(96, i+lung.r*2+11);
                    } else if (r==0) {
                        gotoXY(70-(lung.c+1)*3, i+lung.r*2+11);
                    }
                }
            }
        }
        coutElementi (turno, griglia, lung, a, gioc, cost);
    }
}

void posizNavi(Dim lung, int mat[][20][20], Statistiche gioc[], int p) {
    int risp, i, posc, appo, j, mass, mass1;
    char posr;
    bool isFinish=false, isUsato;
    for(i=0; i<gioc[p].totNavi; i++) {
        do {
            if(j==1) {
                isFinish=true;
            }
            do {
                clearPoz(2*lung.r+6);
                gotoXY(0,2*lung.r+6);
                cout << "\n" << gioc[p].nome << ":";
                isUsato=false;
                cout << "\n" << gioc[p].navi[i].nome << " (lunghezza " << gioc[p].navi[i].lung << "):" << endl;
                if(gioc[p].navi[i].lung>1) {
                    do {
                        cout << "\nOrizzontale[1]\nVerticale[2]" << endl;
                        cin >> risp;
                    } while(risp!=1&&risp!=2);
                    gioc[p].navi[i].verso=risp;
                }
                cout << "\nIndicare le coordinate della prima parte della nave " << i+1 << endl;
                do {
                    fflush(stdin);
                    cout << "Riga(lettera): ";
                    cin >> posr;
                    fflush(stdin);
                    appo=int(posr);
                    if(appo<97) {
                        appo-=64;
                    } else {
                        appo-=96;
                    }
                    mass=0;
                    if(risp==2) {
                        mass=gioc[0].navi[i].lung-1;
                    }
                } while(appo<=0||appo>lung.r-mass);
                do {
                    cout << "Colonna(cifra): ";
                    cin >> posc;
                    mass1=0;
                    if(risp==1) {
                        mass1=gioc[0].navi[i].lung-1;
                    }
                } while(posc<=0||posc>lung.c-mass1);
                appo--;
                posc--;
                if(risp==1) {
                    for(j=0; j<gioc[p].navi[i].lung; j++) {
                        if(mat[p][appo][j+posc]==1) {
                            cout << "Disposizione gia' in uso, reinserire nave " << i+1 << endl;
                            isUsato=true;
                            for(j=j;j>=0;j--){
                                mat[p][appo][j+posc]=0;
                            }
                            for(int c=i-1;c>=0;c--){
                               for(j=0;j<gioc[p].navi[c].lung; j++){
                                    mat[p][gioc[p].navi[c].nave[0][j]][gioc[p].navi[c].nave[1][j]]=1;
                               }
                               j=gioc[p].navi[i].lung;
                            }
                        } else {
                            if(!isUsato){
                                mat[p][appo][j+posc]=1;
                                gioc[p].navi[i].nave[0][j]=appo;
                                gioc[p].navi[i].nave[1][j]=posc+j;
                            }
                        }
                    }
                } else {
                    for(j=0; j<gioc[p].navi[i].lung; j++) {
                        if(mat[p][j+appo][posc]==1) {
                            cout << "Disposizione gia' in uso, reinserire nave " << i+1 << endl;
                            isUsato=true;
                            for(j=j;j>=0;j--){
                                mat[p][j+appo][posc]=0;
                            }
                            for(int c=i-1;c>=0;c--){
                               for(j=0;j<gioc[p].navi[c].lung; j++){
                                    mat[p][gioc[p].navi[c].nave[0][j]][gioc[p].navi[c].nave[1][j]]=1;
                               }
                            }
                            for(int c=i+1;c<gioc[p].totNavi;c++){
                               for(j=0;j<gioc[p].navi[c].lung; j++){
                                    mat[p][gioc[p].navi[c].nave[0][j]][gioc[p].navi[c].nave[1][j]]=1;
                               }
                               j=gioc[p].navi[i].lung;
                            }
                        } else {
                            if(!isUsato){
                                mat[p][j+appo][posc]=1;
                                gioc[p].navi[i].nave[0][j]=appo+j;
                                gioc[p].navi[i].nave[1][j]=posc;
                            }
                        }
                    }
                }
            } while(isUsato);
            j=0;
            coutGriglia(gioc, p, lung, mat, 3);
            if(isFinish) {
                i=gioc[p].totNavi-1;
            }
            if(i==gioc[p].totNavi-1) {
                do {
                    gotoXY(0,2*lung.r+7);
                    cout << "Vuoi continuare[1]\no modificare una nave[2]?" << endl;
                    cin >> j;
                }while(j!=1&&j!=2);
                j=j*(-1)+3;
                if(j==1) {
                    isFinish=false;
                    do{
                        cout << "Quale nave vuoi modificare?" << endl;
                        cin >> i;
                    } while(i<=0||i>gioc[p].totNavi);
                    i--;
                    for(posc=0; posc<gioc[p].navi[i].lung; posc++) {
                        mat[p][gioc[p].navi[i].nave[0][posc]][gioc[p].navi[i].nave[1][posc]]=0;
                    }
                }
            }
        } while(!isFinish&&j==1);
    }
}

void coutElementi (int turno, int griglia[][20][20], Dim lung, int a, Statistiche gioc[], int caso) {
    int r, c, g, n=79, i, j, x, y;
    if (lung.c%2==0) {
        n++;
    }
    if (a==1) {
        turno=turno*(-1)+1;
    }
    for (g=0; g<caso; g++) {
        for (r=0; r<lung.r; r++) {
            for (c=0; c<lung.c; c++) {
                if (g==0) {
                    if (a==0) {
                        if (caso==1) {
                            gotoXY(n-(lung.c+2)*3/2+(c+1)*3, (r-1)*2+8);
                        } else {
                            gotoXY(71-(lung.c-c+3)*3+6, (r-1)*2+8);
                        }
                    } else {
                        gotoXY(71-(lung.c-c+5)*3+12, r*2+lung.r*2+11);
                    }
                    switch (griglia[turno][r][c]) {
                    case -1:
                        setColor(1,1);
                        break;
                    case 1:
                        setColor(14,14);
                        break;
                    case 2:
                        setColor(12,12);
                        break;
                    default:
                        setColor(3,3);
                    }
                } else {
                    if (a==0) {
                        gotoXY(94+(c+1)*3, (r-1)*2+8);
                    } else {
                        gotoXY(94+(c+1)*3, (r-1)*2+lung.r*2+13);
                    }
                    switch (griglia[turno*(-1)+1][r][c]) {
                    case -1:
                        setColor(1,1);
                        break;
                    case 1:
                        setColor(0,0);
                        break;
                    case 2:
                        setColor(12,12);
                        break;
                    default:
                        setColor(0,0);
                    }
                }
                putchar(219);
                putchar(219);
                setColor(0,15);
            }
        }
    }
    for(i=0;i<gioc[turno].totNavi;i++){
        for(j=1;j<gioc[turno].navi[i].lung;j++){
            x=griglia[turno][gioc[turno].navi[i].nave[0][j-1]][gioc[turno].navi[i].nave[1][j-1]];
            y=griglia[turno][gioc[turno].navi[i].nave[0][j]][gioc[turno].navi[i].nave[1][j]];
            if ((x==1||x==2)&&(y==1||y==2)) {
                if(x==2&&y==2) {
                    setColor(12,15);
                } else {
                    setColor(14,15);
                }
                switch(gioc[turno].navi[i].verso) {
                case 1:
                    if (a==0) {
                        if (caso==1) {
                            gotoXY(n-(lung.c+2)*3/2+gioc[turno].navi[i].nave[1][j-1]*3+5, (gioc[turno].navi[i].nave[0][j-1]+2)*2+2);
                        } else {
                            gotoXY(73-(lung.c-gioc[turno].navi[i].nave[1][j-1]+2)*3+3, (gioc[turno].navi[i].nave[0][j-1]+2)*2+2);
                        }
                    } else {
                        gotoXY(73-(lung.c-gioc[turno].navi[i].nave[1][j-1]+2)*3+3, (gioc[turno].navi[i].nave[0][j-1]+lung.r)*2+11);
                    }
                    putchar(179);
                    break;
                case 2:
                    if (a==0) {
                        if (caso==1) {
                            gotoXY(n-(lung.c+2)*3/2+gioc[turno].navi[i].nave[1][j-1]*3+3, (gioc[turno].navi[i].nave[0][j-1]+2)*2+3);
                        } else {
                            gotoXY(69-(lung.c-gioc[turno].navi[i].nave[1][j-1]+2)*3+5, (gioc[turno].navi[i].nave[0][j-1]+2)*2+3);
                        }
                    } else {
                        gotoXY(69-(lung.c-gioc[turno].navi[i].nave[1][j-1]+2)*3+5, (gioc[turno].navi[i].nave[0][j-1]+lung.r)*2+12);
                    }
                    putchar(196);
                    putchar(196);
                    break;
                }
                setColor(0,15);
            }
        }
    }
}

void gestNavi(Statistiche gioc[], Dim lung) {
    int n, risp, menu, mostra, celle=0, nave;
    bool fine=false, errore=false;
    fstream fNavi;
    do {
        do {
            system("cls");
            cout << "\n\n\tVuoi scegliere un elenco di navi[1]\n\to crearne uno da zero[2]?" << endl << "\t";
            cin >> menu;
        } while (menu!=1&&menu!=2);
        switch (menu) {
            case 1:
                do {
                    system("cls");
                    errore=false;
                    cout << "\n\n\tProva[1]\t(consigliato per una partita veloce)\n\t4 da 5[2]\n\tClassico[3]\n\tTutte corte[4]\n\t";
                    cin >> risp;
                    switch (risp) {
                    case 1:
                        fNavi.open("predef\\Prova.dat",ios::in);
                        break;
                    case 2:
                        fNavi.open("predef\\4da5.dat",ios::in);
                        break;
                    case 3:
                        fNavi.open("predef\\Classico.dat",ios::in);
                        break;
                    case 4:
                        fNavi.open("predef\\TutteCorte.dat",ios::in);
                        break;
                    }
                    if (!fNavi.is_open()) {
                        errore=true;
                        cout << "\n\tErrore durante l'apertura del file" << endl;
                        cout << "\tPremi 0 per riprovare\n\tPremi 1 per creare una lista\n\t";
                        cin >> risp;
                    } else {
                        cout << "\n\tDimensioni delle navi:\n";
                        fNavi >> mostra;
                        while (!fNavi.eof()) {
                            cout << "\t" << mostra << endl;
                            celle+=mostra;
                            fNavi >> mostra;
                        }
                        cout << "\n\tPremi 0 per cambiare scelta, 1 per confermare\n\t";
                        cin >> risp;
                        if (risp<1||risp>4) {
                            cout << "\tScelta annullata" << endl;
                            fNavi.close();
                        }
                        else {
                            cout << "\tScelta confermata" << endl;
                        }
                        if (celle>(lung.r*lung.c)/4) {
                            cout << "\tQuesto elenco contiene navi troppo grandi per la griglia" << endl;
                            risp=0;
                            fNavi.close();
                        }
                    }
                } while (risp<1||risp>4);
                if (!errore) {
                    n=0;
                    fNavi.clear();
                    fNavi.seekg(0, ios::beg);
                    fNavi >> gioc[0].navi[n].lung;
                    while (!fNavi.eof()) {
                        gioc[0].celle+=gioc[0].navi[n].lung;
                        n++;
                        fNavi >> gioc[0].navi[n].lung;
                    }
                    gioc[0].totNavi=n;
                    gioc[0].celleTot=gioc[0].celle;
                    break;
                } else {
                    fNavi.close();
                }
            case 2:
                do {
                    system("cls");
                    cout << "\n\n\tInserisci le dimensioni [non piu di 5] di un massimo di 10 navi, premi 0 per concludere la lista" << endl;
                    celle=0;
                    fine=false;
                    n=0;
                    do {
                        cout << "\t";
                        cin >> nave;
                        if (nave<0||nave>5) {
                            fine=true;
                        }
                        else {
                            gioc[0].navi[n].lung=nave;
                        }
                        celle+=nave;
                        n++;
                    } while(!fine&&nave!=0&&n<10);
                    if (fine) {
                        cout << "\n\tLa lunghezza della nave e' errata" << endl;
                    }
                    if (nave==0) {
                        n--;
                    }
                    if (n==0) {
                        cout << "\n\tNon hai inserito navi!" << endl;
                        fine=true;
                    }
                    if (celle>(lung.r*lung.c)/4) {
                        if (!fine) {
                            cout << "\n";
                        }
                        cout << "\tLe celle occupate sono troppe" << endl;
                        fine=true;
                    }
                    else {
                        gioc[0].celle=celle;
                        gioc[0].celleTot=celle;
                        gioc[0].totNavi=n;
                    }
                    if (fine) {
                        cout << "\n\tReinserisci l'elenco";
                    }
                    cout << "\n\t";
                    system("pause");
                } while (fine);
                break;
        }
    } while (menu!=1&&menu!=2);
    for (n=0; n<gioc[0].totNavi; n++) {
        switch (gioc[0].navi[n].lung) {
            case 1:
                strcpy(gioc[0].navi[n].nome, "Caravella");
                break;
            case 2:
                strcpy(gioc[0].navi[n].nome, "Nave d'assalto");
                break;
            case 3:
                strcpy(gioc[0].navi[n].nome, "Sottomarino");
                break;
            case 4:
                strcpy(gioc[0].navi[n].nome, "Corazzata");
                break;
            case 5:
                strcpy(gioc[0].navi[n].nome, "Portaerei");
        }
        gioc[1].navi[n]=gioc[0].navi[n];
    }
    gioc[1].celle=gioc[0].celle;
    gioc[1].celleTot=gioc[0].celleTot;
    gioc[1].totNavi=gioc[0].totNavi;
}

Dim attacco(Dim lung, int mat[][20][20], int p, Statistiche gioc[], Suoni impost, Dim colpo) {
    int risp;
    char posr;
    p=p*(-1)+1;
    do {
        clearPoz(2*lung.r+6);
        gotoXY(0,2*lung.r+6);
        p=p*(-1)+1;
        cout << gioc[p].nome << ":";
        cout << "\nIndicare le coordinate del colpo " << endl;
        do {
            fflush(stdin);
            cout << "Riga(lettera): ";
            cin >> posr;
            fflush(stdin);
            colpo.r=int(posr);
            if(colpo.r<97) {
                colpo.r-=64;
            }
            else {
                colpo.r-=96;
            }
        } while(colpo.r<=0||colpo.r>lung.r);
        do {
            cout << "Colonna(cifra): ";
            cin >> colpo.c;
        } while(colpo.c<=0||colpo.c>lung.c);
        colpo.r--;
        colpo.c--;
        cout << "Sei sicuro di voler colpire qua?\nSi[1]\nNo[2]" << endl;
        cin >> risp;
        p=p*(-1)+1;
    } while(mat[p][colpo.r][colpo.c]==-1||mat[p][colpo.r][colpo.c]==2||risp!=1);
    if(mat[p][colpo.r][colpo.c]==0) {
        mat[p][colpo.r][colpo.c]=-1;
    }
    else {
        mat[p][colpo.r][colpo.c]=2;
        gioc[p].celle--;
    }
    return colpo;
}

bool checkWin(Statistiche gioc[], Statistiche vincitori[], int t, int totG){
    bool win;
    int mass, i, j;
    Statistiche appo;
    ofstream fClassifica;
    if (gioc[(t*(-1))+1].celle==0){
        win=true;
        if (gioc[t].celleTot>vincitori[9].celleTot||(gioc[t].celleTot==vincitori[9].celleTot&&gioc[t].tempo<vincitori[9].tempo)) {
            vincitori[9]=gioc[t];
            for (i=0; i<9; i++) {
                mass=i;
                for (j=i+1;j<10;j++) {
                    if (vincitori[j].celleTot>vincitori[mass].celleTot||(vincitori[j].celleTot==vincitori[mass].celleTot&&vincitori[j].tempo<vincitori[mass].tempo)) {
                        mass=j;
                    }
                }
                appo=vincitori[mass];
                vincitori[mass]=vincitori[i];
                vincitori[i]=appo;
            }
        }
        fClassifica.open("Classifica.txt");
        for (i=0; i<totG+1; i++) {
            fClassifica << i+1 << " posizione\n" << vincitori[i].nome << endl << vincitori[i].celleTot << " celle\n" << vincitori[i].tempo << " turni\n\n";
        }
        fClassifica.close();
    } else {
        win=false;
    }
    return win;
}

void coda(Suoni impost) {
    char titoli[MAX][MAX],
    sup[MAX][MAX]{"Menu iniziale: ", "  Stefano Galligani", "Input giocatori:", "  Marcel Bujor", "Calcolo classifica:", "  Stefano Galligani",
    "Gestione navi:", "  Stefano Galligani","Posizionamento navi:", "  Marcel Bujor", "Grafica:", "  Stefano Galligani", "Gestione eventi:",
    "  Marcel Bujor", "Gestione e scelta suoni:", "  Stefano Galligani", "Funzione di attacco:", "  Marcel Bujor", "Organizzazione sviluppo:",
    "  Stefano Galligani"};
    int i, j, z;
    system("Cls");
    if (impost.sottofondo) {
        sndPlaySound("suoni\\silenzio.wav", SND_SYNC);
        sndPlaySound("suoni\\Classifica.wav", SND_ASYNC);
    }
    for(i=0; i<MAX; i++) {
        for (j=0; j<MAX-1; j++) {
            strcpy(titoli[j],titoli[j+1]);
        }
        strcpy(titoli[MAX-1],sup[i]);
        for (z=10; z<MAX; z++) {
            if (z>MAX-i&&z>10&&z<MAX) {
                gotoXY(45,z);
                cout << titoli[z] << "                                    ";
            }
        }
        Sleep(500);
    }
}

int verificaColpo(Statistiche gioc[], int p, Dim colpo, Suoni impost){
    int i, j, c, stato=0;
    p=p*(-1)+1;
    for(i=0;i<gioc[p].totNavi;i++){
        for(j=0;j<gioc[p].navi[i].lung;j++){
            if(colpo.r==gioc[p].navi[i].nave[0][j]&&colpo.c==gioc[p].navi[i].nave[1][j]){
                gioc[p].navi[i].isColpito[j]=true;
                gioc[p].navi[i].isAffondato=true;
                for(c=0;c<gioc[p].navi[i].lung&&gioc[p].navi[i].isAffondato;c++){
                    if(!gioc[p].navi[i].isColpito[c]){
                        gioc[p].navi[i].isAffondato=false;
                    }
                }
                if (gioc[p].navi[i].isAffondato) {
                    stato=2;
                } else {
                    stato=1;
                }
            }
        }
    }
    return stato;
}

void schermFin() {
    int i, j;
    system("cls");
    for (i=0; i<59; i++) {
        if (i==0) {
            putchar(218);
            for (j=0; j<158; j++) {
                putchar(196);
            }
            putchar(191);
        }
        if (i==1) {
            putchar(179);
            cout << " ";
            putchar(218);
            for (j=0; j<154; j++) {
                putchar(196);
            }
            putchar(191);
            cout << " ";
            putchar(179);
        }
        if (i>1&&i<11) {
            putchar(179);
            cout << " ";
            putchar(179);
            for (j=0; j<154; j++) {
                cout << " ";
            }
            putchar(179);
            cout << " ";
            putchar(179);
        }
        if (i>10&&i<30) {
            putchar(179);
            cout << " ";
            putchar(179);
            for (j=0; j<48; j++) {
                cout << " ";
            }
            switch (i-10) {
            case 1:
                cout << "                          _                               ";
                break;
            case 2:
                cout << "                         (_)                              ";
                break;
            case 3:
                cout << "       __ _ _ __ __ _ _____  ___    _ __   ___ _ __       ";
                break;
            case 4:
                cout << "      / _` | '__/ _` |_  / |/ _ \\  | '_ \\ / _ \\ '__/      ";
                break;
            case 5:
                cout << "     | (_| | | | (_| |/ /| |  __/  | |_) |  __/ |         ";
                break;
            case 6:
                cout << "      \\__, |_|  \\__,_/___|_|\\___|  | .__/ \\___|_|         ";
                break;
            case 7:
                cout << "       __/ |                       | |                    ";
                break;
            case 8:
                cout << "      |___/                        |_|                    ";
                break;
            case 9:
                cout << "                                                          ";
                break;
            case 10:
                cout << "                                                          ";
                break;
            case 11:
                cout << "                               _                 _        ";
                break;
            case 12:
                cout << "                              (_)               | |       ";
                break;
            case 13:
                cout << "   __ ___   _____ _ __    __ _ _  ___   ___ __ _| |_ ___  ";
                break;
            case 14:
                cout << "  / _` \\ \\ / / _ \\ '__/  / _` | |/ _ \\ / __/ _` | __/ _ \\ ";
                break;
            case 15:
                cout << " | (_| |\\ V /  __/ |    | (_| | | (_) | (_| (_| | || (_) |";
                break;
            case 16:
                cout << "  \\__,_| \\_/ \\___|_|     \\__, |_|\\___/ \\___\\__,_|\\__\\___/ ";
                break;
            case 17:
                cout << "                          __/ |                           ";
                break;
            case 18:
                cout << "                         |___/                            ";
                break;
            case 19:
                cout << "                                                          ";
                break;
            case 20:
                cout << "                                                          ";
                break;
            }
            for (j=0; j<48; j++) {
                cout << " ";
            }
            putchar(179);
            cout << " ";
            putchar(179);
        }
        if (i>29&&i<57) {
            putchar(179);
            cout << " ";
            putchar(179);
            for (j=0; j<154; j++) {
                cout << " ";
            }
            putchar(179);
            cout << " ";
            putchar(179);
        }
        if (i==57) {
            putchar(179);
            cout << " ";
            putchar(192);
            for (j=0; j<154; j++) {
                putchar(196);
            }
            putchar(217);
            cout << " ";
            putchar(179);
        }
        if (i==58) {
            putchar(192);
            for (j=0; j<158; j++) {
                putchar(196);
            }
            putchar(217);
        }
        cout << endl;
    }
    nave();
}

int coutLog(Dim lung, Dim colpo, Statistiche gioc[], int p, int stato, char log[][20][20], int n, bool isAttacco){
    int i, j, g;
    char appo[20];
    g=p*(-1)+1;
    gotoXY(77,4);
    cout << "Storia:";
    if(isAttacco){
        strcpy(log[0][n],gioc[p].nome);
        if(stato==0){
            strcpy(log[1][n],"colpo mancato");
        }
        if(stato==1){
            strcpy(log[1][n],"nave colpita");
        }
        if(stato==2){
            strcpy(appo," affondata");
            for(i=0;i<gioc[p].totNavi;i++){
                for(j=0;j<gioc[p].navi[i].lung;j++){
                    if(colpo.r==gioc[g].navi[i].nave[0][j]&&colpo.c==gioc[g].navi[i].nave[1][j]){
                        strcpy(log[1][n],gioc[p].navi[i].nome);
                        strcat(log[1][n],appo);
                    }
                }
            }
        }
        n++;
    }
    for(i=0;i<n;i++){
        gotoXY(68,3*i+6);
        cout << log[0][i];
        gotoXY(68,3*i+7);
        cout << log[1][i];
    }
    if(3*n>=2*lung.r){
        strcpy(log[0][0],log[0][n-1]);
        strcpy(log[1][0],log[1][n-1]);
        n=1;
    }
    gotoXY(0,0);
    return n;
}

void nave(){
int i, n;
    gotoXY(79,40);
    cout << "~;";
    gotoXY(78,41);
    cout << ",/|";
    putchar(92);
    gotoXY(76,42);
    cout << ",/' |   ,";
    gotoXY(81,42);
    putchar(92);
    gotoXY(83,42);
    putchar(92);
    gotoXY(74,43);
    cout << ",/'   | |  ";
    putchar(92);
    gotoXY(72,44);
    cout << ",/'     | |   |";
    gotoXY(70,45);
    cout << ",/'       |/    |";
    gotoXY(68,46);
    cout << ",/__________|-----' ,";
    gotoXY(67,47);
    cout << "___.....-----''-----/";
    gotoXY(67,48);
    putchar(92);
    cout << "                  /";
    for(i=0; i<21; i++) {
        n=i%2;
        if(n==0){
            n=0;
            gotoXY(65,49);
            cout << "                                ";
            gotoXY(67,50);
            cout << "                                ";
        }else{
            n=2;
            gotoXY(63,49);
            cout << "                                ";
            gotoXY(65,50);
            cout << "                                ";
        }
        gotoXY(n+63,49);
        cout << "~~-~^~^~`~^~`~^^~^~-^~^~^~-~^~^";
        gotoXY(-n+65,50);
        cout << "~-^~^-`~^~-^~^`^~^-^~^`^~^-~^";
        Sleep(300);
        gotoXY(0,58);
    }
}

void clearPoz(int l){
    int i;
    for(i=0;i<15;i++){
        gotoXY(0,i+l);
        cout << "                                                       ";
    }
}

void classifica(Statistiche vincitori[], int totG) {
    int i;
    system("cls");
    cout << endl << "                                                              _               _  __ _" << endl;
    cout << "                                                             | |             (_)/ _(_)" << endl;
    cout << "                                                          ___| | __ _ ___ ___ _| |_ _  ___ __ _ " << endl;
    cout << "                                                         / __| |/ _` / __/ __| |  _| |/ __/ _` |" << endl;
    cout << "                                                        | (__| | (_| \\__ \\__ \\ | | | | (_| (_| |" << endl;
    cout << "                                                         \\___|_|\\__,_|___/___/_|_| |_|\\___\\__,_|" << endl;
    for (i=0; i<totG; i++) {
        if (i!=9) {
            gotoXY(65, i*5+10);
        } else {
            gotoXY(64, i*5+10);
        }
        cout << i+1<< " POSTO:";
        gotoXY(65, i*5+11);
        cout << "\t" << vincitori[i].nome;
        gotoXY(65, i*5+12);
        cout << "\tFlotta: " << vincitori[i].celleTot << " celle";
        gotoXY(65, i*5+13);
        cout << "\tTempo: " << vincitori[i].tempo << " turni";
    }
    gotoXY(59, totG*5+12);
    system("pause");
}

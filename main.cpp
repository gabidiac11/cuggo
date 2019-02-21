//PUNE -lwinmm LA "OTHER LINKER SETTINGS"
#include <iostream>
#include <winbgim.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<mmsystem.h>//pt audio
#include<windows.h>//pt audio
#include<fstream>
using namespace std;
#define max_library 100


/*
mciSendString( "open click.wav type mpegvideo alias click", lpszReturnString, lstrlen(lpszReturnString), NULL);
mciSendString( "open erase.wav type mpegvideo alias erase", lpszReturnString, lstrlen(lpszReturnString), NULL);
mciSendString( "open horn.wav type mpegvideo alias horn", lpszReturnString, lstrlen(lpszReturnString), NULL);
mciSendString( "open inserare_piesa.wav type mpegvideo alias inserare_piesa", lpszReturnString, lstrlen(lpszReturnString), NULL);
mciSendString( "open keyboard.wav type mpegvideo alias keyboard", lpszReturnString, lstrlen(lpszReturnString), NULL);
mciSendString( "open mutare_piesa.wav type mpegvideo alias mutare_piesa", lpszReturnString, lstrlen(lpszReturnString), NULL);
mciSendString( "open muzica_de_fundal.wav type mpegvideo alias muzica_de_fundal", lpszReturnString, lstrlen(lpszReturnString), NULL);
mciSendString( "open remiza.wav type mpegvideo alias remiza", lpszReturnString, lstrlen(lpszReturnString), NULL);
mciSendString( "open selectare_piesa.wav type mpegvideo alias selectare_piesa", lpszReturnString, lstrlen(lpszReturnString), NULL);
mciSendString( "open winning_music.wav type mpegvideo alias winning_music", lpszReturnString, lstrlen(lpszReturnString), NULL);
*/

int nr_miscari = 0;


char cuvinte[40][200];
char limbi[15][20];
int index_limba = 0;

void inregistrare_limbi( )
{
    ifstream fin;

    int i = 0;
    fin.open("lista_limbi.txt");

    if (fin)
    {
        fin>>index_limba;

        for( i = 0; i<12; i++ )
        {
            fin>>limbi[i];
            cout<<limbi[i]<<endl;



        }


    }

    fin.close();

}

void inregistrare_cuvinte( )
{
    ifstream fin;

    int i = 0;
    fin.open( limbi[index_limba] );

    if(fin)
    {
        //citim la inceput ceva pt ca nu se realizeaza bine prima citire...
        fin.getline(cuvinte[i], 200);
        for(  i = 0; i<37; i++ )
        {
            fin.getline(cuvinte[i], 200);

            cout<<i<<" "<<cuvinte[i]<<endl;


            if( fin.eof() )
                break;

        }


    }

    fin.close();
}


struct casuta {

    int player; // 0, daca e gol; 1, daca e ocupat de player1; 2, daca e ocupat de player2;

    //PENTRU RUNDA 2 -> daca e 3 atunci e goala, dar acolo se poate muta o anumita piesa de tip player1/player2


    int x, y; //locatia intersectiei diagonalelor unei casute, gen mijlocul casutei

    int frec;

};

struct locatie {

        int i, j;

};

struct locatie_playeri_in_tabel{

    int i,j;

};

struct librarie{
    char player1[15], player2[15];
    int scor_playeri[2], remize;
};

char jucatori[2][15];//numele playerilor poate vom pune

int stanga, sus, width, height, latura;//latura unei casute

casuta tabel[50][50];//are in componenta mijlocul(x,y) fiecarei casute si o variabila care arata de catre cine e ocupata casuta

casuta tabel_copie1[50][50];


int nr_linii, nr_col;

locatie pozitii_mutare[4];  //locatii unde o piesa poate fi mutata ( pe linie sau coloana )
locatie locatie_de_mutat; // locatie din care o piesa este mutata si care urmeaza sa fie golita

locatie restart[2]; // locatii pentru desenare buton de restart
locatie buton_meniu[2];

int latimeButonStart = 30;
int border_tabel = 30;

locatie_playeri_in_tabel piese_player[2][8];//aici sunt memorate locatiile ocupate de piese,
                                            //8 locatii piese de tip player1 pe linia 0,
                                            //8 locatii piese player2 pe linia 1



int culoare_player[3];

int nr_bilanturi = 1, index_bilant_activ = 0;//adica se unde se retine pl1 vs pl2 care joaca acum
librarie bilanturi[max_library];

void inregistrare_miscare_in_fisier()
{

    ifstream fin;
    ofstream fout;

    fin.open("istoric_mutari.txt");
    fout.open("istoric_mutari_aux.txt");

    //bag mutarile precedente in auxiliar
    if (fout && fin)
    {

        int nr, player_val;

        fin>>nr;
        fout<<nr;

        for( int k = 0; k<nr; k++ )
        {

            for( int i = 1; i<=nr_linii; i++ )
            {

                fout<<"\n";

                for( int j = 1; j<=nr_col; j++ )
                {

                    fin>>player_val;

                    fout<<player_val<<" ";

                }
            }

        }

    }

    fin.close();
    fout.close();


    //actualizez numarul de miscari
    //copii tot din auziliar in fisierul cu miscari si bag la sfarsit si ultima miscare

    fin.open("istoric_mutari_aux.txt");
    fout.open("istoric_mutari.txt");

    if (fout && fin)
    {

        int nr, player_val;

        fin>>nr;

        //actualizez numarul de miscari actuale
        fout<<nr+1;

        for( int k = 0; k<nr; k++ )
        {

            for( int i = 1; i<=nr_linii; i++ )
            {

                fout<<"\n";

                for( int j = 1; j<=nr_col; j++ )
                {

                    fin>>player_val;

                    fout<<player_val<<" ";

                }
            }

        }

    }

    for( int i = 1; i<=nr_linii; i++ )
    {

        fout<<"\n";

        for( int j = 1; j<=nr_col; j++ )
        {



            fout<<tabel[i][j].player<<" ";

        }
    }

    fin.close();
    fout.close();

    nr_miscari++;

}

void drawX( int i, int j, int color )
{
    setlinestyle(0, 1, 3);
    setcolor(color );

    line( tabel[i][j].x - (latura/2 - 1), tabel[i][j].y - (latura/2), tabel[i][j].x + (latura/2 - 1), tabel[i][j].y + (latura/2) );



    line( tabel[i][j].x + (latura/2 - 1), tabel[i][j].y - (latura/2), tabel[i][j].x - (latura/2 - 1), tabel[i][j].y + (latura/2) );


}

void deseneaza_buton( int x1, int x2, int y1, int y2, char a[], int culoar )
{

    setcolor(culoar);
    setlinestyle(3, 0, 3);
    rectangle( x1, y1, x2, y2 );

    settextjustify(CENTER_TEXT,CENTER_TEXT);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( (x1+x2)/2, (y1+y2)/2, a);

}

bool verifica_cursorul_atinge_buton( int x1, int x2, int y1, int y2, int x_mouse, int y_mouse )
{

    if( x_mouse>=x1 && x_mouse<=x2 && y_mouse>=y1 && y_mouse<=y2 )
        return true;

    return false;

}

void retinere_scor_in_fisier()
{

    ofstream fout;

    fout.open("scoruri_arhiva.txt");

    if (fout)
    {

        fout<<nr_bilanturi<<"\n";
        for( unsigned int i = 0; i<nr_bilanturi; i++ )
        {
            fout<<bilanturi[i].player1<<" ";
            fout<<bilanturi[i].player2<<" ";
            fout<<bilanturi[i].scor_playeri[0]<<" ";
            fout<<bilanturi[i].scor_playeri[1]<<" ";
            fout<<bilanturi[i].remize<<"\n";

        }


    }

    fout.close();

}

void scrie_castigator(int player)
{
    setcolor(14);
    settextjustify(1, 1);
    settextstyle(0, HORIZ_DIR, 5);

    if( player >2 )
    {

        PlaySound("remiza.wav", NULL, SND_FILENAME|SND_ASYNC);

       //char remiza[] = "REMIZA";
        outtextxy( getmaxx()/2, getmaxy()-45, cuvinte[ 25 ] );
        bilanturi[ index_bilant_activ ].remize +=1;

        retinere_scor_in_fisier();

        return;
    }

    PlaySound("winning_music.wav", NULL, SND_FILENAME|SND_ASYNC);

    char winner[50];
    strcpy(winner, cuvinte[ 32 ]);
    strcat(winner, jucatori[player-1]);
    outtextxy( getmaxx()/2, getmaxy()-45, winner  );

    int pl_inamic = (player == 1)? 2 : 1;

    for(unsigned int i = 0; i<nr_bilanturi; i++)
    {
            if(  ( jucatori[player-1], bilanturi[i].player1  ) == 0 && strcmp( jucatori[pl_inamic-1], bilanturi[i].player2 ) == 0  )
            {
                player = 1;
            }else{
                    if(  strcmp( jucatori[pl_inamic-1], bilanturi[i].player1  ) == 0 && strcmp( jucatori[player-1], bilanturi[i].player2 ) == 0  )
                    {
                        player = 2;
                    }

            }
    }

    bilanturi[ index_bilant_activ ].scor_playeri[player-1] +=1;
    retinere_scor_in_fisier();
}
int cul = 0;
void desenare_altceva_x_y_pt_testing(int i, int j, int c)
{

    if( i>9 || j>9)
        return;

            setcolor(c);
            settextjustify(0, 1);
            settextstyle(1, HORIZ_DIR, 1);

            char numbers[]="   ";
            numbers[0] = i + 48;
            numbers[2] = j + 48;

            outtextxy( tabel[i][j].x - latura/2 + 15, tabel[i][j].y - latura/2 + 15, numbers );

//    for( unsigned int i = 1; i<=nr_linii && i<=9; i++ )
//    {
//
//        for(unsigned int j = 1; j<=nr_col && j<=9; j++)
//        {
//
//            setcolor(15);
//            settextjustify(0, 1);
//            settextstyle(1, HORIZ_DIR, 1);
//
//            char numbers[]="       ";
//            numbers[0] = i + 48;
//            numbers[3] = j + 48;
//
//            outtextxy( tabel[i][j].x - latura/2 + 1, tabel[i][j].y - latura/2 + 1, numbers );
//
//        }
//
//    }

}

void desenare_x_y_pt_testing(int i, int j)
{

    if( i>9 || j>9)
        return;

            setcolor(15);
            settextjustify(0, 1);
            settextstyle(1, HORIZ_DIR, 1);

            char numbers[]="   ";
            numbers[0] = i + 48;
            numbers[2] = j + 48;

            outtextxy( tabel[i][j].x - latura/2 + 15, tabel[i][j].y - latura/2 + 15, numbers );

//    for( unsigned int i = 1; i<=nr_linii && i<=9; i++ )
//    {
//
//        for(unsigned int j = 1; j<=nr_col && j<=9; j++)
//        {
//
//            setcolor(15);
//            settextjustify(0, 1);
//            settextstyle(1, HORIZ_DIR, 1);
//
//            char numbers[]="       ";
//            numbers[0] = i + 48;
//            numbers[3] = j + 48;
//
//            outtextxy( tabel[i][j].x - latura/2 + 1, tabel[i][j].y - latura/2 + 1, numbers );
//
//        }
//
//    }

}

void desenare_tabla( int n, int m, int latura )
{

     int i,j;

    setcolor(15);//culoare linii
    setlinestyle(1, 1, 3); //

    int poly[8]; //poligonul se construieste din a(x,y), inapoi spre stanga catre B(x,y), apoi merge jos la c(x,y), apoi, spre dreapta la D(x, y)
    poly[0] = stanga + latura*n;   //B
    poly[1] = sus;
    circle(poly[0], poly[1], 10);

    poly[2] = stanga;               //A
    poly[3] = sus;
    circle(poly[2], poly[3], 10);


    poly[4] = stanga;    //C
    poly[5] = sus + latura*m;
    circle(poly[4], poly[5], 10);

    poly[6] = stanga + latura*n;    //D
    poly[7] = sus + latura*m;
    circle(poly[6], poly[7], 10);

    setcolor(15);
    setfillstyle(1,10);
    fillpoly(4, poly);






    setcolor(0);//culoare linii
    setlinestyle(1, 1, 3); //

    for( i=1; i<=n; i++ )
        for( j=1; j<=m; j++ ) // se construieste adaugand pozitia de start (width, height) + distanta_unei_laturi*nr_laturi_de_construit
            {

                tabel[j][i].player = 0;
                tabel[j][i].x = (  ( stanga + latura * (i-1) )  +  (stanga + latura*i) ) / 2;
                tabel[j][i].y = (  ( sus + latura*(j-1) )  +  ( sus + latura*j )  ) / 2;


                rectangle( stanga + latura*(i-1), sus + latura*(j-1), stanga+latura*i, sus+latura*j );


            }

                //satam conturul tabelului in caz ca piesa mutata e la mijloc si strica conturul
    setcolor(15);
    setlinestyle(DASHED_LINE, 0, 2);
    rectangle(stanga, sus, stanga + latura*nr_col, sus + latura*nr_linii);



}


bool se_poate_muta( int i, int j )
{

    if( i<nr_linii && tabel[ i+1 ][j].player == 0 )
    {
            return true;
    }

    if( i>1 && tabel[ i-1 ][j].player == 0 )
    {
            return true;
    }

    if( j<nr_col && tabel[i][ j+1 ].player == 0 )
    {
            return true;
    }

    if( j>1 && tabel[i][ j-1 ].player == 0 )
    {
            return true;
    }

    return false;

}

bool adiacente( int i, int j, int a, int b )
{

    if( i==a && abs(j-b) == 1 )
        return true;

    if( j==b && abs( i-a ) == 1 )
        return true;

    return false;

}

//functia defapt vede daca cel putin o piesa dintr-un grup se poate muta
int cate_piese_se_pot_muta( int i, int j, int &nr, int index )
{
    //cout<<'\n'<<i<<" "<<j;
    if( se_poate_muta(i, j) )//daca o piesa se poate muta grupul nu e izolat
        {
            nr = 0;
            tabel[i][j].frec = 2;//o sa consideram ca un grup nu e inconjurat daca vreo piesa din el are frec = 2, pt ca asta ar conduce spre o piesa care se poate muta
            //cout<<" "<<2<<'\n';
            return 1;
        }
    //daca nu se poate muta incrementam nr de piese din grup si marcam ca am vizitat o piesa izolata
    nr = nr+1;
    tabel[i][j].frec = 1;

    //cout<<" "<<1<<'\n';



    //caut un grup de piese de acelasi fel intre toate cele 8 piese care exista
    for( unsigned int k = 0; k<index; k++ )
    {


        //daca exista un vecin din tot grupul care are posibilitati de a fi mutat atunci grupul nu e inconjurat si toate piesele i-au frecventa 2
        // vad daca ea e izolata si daca e vad daca mai e are si ea niste vecini izolati de acelasi fel


        //caut o piesa adiacenta cu piesa deja verificata de coordonate i, j
        if( adiacente( i, j, piese_player[ tabel[i][j].player-1 ][k].i, piese_player[ tabel[i][j].player-1 ][k].j  )  )
        {

           int a = piese_player[ tabel[i][j].player-1 ][k].i;

           int b = piese_player[ tabel[i][j].player-1 ][k].j;

           if( tabel[a][b].frec == 1 )//vad daca a fost deja evaluata si are statutul de piesa izolata
           {
                //cout<<'\n'<<a<<" "<<b; cout<<" "<<1<<'\n';
                continue;//atunci sar direct la ciclul urmator
           }

            //daca frecventa e 2 atunci piesa nu este izolata sau conduce catre o piesa neizolata din grup
            //daca piesa are piesa din acelasi grup care se pot muta atunci
            //rezulta ca nu avem grup inconjurat
           if( tabel[a][b].frec == 2 || cate_piese_se_pot_muta(a, b, nr, index ) > 0 )
           {
                //cout<<'\n'<<a<<" "<<b; cout<<" "<<2<<'\n';
               tabel[i][j].frec = 2;
                nr = 0;
               return 1;

           }


        }


    }

    return 0;


}

void setare_frecventa(int x)
{
    for( unsigned int i = 1; i<=nr_linii; i++ )
    {
        for(unsigned int j=1; j<=nr_col; j++)
            tabel[ i ][ j ].frec = x;

    }
}

void taiere_grup( int player, int index )//marcheza cu un x de culoare galbena piesele din grupul izolat gasit
{

    for( unsigned int k = 0; k<index; k++ )
    {
        if( tabel[ piese_player[player-1][k].i ][ piese_player[player-1][k].j ].frec == 1  )
            drawX ( piese_player[player-1][k].i, piese_player[player-1][k].j, 14 );

    }

}


int returnare_castigator_runda2( int i, int j, int player_inamic )
{


    taiere_grup(player_inamic, 8);

    return tabel[i][j].player;


}

void gasire_grupuri( locatie grupuri[][8], int &nr_grupuri, int nr_piese[], int index_player, int player, int start_i, int start_j )
{

    //i-au gasesc recursiv un grup cautand intre piesele inamice vecinii si vecinii vecinilor
    for( unsigned int i = 0; i<index_player; i++ )
    {

        if( tabel[ piese_player[player-1][i].i ][ piese_player[player-1][i].j ].frec == 0 )
        {

            if( adiacente( piese_player[player-1][i].i, piese_player[player-1][i].j, start_i, start_j ) )
            {

                tabel[ piese_player[player-1][i].i ][ piese_player[player-1][i].j ].frec = 1;

                grupuri[nr_grupuri][ nr_piese[nr_grupuri] ].i = piese_player[player-1][i].i;
                grupuri[nr_grupuri][ nr_piese[nr_grupuri] ].j = piese_player[player-1][i].j;

                nr_piese[nr_grupuri]++;

                gasire_grupuri( grupuri, nr_grupuri, nr_piese, index_player, player, piese_player[player-1][i].i, piese_player[player-1][i].j );

            }

        }

    }


}

int castigaRunda2( int i, int j, int player_inamic )
{

    int PieseInconjurate = 0; //memoram cate piese se gasesc intr-un grup izolat adiacent cu piesa mutata
                             //daca nu exista atunci va ramane 0
                             //nu e necasar sa fie grupul cel mai mare fiindca piesa mutata nu poate face parte dintr-un grup izolat
                             //piesa mutata e intodeana cu cel putin o posibilitate de mutare

    //initializam cu 0 casuta de frecventa la toate locatiile nevizitate OCUPATE DE PIESA INAMICA
    for( unsigned int k = 0; k<8; k++ )
        tabel[ piese_player[ player_inamic-1 ][k].i  ][ piese_player[ player_inamic-1 ][k].j  ].frec = 0;


    //caut un grup izolat de piese inamice, grup adiacent cu piesa mutata
    if( i<nr_linii && tabel[ i+1 ][j].player > 0 )
    {


//cout<<"grup1: ";
      if( tabel[ i+1 ][j].player != tabel[i][j].player ) // daca ultima piesa inserata e vecina cu o piesa P de culoare diferita
        {
            if( cate_piese_se_pot_muta( i+1, j, PieseInconjurate, 8  ) == 0 ) // vad daca exista un grup la care apartine P, in care nicio piesa nu se poate muta
            {
//cout<<" "<<PieseInconjurate;

                if(PieseInconjurate>0)
                {

                    return returnare_castigator_runda2(i, j, player_inamic );
                }

            }
        }
//cout<<'\n';
//cout<<"........................ gata"<<'\n';



    }


    if( i>1 && tabel[ i-1 ][j].player > 0 )
    {
//cout<<"grup1: ";
      if( tabel[ i-1 ][j].player != tabel[i][j].player )
        if( cate_piese_se_pot_muta( i-1, j, PieseInconjurate, 8 ) == 0 )
            {
//cout<<" "<<PieseInconjurate;

                if(PieseInconjurate>0)
                {

                    return returnare_castigator_runda2(i, j, player_inamic);
                }

            }
//cout<<'\n';
//cout<<"........................ gata"<<'\n';

    }





    if( j<nr_col && tabel[i][ j+1 ].player > 0 )
    {
//cout<<"grup1: ";
      if( tabel[i][ j+1 ].player != tabel[i][j].player )
        if( cate_piese_se_pot_muta( i, j+1, PieseInconjurate, 8 ) == 0 )
            {

//cout<<" "<<PieseInconjurate;

                if(PieseInconjurate>0)
                {

                    return returnare_castigator_runda2(i, j, player_inamic);
                }


            }
//cout<<'\n';
//cout<<"........................ gata"<<'\n';


    }







    if( j>1 && tabel[i][ j-1 ].player > 0 )
    {
//cout<<"grup1: ";
      if( tabel[i][ j-1 ].player != tabel[i][j].player )
        if( cate_piese_se_pot_muta( i, j-1, PieseInconjurate, 8 ) == 0 )
            {

//cout<<" "<<PieseInconjurate;

                if(PieseInconjurate>0)
                {

                    return returnare_castigator_runda2(i, j, player_inamic);
                }

            }

//cout<<'\n';
//cout<<"........................ gata"<<'\n';


    }
//returnam piesa mutata daca am gasit un grup izolat
//sau 0 daca nu am gasit

        return 0;



}


int castigaRunda1( int piesa_mutata_i, int piesa_mutata_j, int index1, int index2  )
{

    //vad daca pisa mutata/adaugata este adiacenta cu o piesa de culoare diferita

    //daca gasesc o piesa de culoare diferita vad pt ea si pt toate piesele adiacente cu ea daca sunt toate intepenite(nu se pot muta nicaieri), numar cate piese sunt asa

    //vad daca piesa mutata/adaugata e intr-un grup de acelasi fel, vad daca si el e inconjurat NUMAI DACA E IN RUNDA1


    int maxPieseInconjurate_piesa_mutata = 0;//se poate ca piesa adaugata sa faca parte dintr-un grup izolat

    //daca o piesa adiacenta inamica si piesa adaugata fac parte dintr-un grupuri izolate atunci pierde
    //grupul cu piesele cele mai numeroase, sau e remiza in caz de egalitate




//initializam cu 0 casuta de frecventa la toate locatiile nevizitate OCUPATE DE ORICE PIESA
    setare_frecventa(0);

    int player_mutata = tabel[ piesa_mutata_i ][ piesa_mutata_j ].player;
    locatie grupul_piesei_mutate[8][8];
    int nr_piese_grup_piesei_mutate[8] = {0,0,0,0,0,0,0,0};
    int nr_grupuri = 0;


    grupul_piesei_mutate[0][ nr_piese_grup_piesei_mutate[0] ].i = piesa_mutata_i;
    grupul_piesei_mutate[0][ nr_piese_grup_piesei_mutate[0] ].j = piesa_mutata_j;

    tabel[piesa_mutata_i][piesa_mutata_j].frec = 1;

    nr_piese_grup_piesei_mutate[0]++;

    gasire_grupuri( grupul_piesei_mutate, nr_grupuri, nr_piese_grup_piesei_mutate, index1, player_mutata, piesa_mutata_i, piesa_mutata_j  );



    cout<<"\n Grupul piesei CARE A FOST INSERATA *castiga_runda1()* \n";
    for( unsigned int j = 0; j<nr_piese_grup_piesei_mutate[0]; j++ )
    {

        cout<<grupul_piesei_mutate[0][j].i<<" "<<grupul_piesei_mutate[0][j].j<<"\n";

    }
    cout<<"GATA ............................................grupul piesei INSERATA \n";


    bool grupul_piesei_mutate_E_Izolat = true;

    for( unsigned int j = 0; j<nr_piese_grup_piesei_mutate[0]; j++ )
    {

        if( se_poate_muta( grupul_piesei_mutate[0][j].i, grupul_piesei_mutate[0][j].j ) )
            {
                    grupul_piesei_mutate_E_Izolat = false;
                    break;
            }

    }

    if( grupul_piesei_mutate_E_Izolat )
    {
        maxPieseInconjurate_piesa_mutata = nr_piese_grup_piesei_mutate[0];
    }


    int player_inamic;
    locatie grupul_inam[8][8];
    int nr_piese_grup_inam[8] = {0,0,0,0,0,0,0,0};
    int nr_grupuri_inam = 0;


    if( piesa_mutata_i<nr_linii )
    {

        if( tabel[ piesa_mutata_i+1 ][piesa_mutata_j].player!=player_mutata && tabel[ piesa_mutata_i+1 ][piesa_mutata_j].player > 0 )
        {


            if( tabel[ piesa_mutata_i+1 ][piesa_mutata_j].frec == 0 )
            {

                player_inamic = tabel[ piesa_mutata_i+1 ][piesa_mutata_j].player;

                grupul_inam[nr_grupuri_inam][ nr_piese_grup_inam[nr_grupuri_inam] ].i = piesa_mutata_i+1;
                grupul_inam[nr_grupuri_inam][ nr_piese_grup_inam[nr_grupuri_inam] ].j = piesa_mutata_j;

                tabel[ piesa_mutata_i+1 ][piesa_mutata_j].frec = 1;

                nr_piese_grup_inam[nr_grupuri_inam]++;

                gasire_grupuri( grupul_inam, nr_grupuri_inam, nr_piese_grup_inam, index2, player_inamic, piesa_mutata_i+1,  piesa_mutata_j  );

                nr_grupuri_inam++;

            }

        }


    }

    if( piesa_mutata_i>1 )
    {

        if( tabel[ piesa_mutata_i-1 ][piesa_mutata_j].player!=player_mutata && tabel[ piesa_mutata_i-1 ][piesa_mutata_j].player > 0 )
        {


            if( tabel[ piesa_mutata_i-1 ][piesa_mutata_j].frec == 0 )
            {

                player_inamic = tabel[ piesa_mutata_i-1 ][piesa_mutata_j].player;

                grupul_inam[nr_grupuri_inam][ nr_piese_grup_inam[nr_grupuri_inam] ].i = piesa_mutata_i-1;
                grupul_inam[nr_grupuri_inam][ nr_piese_grup_inam[nr_grupuri_inam] ].j = piesa_mutata_j;

                tabel[ piesa_mutata_i-1 ][piesa_mutata_j].frec = 1;

                nr_piese_grup_inam[nr_grupuri_inam]++;

                gasire_grupuri( grupul_inam, nr_grupuri_inam, nr_piese_grup_inam, index2, player_inamic, piesa_mutata_i-1,  piesa_mutata_j  );


                nr_grupuri_inam++;

            }

        }

    }

    if( piesa_mutata_j<nr_col )
    {

        if( tabel[piesa_mutata_i][ piesa_mutata_j+1 ].player!=player_mutata && tabel[piesa_mutata_i][ piesa_mutata_j+1 ].player > 0 )
        {


            if( tabel[piesa_mutata_i][ piesa_mutata_j+1 ].frec == 0 )
            {

                player_inamic = tabel[piesa_mutata_i][ piesa_mutata_j+1 ].player;

                grupul_inam[nr_grupuri_inam][ nr_piese_grup_inam[nr_grupuri_inam] ].i = piesa_mutata_i;
                grupul_inam[nr_grupuri_inam][ nr_piese_grup_inam[nr_grupuri_inam] ].j = piesa_mutata_j+1;

                tabel[piesa_mutata_i][ piesa_mutata_j+1 ].frec = 1;

                nr_piese_grup_inam[nr_grupuri_inam]++;

                gasire_grupuri( grupul_inam, nr_grupuri_inam, nr_piese_grup_inam, index2, player_inamic, piesa_mutata_i,   piesa_mutata_j+1   );

                nr_grupuri_inam++;

            }

        }


    }

    if( piesa_mutata_j>1 )
    {

        if( tabel[piesa_mutata_i][ piesa_mutata_j-1 ].player!=player_mutata && tabel[ piesa_mutata_i ][ piesa_mutata_j-1 ].player > 0 )
        {


            if( tabel[piesa_mutata_i][piesa_mutata_j-1].frec == 0 )
            {

                player_inamic = tabel[piesa_mutata_i][ piesa_mutata_j-1 ].player;

                grupul_inam[nr_grupuri_inam][ nr_piese_grup_inam[nr_grupuri_inam] ].i = piesa_mutata_i;
                grupul_inam[nr_grupuri_inam][ nr_piese_grup_inam[nr_grupuri_inam] ].j = piesa_mutata_j-1;

                tabel[piesa_mutata_i][ piesa_mutata_j-1 ].frec = 1;

                nr_piese_grup_inam[nr_grupuri_inam]++;

                gasire_grupuri( grupul_inam, nr_grupuri_inam, nr_piese_grup_inam, index2, player_inamic, piesa_mutata_i,  piesa_mutata_j-1  );

                nr_grupuri_inam++;

            }

        }

    }


    cout<<"\n Grupurile INAMICE INCIDENTE cu piesa inserata *castiga_runda1()* \n";

    for( unsigned int i = 0; i<nr_grupuri_inam; i++ )
    {

        cout<<"grup 0:\n";
        for( unsigned int j = 0; j<nr_piese_grup_inam[i]; j++ )
        {

            cout<<grupul_inam[i][j].i<<" "<<grupul_inam[i][j].j<<"\n";

        }
        cout<<"\n";

    }
    cout<<"GATA ............................................Grupurile INAMICE INCIDENTE cu piesa mutata \n";



    int ind_grup_inamic_izolat = -1;
    for( unsigned int  i = 0; i<nr_grupuri_inam; i++ )
    {
        bool e_izolat = true;
        for( unsigned int j = 0; j<nr_piese_grup_inam[i]; j++ )
        {

            if( se_poate_muta( grupul_inam[i][j].i, grupul_inam[i][j].j ) )
            {
                e_izolat = false;
                break;
            }

        }

        if(e_izolat)
        {

            if( ind_grup_inamic_izolat > -1 )
            {

                if( nr_piese_grup_inam[i] > nr_piese_grup_inam[ ind_grup_inamic_izolat ] )
                {

                    ind_grup_inamic_izolat = i;

                }

            }else ind_grup_inamic_izolat = i;

        }

    }

    if( maxPieseInconjurate_piesa_mutata==0 && ind_grup_inamic_izolat == -1 )
        return 0;

    if( maxPieseInconjurate_piesa_mutata == nr_piese_grup_inam[ ind_grup_inamic_izolat ] )
        return 3;//remiza


    if( maxPieseInconjurate_piesa_mutata > nr_piese_grup_inam[ ind_grup_inamic_izolat ] )
    {
        setare_frecventa(0);

        for( unsigned int j = 0; j<nr_piese_grup_piesei_mutate[0]; j++ )
        {

            tabel[ grupul_piesei_mutate[0][j].i ][ grupul_piesei_mutate[0][j].j ].frec = 1;

        }

        taiere_grup(player_mutata, index1);

        return player_inamic;
    }

    if( maxPieseInconjurate_piesa_mutata < nr_piese_grup_inam[ ind_grup_inamic_izolat ] )
    {

        setare_frecventa(0);
        for( unsigned int j = 0; j<nr_piese_grup_inam[ ind_grup_inamic_izolat ]; j++ )
        {

            tabel[ grupul_inam[ ind_grup_inamic_izolat ][j].i ][ grupul_inam[ ind_grup_inamic_izolat ][j].j ].frec = 1;

        }

        taiere_grup(player_inamic, index2);

        return player_mutata;
    }

    return 0;


}

//deseneaza un loc gol din tabel
void desenare_loc_de_mutat( int i, int j, int patt, int color )
{


    int poly[8];

    poly[0] = tabel[ i ][ j ].x  + latura/2;
    poly[1] = tabel[ i ][ j ].y - latura/2;

    poly[2] = tabel[ i ][ j ].x - latura/2;
    poly[3] = tabel[ i ][ j ].y - latura/2;

    poly[4] = tabel[ i ][ j ].x - latura/2;
    poly[5] = tabel[ i ][ j ].y + latura/2;

    poly[6] = tabel[ i ][ j ].x + latura/2;
    poly[7] = tabel[ i ][ j ].y + latura/2;




    setcolor(0);//culoare linii
    setfillstyle(patt, color);
    fillpoly(4, poly);

    //satam conturul tabelului in caz ca piesa mutata e la mijloc si strica conturul

    setcolor(15);
    setlinestyle(DASHED_LINE, 0, 2);
    rectangle(stanga, sus, stanga + latura*nr_col, sus + latura*nr_linii);

    setlinestyle(0, 1, 3);



}


int nr_casute_x_vecine( int i, int j, int x, locatie casute_goale[], int &nr_casute_goale )
{

    int nr = 0;
//coloanele care nu exista in stanga sau dreapta sau liniile care nu exista in sus sau jos sunt tratate ca casute inamice
if( i<nr_linii )
    {
        if( x==0 )
        {

            if( tabel[ i+1 ][j].player == 0 && tabel[i+1][j].frec == 0 )
            {

                tabel[i+1][j].frec = 1;

                casute_goale[ nr_casute_goale ].i = i+1;
                casute_goale[ nr_casute_goale ].j = j;

                nr_casute_goale++;

            }

        }else{


            if( tabel[ i+1 ][j].player == x )
            {
                nr++;

            }


        }

    }else{

        //coloanele care nu exista in stanga sau dreapta sau liniile care nu exista in sus sau jos sunt tratate ca casute inamice
        if(x>0)
        {
            nr++;
        }

    }


    if( i>1 )
    {
      if( x==0 )
       {

            if( tabel[ i-1 ][j].player == 0 && tabel[i-1][j].frec == 0 )
            {

                tabel[i-1][j].frec = 1;
                casute_goale[ nr_casute_goale ].i = i-1;
                casute_goale[ nr_casute_goale ].j = j;
                nr_casute_goale++;
            }
       }else{

            if( tabel[ i-1 ][j].player == x )
            {
                nr++;

            }

       }

    }else{
            //coloanele care nu exista in stanga sau dreapta sau liniile care nu exista in sus sau jos sunt tratate ca casute inamice
        if(x>0)
        {
            nr++;
        }

    }


    if( j<nr_col )
    {
        if( x==0 )
        {

                if( tabel[i][ j+1 ].player == 0 && tabel[i][j+1].frec == 0 )
                {

                    tabel[i][j+1].frec = 1;
                    casute_goale[ nr_casute_goale ].i = i;
                    casute_goale[ nr_casute_goale ].j = j+1;
                    nr_casute_goale++;
                }

        }else{
                if( tabel[i][ j+1 ].player == x )
                {
                    nr++;

                }


        }

    }else{
            //coloanele care nu exista in stanga sau dreapta sau liniile care nu exista in sus sau jos sunt tratate ca casute inamice
        if(x>0)
        {
            nr++;
        }

    }

    if( j>1 )
    {
        //daca vreau sa caut un vecin gol atunci ma intereseaza si frecventa lui
        if( x == 0 )
        {
            if(  tabel[i][ j-1 ].player == 0 && tabel[i][j-1].frec == 0 )
            {

                tabel[i][j-1].frec = 1;
                casute_goale[ nr_casute_goale ].i = i;
                casute_goale[ nr_casute_goale ].j = j-1;
                nr_casute_goale++;
            }
        }else{
        //daca caut un vecin de tip 1,2 atunci nu mai intereseaza frecventa

            if(  tabel[i][ j-1 ].player == x )
            {
                nr++;

            }


        }


    }else{

        //coloanele care nu exista in stanga sau dreapta sau liniile care nu exista in sus sau jos sunt tratate ca casute inamice
        if(x>0)
        {
            nr++;
        }

    }

    return nr;

}

void plasare_piesa_computer_runda1( int &piesa_pusa_i, int &piesa_pusa_j, int &index_computer, int index_inamic )
{

    //gasesc toate grupurile de piese inamice ( maxim 8 )
    //aleg grupul cel mai usor de inconjurat
            //daca e o singura pozitie de umplut atunci o umplu si am castigat
            //daca sunt mai multe:
                // pasul 1: gasesc pozitia care pune in cel mai mic pericol piesa ( adica pozitia care e cel mai putin inconjurata )
                // pasul 2: daca gasesc un grup de-al meu care e pe cale sa fie incojurat doar printr-o plasare atunci:
                                //pun o piesa de-a mea in acel loc in incercarea de a ma salva ca sa pot creea mai multe spatii de umplut langa grup ( oricum pierd daca nu o fac )
                           //daca nu gasesc un asemenea grp de-al meu atunci plasez piesa pe pozitia de la pasul 1


    //gasesc toate grupurile de piese inamice ( maxim 8 )
    int nr_grupuri_inamice = 0, nr_piese_inamice[8]={0,0,0,0,0,0,0,0};
    locatie grupuri_inamice[8][8];

    for( unsigned int i = 0; i<index_inamic; i++ )
    {

        tabel[ piese_player[0][i].i ][ piese_player[0][i].j ].frec = 0;

    }

    cout<<'\n'<<".................JOC NOU..........................."<<"index_inamic="<<index_inamic<<'\n';
    cout<<"pise inamice................................"<<'\n';


    //incep sa caut grupuri pt fiecare piesa inamica nevizitata care exista ( player1 este inamic )
    for( unsigned int i = 0; i<index_inamic; i++ )
    {
        cout<<'\n'<<piese_player[0][i].i<<" "<<piese_player[0][i].j<<'\n';

        if( tabel[ piese_player[0][i].i ][ piese_player[0][i].j ].frec == 0 )
        {

            tabel[ piese_player[0][i].i ][ piese_player[0][i].j ].frec = 1;

            grupuri_inamice[nr_grupuri_inamice][ nr_piese_inamice[nr_grupuri_inamice] ].i = piese_player[0][i].i;
            grupuri_inamice[nr_grupuri_inamice][ nr_piese_inamice[nr_grupuri_inamice] ].j = piese_player[0][i].j;

            nr_piese_inamice[ nr_grupuri_inamice ]++;

            gasire_grupuri( grupuri_inamice, nr_grupuri_inamice, nr_piese_inamice, index_inamic, 1, piese_player[0][i].i, piese_player[0][i].j );

            nr_grupuri_inamice++;

        }

    }

    for( unsigned int i = 0; i<nr_grupuri_inamice; i++ )
    {

        cout<<"grup nr: "<<i<<'\n';
        for( unsigned int j = 0; j<nr_piese_inamice[i]; j++ )
        {

            cout<<grupuri_inamice[i][j].i<<" "<<grupuri_inamice[i][j].j<<'\n';

        }
        cout<<"......"<<'\n';

    }


    //i-au fiecare element din grupuri_inamice[i] si vad cate casute goale are ca vecin
    //grupul cu cele mai putine casute goale e ales
    //daca e doar o singura casuta goala am castigat
    //daca sunt mai multe aleg casuta care ma face vecin cu cele mai putine piese inamice
            //dupa ce am gasit acea casuta caut sa vad daca sunt vulnerabil
                        //caut grupurile mele si vad daca exista una care are doar o casuta





    //caut grupul inamic cu CELE MAI PUTINE LOCURI DE UMPLUT pt a fi inconjurat:


    setare_frecventa(0);

    int minimum = 0;

    //presupun ca nu sunt mai multe de 100...
    locatie casute_goale[100];
    int nr_casute_goale = 0;

    //initializez minimul cu nr de piese goale pe care le are primul grup
    for( unsigned int j = 0; j<nr_piese_inamice[0]; j++ )
    {
        minimum += nr_casute_x_vecine( grupuri_inamice[0][j].i, grupuri_inamice[0][j].j, 0, casute_goale, nr_casute_goale );
    }

    minimum = nr_casute_goale;

    cout<<'\n'<<"grup 0, nr casute goale vecine: "<<minimum<<'\n';


    int grup_minim_index = 0;

    locatie casute_goale_copie[100];

    for( unsigned int i = 1; i<nr_grupuri_inamice; i++ )
    {

        int nr_casute = 0;

        //in caz ca spatiile goale verificate anterior overlap somehow
        setare_frecventa(0);

        for( unsigned int j = 0; j<nr_piese_inamice[i]; j++ )
        {

                nr_casute  += nr_casute_x_vecine( grupuri_inamice[i][j].i, grupuri_inamice[i][j].j, 0, casute_goale_copie, nr_casute );

        }

        cout<<"grup "<<i<<", nr casute goale vecine: "<<nr_casute<<'\n';

        if( nr_casute < minimum )
        {
            minimum = nr_casute;
            grup_minim_index = i;

            nr_casute_goale = nr_casute;

            for( unsigned int k = 0; k<nr_casute_goale; k++ )
            {

                casute_goale[k].i = casute_goale_copie[k].i;
                casute_goale[k].j = casute_goale_copie[k].j;

            }

        }


    }

        cout<<"\n casute goale minime langa grupul "<<grup_minim_index<<":\n";
            for( unsigned int k = 0; k<nr_casute_goale; k++ )
            {

                cout<<casute_goale[k].i;
                cout<<" "<<casute_goale[k].j<<'\n';

            }
        cout<<".........................\n";


    //daca e numai o casuta de umplut pt a inconjura un grup inamic atunci am castigat direct
    if( nr_casute_goale == 1 )
    {

        piesa_pusa_i = casute_goale[0].i;
        piesa_pusa_j = casute_goale[0].j;
        return;

    }


    //daca sunt mai multe casute goale la grupul ales alegem pozitia cea mai greu de inconjurat de inamic
    minimum = nr_casute_x_vecine( casute_goale[0].i, casute_goale[0].j, 1, casute_goale, nr_casute_goale );

    piesa_pusa_i = casute_goale[0].i;
    piesa_pusa_j = casute_goale[0].j;

    //aleg o pozitie cel mai putin inconjurata din cele goale
    for( unsigned int j = 1; j<nr_casute_goale; j++ )
    {
            int nr_casute = nr_casute_x_vecine( casute_goale[j].i, casute_goale[j].j, 1, casute_goale, nr_casute_goale );

            if( nr_casute < minimum )
            {
                minimum = nr_casute;
                piesa_pusa_i = casute_goale[j].i;
                piesa_pusa_j = casute_goale[j].j;

            }

    }


//defensiva..................................................................................................................................................................



    int nr_grupurile_mele = 0, nr_piesele_mele[8] = {0,0,0,0,0,0,0,0};
    locatie grupurile_mele[8][8];

    for( unsigned int i = 0; i<index_computer; i++ )
    {

        tabel[ piese_player[1][i].i ][ piese_player[1][i].j ].frec = 0;

    }

    for( unsigned int i = 0; i<index_computer; i++ )
    {

        if( tabel[ piese_player[1][i].i ][ piese_player[1][i].j ].frec == 0 )
        {

            tabel[ piese_player[1][i].i ][ piese_player[1][i].j ].frec = 1;

            grupurile_mele[nr_grupurile_mele][ nr_piesele_mele[nr_grupurile_mele] ].i = piese_player[1][i].i;
            grupurile_mele[nr_grupurile_mele][ nr_piesele_mele[nr_grupurile_mele] ].j = piese_player[1][i].j;

            nr_piesele_mele[ nr_grupurile_mele ]++;

            gasire_grupuri( grupurile_mele, nr_grupurile_mele, nr_piesele_mele, index_computer, 2, piese_player[1][i].i, piese_player[1][i].j );

            nr_grupurile_mele++;

        }

    }

    for( unsigned int i = 0; i<nr_grupurile_mele; i++ )
    {

        int nr_casute = 0;

        //in caz ca spatiile goale verificate anterior overlap somehow
        setare_frecventa(0);

        for( unsigned int j = 0; j<nr_piesele_mele[i]; j++ )
        {

                nr_casute  += nr_casute_x_vecine( grupurile_mele[i][j].i, grupurile_mele[i][j].j, 0, casute_goale_copie, nr_casute );

        }

        if( nr_casute == 1 )
        {


            piesa_pusa_i = casute_goale_copie[0].i;
            piesa_pusa_j = casute_goale_copie[0].j;

            return;

        }


    }



}
//....................................................................

bool punct_de_inserare( int player, int &linia, int &coloana, int &index, bool &restartare, int &i_ant, int &j_ant, bool &menu )
{
    linia=-1;
    coloana=-1;

    int x_mouse=mousex();
    int y_mouse=mousey();

    if( x_mouse>stanga && x_mouse<stanga+width && y_mouse>sus && y_mouse<sus+height )
    {

        int l = (y_mouse-sus)/latura+1;
        int c = (x_mouse-stanga)/latura+1;


//daca casuta e libera, adaugam niste conditii in plus deorece x_mouse si y_mouse au probleme
if( tabel[l][c].player == 0  )
{

        if( i_ant > -1 )
        {
            //stergem hasurul vechi daca pozitia e una noua si nu e ocupata
            if( (l!=i_ant || c!=j_ant) && tabel[i_ant][j_ant].player == 0 )
               desenare_loc_de_mutat( i_ant, j_ant,  1, 10 );
        }

        //we make sure sa nu deseneze inca o data aceeasi locatie
        if( i_ant == -1 || ( l!=i_ant || c!=j_ant ) )
            desenare_loc_de_mutat(l, c, 3, culoare_player[player] );

        i_ant = l;
        j_ant = c;
}else{

    if( i_ant>-1 )
    {


        desenare_loc_de_mutat( i_ant, j_ant,  1, 10 );
        i_ant = -1;
        j_ant = -1;

    }

}

    }


    if(ismouseclick(WM_LBUTTONDOWN))
            {
                clearmouseclick(WM_LBUTTONDOWN);
                x_mouse = mousex();
                y_mouse = mousey();


                if(x_mouse<=restart[1].i && x_mouse>=restart[0].i && y_mouse<=restart[1].j && y_mouse>=restart[0].j )
                {

                    PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                    restartare = true;
                    return false;
                }


                if(x_mouse<=buton_meniu[1].i && x_mouse>=buton_meniu[0].i && y_mouse<=buton_meniu[1].j && y_mouse>=buton_meniu[0].j )
                {

                    PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                    menu = true;
                    return false;
                }


                //daca click(x,y) apartine tabel
                if (x_mouse>=stanga && x_mouse<=stanga+width && y_mouse>=sus && y_mouse<=sus+height)
                {
                     linia=(y_mouse-sus)/latura+1;
                     coloana=(x_mouse-stanga)/latura+1;



                    if (tabel[linia][coloana].player ==0)
                    {

                        PlaySound("inserare_piesa.wav", NULL, SND_FILENAME|SND_ASYNC);
                        tabel[linia][coloana].player = player;

                        //inregistrez fiecare locatie a pieselor ca sa le am la indemana
                        piese_player[player-1][index].i = linia;
                        piese_player[player-1][index].j = coloana;
                        index++;

                        //golim coloana de efectul pe care l-am pus cand cursorul e deasupra unei casute
                        desenare_loc_de_mutat( linia, coloana,  1, 10 );

                        setcolor(1);
                        setfillstyle(1, 1);


                        drawX( linia, coloana, 12 );

                        setcolor(culoare_player[player]);
                        setfillstyle(1,culoare_player[player]);
                        fillellipse(tabel[linia][coloana].x, tabel[linia][coloana].y, latura/4, latura/4);

                        return true;

                    }else return false;


                }else return false;

            }


    return false;



}

int culoare_rezerva[3];
int misc_ind[2];

int prima_runda(bool computer, bool e_cu_miscari)
{
    //pentru ca se schimba culoarea in mod irational, din cauza bibliotecii
    //culoarea normal e modificata doar la inceput si atat,
    //in rest nu e implicata in nicio atribuire, am cautat fiecare linie care contine vectorul culori


    int miscari = 0; // tre sa fie 8 piese de fiecare
     // 1, a castigat player1; 2, a castigat player2

    int piesa_pusa_i, piesa_pusa_j;

    int index1, index2;

    int cine_muta_primul = 4;

    if( !e_cu_miscari )
    {
        index1 = 0;
        index2 = 0;
    }else{
        index1 = misc_ind[0];
        index2 = misc_ind[1];

        if( index2 < index1 )
                cine_muta_primul = 2;





    }



//pentru indicator cine pune piesa....................
    int poly[8];
    poly[0] = stanga + width;
    poly[1] = buton_meniu[0].j;

    poly[2] = buton_meniu[1].i+5;
    poly[3] =  buton_meniu[0].j;

    poly[4] = buton_meniu[1].i+5;
    poly[5] =  buton_meniu[1].j;

    poly[6] = stanga + width;
    poly[7] =  buton_meniu[1].j;

    if( stanga+width - buton_meniu[1].i+5 < 300 )
    {
        poly[0] = buton_meniu[1].i+5 + 300;
        poly[6] = buton_meniu[1].i+5 + 300;

    }



//.................................
    bool restartare = false;
    bool menu = false;

    while( miscari < 8 )
    {
        bool gata = false;

        culoare_player[1] = culoare_rezerva[1];


        //facem un dreptunghi colorat in culoarea playerului care trb sa mute o piesa in prezent
        setcolor(0);
        setlinestyle(0, 1, 0);
        setfillstyle(1, culoare_player[1]);
        fillpoly(4, poly);

        //text cu numele playerului1, care urmeaza sa puna mute o piesa
        setcolor(culoare_player[1]);
        settextjustify(1, 1);
        settextstyle(0, HORIZ_DIR, 3);
        outtextxy(poly[0]-(poly[0] - poly[2])/2, poly[5]-(poly[5]-poly[1])/2+6, jucatori[0]);


        int i_ant = -1, j_ant = -1;

         culoare_player[2] = culoare_rezerva[2];

        //cautare input player1
int castigator;
if(cine_muta_primul > 2)
{

        do{



            gata = punct_de_inserare(1, piesa_pusa_i, piesa_pusa_j, index1, restartare, i_ant, j_ant, menu );

            if( restartare )
                return -1;
            if( menu )
                return -2;

        }while(!gata);

       gata  = false;

        castigator = castigaRunda1( piesa_pusa_i, piesa_pusa_j, index1, index2 );

        inregistrare_miscare_in_fisier();

        if(  castigator > 0 )
        {

            return castigator;

        }

}
//..........................PLAYER 2..........................................................................


        //facem un dreptunghi colorat in culoarea playerului care trb sa mute o piesa in prezent
        setcolor(0);
        setlinestyle(0, 1, 0);
        setfillstyle(1, culoare_player[2]);
        fillpoly(4, poly);

        //text cu numele playerului1, care urmeaza sa puna mute o piesa
        setcolor(culoare_player[2]);
        settextjustify(1,1);
        settextstyle(0, HORIZ_DIR, 3);
        outtextxy(poly[0]-(poly[0] - poly[2])/2, poly[5]-(poly[5]-poly[1])/2+6, jucatori[1]);

       i_ant = -1;
       j_ant = -1;


if(!computer)
{


       //cautare input player2
       do{

        gata = punct_de_inserare(2, piesa_pusa_i, piesa_pusa_j, index2, restartare, i_ant, j_ant, menu );

            if( restartare )
                return -1;

            if( menu )
                return -2;


       }while(!gata);





}else{

       piesa_pusa_i = -1;
       piesa_pusa_j = -1;

       plasare_piesa_computer_runda1( piesa_pusa_i, piesa_pusa_j, index2, index1 );

       tabel[piesa_pusa_i][piesa_pusa_j].player = 2;

       cout<<'\n'<<piesa_pusa_i<<" "<<piesa_pusa_j<<" piesa pusa"<<'\n';

        //inregistrez fiecare locatie a pieselor ca sa le am la indemana
       piese_player[1][index2].i = piesa_pusa_i;
       piese_player[1][index2].j = piesa_pusa_j;
       index2++;


       setcolor(1);
       setfillstyle(1, 1);

       drawX( piesa_pusa_i, piesa_pusa_j, 12 );

       setcolor(culoare_rezerva[2]);
       setfillstyle(1,culoare_rezerva[2]);
       fillellipse(tabel[piesa_pusa_i][piesa_pusa_j].x, tabel[piesa_pusa_i][piesa_pusa_j].y, latura/4, latura/4);


        PlaySound("inserare_piesa.wav", NULL, SND_FILENAME|SND_ASYNC);

      }

        inregistrare_miscare_in_fisier();


       castigator = castigaRunda1( piesa_pusa_i, piesa_pusa_j, index2, index1 );

        if(  castigator > 0 )
        {

            return castigator;

        }

        cine_muta_primul = 4;
        miscari++;

        if( index2 == 8 && index1 == 8 )
            break;

        if( index1 != index2 )
        {

            char eroare[] = "eroareee";

            setcolor(14);
            outtextxy(100, 100, eroare);

        }

    }

    //prima data verific daca un click.locatie e in afara

    return 0;
}

//sterge pozitiile hasurate, le eliminam statutul de blocuri in care se poate muta( facem din ele casute goale)
void stergere_pozitii(int &nr_de_locatii)
{

    for(unsigned int k = 0; k<nr_de_locatii; k++)
    {
        //le eliminam statutul de blocuri in care se poate muta( facem din ele casute goale)
        tabel[ pozitii_mutare[k].i ][ pozitii_mutare[k].j ].player = 0;

        //deseneaza locul gol
        desenare_loc_de_mutat( pozitii_mutare[k].i, pozitii_mutare[k].j,  1, 10 );

        pozitii_mutare[k].i = -1;
        pozitii_mutare[k].j = -1;
    }

    //daca exista o piesa anteriora evidentiata, o coloram normal la loc
    if( locatie_de_mutat.i>-1 )
    {

        setcolor( culoare_player[ tabel[locatie_de_mutat.i][locatie_de_mutat.j].player  ] );
        setfillstyle(1, culoare_player[ tabel[locatie_de_mutat.i][locatie_de_mutat.j].player  ] );
        fillellipse(tabel[locatie_de_mutat.i][locatie_de_mutat.j].x, tabel[locatie_de_mutat.i][locatie_de_mutat.j].y, latura/4, latura/4);

    }

    nr_de_locatii = 0;

}

void desenare_efect(int i, int j)
{

        setlinestyle(0, 0, 1);
        //desenez un cerc negru conturat cu linie alba
        setcolor(15);//contur alb
        setfillstyle(1, 0);//patern1, culoare negru
        fillellipse(tabel[i][j].x, tabel[i][j].y, latura/5, latura/5 );

        //desenez un cerc mai mic plin de culoarea playerului deasupra cercului de mai devreme si obtin un contur
        setcolor(culoare_player[  tabel[i][j].player  ]);
        setfillstyle(1, culoare_player[ tabel[i][j].player ]);
        fillellipse(tabel[i][j].x, tabel[i][j].y, latura/6, latura/6);

}

void stergere_efect( int i, int j )
{


    drawX( i, j, 12 );

    setcolor( culoare_player[ tabel[i][j].player ] );
    setfillstyle( 1, culoare_player[ tabel[i][j].player ] );
    fillellipse( tabel[i][j].x, tabel[i][j].y, latura/4, latura/4);


}

//cautam pozitii in care se poate muta o piesa selectata
bool adaugare_pozitii_in_care_se_poate_muta( int i, int j, int &nr_de_locatii )
{
//cautam pozitii in care se poate muta o piesa selectata
if( i<nr_linii )
    {

        if( tabel[ i+1 ][j].player == 0 )
            {
                pozitii_mutare[ nr_de_locatii ].i = i+1;
                pozitii_mutare[ nr_de_locatii ].j = j;

                tabel[ i+1 ][j].player = 3;

                desenare_loc_de_mutat(pozitii_mutare[ nr_de_locatii ].i, pozitii_mutare[ nr_de_locatii ].j, 3, culoare_player[tabel[i][j].player] );

                nr_de_locatii++;


            }

    }



    if( i>1 )
    {

        if( tabel[ i-1 ][j].player == 0 )
            {
                pozitii_mutare[ nr_de_locatii ].i = i-1;
                pozitii_mutare[ nr_de_locatii ].j = j;

                tabel[ i-1 ][j].player = 3;

                desenare_loc_de_mutat(pozitii_mutare[ nr_de_locatii ].i, pozitii_mutare[ nr_de_locatii ].j, 3, culoare_player[tabel[i][j].player] );

                nr_de_locatii++;

            }

    }



    if( j<nr_col )
    {


        if( tabel[i][ j+1 ].player == 0 )
            {
                pozitii_mutare[ nr_de_locatii ].i = i;
                pozitii_mutare[ nr_de_locatii ].j = j+1;

                tabel[i][ j+1 ].player = 3;

                desenare_loc_de_mutat(pozitii_mutare[ nr_de_locatii ].i, pozitii_mutare[ nr_de_locatii ].j, 3, culoare_player[tabel[i][j].player] );

                nr_de_locatii++;

            }

    }

    if( j>1 )
    {

        if( tabel[i][ j-1 ].player == 0 )
            {
                pozitii_mutare[ nr_de_locatii ].i = i;
                pozitii_mutare[ nr_de_locatii ].j = j-1;

                tabel[i][ j-1 ].player = 3;

                desenare_loc_de_mutat(pozitii_mutare[ nr_de_locatii ].i, pozitii_mutare[ nr_de_locatii ].j, 3, culoare_player[tabel[i][j].player] );

                nr_de_locatii++;

            }


    }

    //daca piesa nu se poate muta pe nicaieri
    if(nr_de_locatii == 0)
        {

            return false;
        }

    //daca am gasit niste pozitii in care se poate muta ste


    //memoram locatia piesei ce se poate muta
    locatie_de_mutat.i = i;
    locatie_de_mutat.j = j;


    //marcam ca piesa e selectata printr-un cerculet
    setcolor(15);
    setfillstyle(1, 15);
    fillellipse(tabel[i][j].x, tabel[i][j].y, latura/6, latura/6);

    return true;


}

void mutare( int i, int j, int &nr_de_locatii, int player )
{

    //piese_player are pe o anumita linie toate locatiile unde se afla piesele de tip player

    //in acest for introducem in acest vector o noua pozitie pe care va sta piesa care va fi mutata
    //inlocuind pozitia veche
       for( unsigned int k = 0; k<8; k++ )
        {

        if( piese_player[ player-1 ][k].i == locatie_de_mutat.i && piese_player[ player-1 ][k].j == locatie_de_mutat.j )
            {

            piese_player[player-1][k].i = i;
            piese_player[player-1][k].j = j;

            break;

            }

        }

    //golim casutele hasurate care nu ne mai trebuiesc si le lasam goale
    stergere_pozitii(nr_de_locatii);

    //marcam faptul ca poztiia noua a fost ocupata
    tabel[i][j].player = player;


    //desenam piesa in noua pozitie

    drawX( i, j, 12 );
    setcolor( culoare_player[player] );
    setfillstyle(1, culoare_player[player]);
    fillellipse(tabel[i][j].x, tabel[i][j].y, latura/4, latura/4);


    //golim spatiul din care o fost mutata piesa
    desenare_loc_de_mutat( locatie_de_mutat.i, locatie_de_mutat.j, 1, 10 );

    tabel[locatie_de_mutat.i][locatie_de_mutat.j].player = 0;



}

//spatii langa un grup ( daca are )
void adaugare_spatii_goale( int i, int j, int index_grup, locatie casute_goale[][100], int &nr_casute_goale )
{

    if( i<nr_linii )
        {

            if( tabel[ i+1 ][j].player == 0 && tabel[i+1][j].frec == 0 )
            {

                tabel[i+1][j].frec = 1;

                casute_goale[index_grup][ nr_casute_goale ].i = i+1;
                casute_goale[index_grup][ nr_casute_goale ].j = j;

                nr_casute_goale++;

            }else{
                    //marchez piesele mele care inconjoara deja grupul ca sa nu le mai i-au in calcul mai tarziu
                    if( tabel[ i+1 ][j].player == 2 )
                    {

                        tabel[ i+1 ][j].frec = 4;

                    }

                }




        }


    if( i>1 )
    {

            if( tabel[ i-1 ][j].player == 0 && tabel[i-1][j].frec == 0 )
            {

                tabel[i-1][j].frec = 1;
                casute_goale[index_grup][ nr_casute_goale ].i = i-1;
                casute_goale[index_grup][ nr_casute_goale ].j = j;
                nr_casute_goale++;

            }else{
                    //marchez piesele mele care inconjoara deja grupul ca sa nu le mai i-au in calcul mai tarziu
                    if( tabel[ i-1 ][j].player == 2 )
                    {

                        tabel[ i-1 ][j].frec = 4;

                    }

                }


    }


    if( j<nr_col )
    {


                if( tabel[i][ j+1 ].player == 0 && tabel[i][j+1].frec == 0 )
                {

                    tabel[i][j+1].frec = 1;
                    casute_goale[index_grup][ nr_casute_goale ].i = i;
                    casute_goale[index_grup][ nr_casute_goale ].j = j+1;
                    nr_casute_goale++;
                }else{
                    //marchez piesele mele care inconjoara deja grupul ca sa nu le mai i-au in calcul mai tarziu
                    if( tabel[i][ j+1 ].player == 2 )
                    {

                        tabel[i][ j+1 ].frec = 4;

                    }

                }


    }

    if( j>1 )
    {

            if(  tabel[i][ j-1 ].player == 0 && tabel[i][j-1].frec == 0 )
            {

                tabel[i][j-1].frec = 1;
                casute_goale[index_grup][ nr_casute_goale ].i = i;
                casute_goale[index_grup][ nr_casute_goale ].j = j-1;
                nr_casute_goale++;

            }else{
                    //marchez piesele mele care inconjoara deja grupul ca sa nu le mai i-au in calcul mai tarziu
                    if( tabel[i][ j-1 ].player == 2 )
                    {

                        tabel[i][ j-1 ].frec = 4;

                    }

                }



    }


}


//returneaza cate mutari are nevoie inamicul ca sa cucereasca grupurile dupa o mutare
int pericolul_miscarii( int sursa_i, int sursa_j, int dest_i, int dest_j )
{
    if( tabel[dest_i][dest_j].player > 0 )
    {
        cout<<"\n eroare destinatie player e 0 dest_i = "<<dest_i<<" dest_j= "<<dest_j<<"\n";
        return -5;
    }

    int index_in_piese_player=-1;

    for( unsigned int i = 0; i<8; i++ )
    {

        if( piese_player[1][i].i == sursa_i && piese_player[1][i].j == sursa_j )
        {
            index_in_piese_player = i;
            break;
        }

    }

    if( index_in_piese_player == -1 )
    {
        cout<<"\n indexul in piese player nu e \n";
        return -5;
    }

    //presupunem ca am aplicat miscarea
    tabel[dest_i][dest_j].player = 2;
    tabel[sursa_i][sursa_j].player = 0;

    piese_player[ 1 ][index_in_piese_player].i = dest_i;
    piese_player[ 1 ][index_in_piese_player].j = dest_j;



//testez vulnerabilitatile.......................................................

    //gasesc grupurile mele
    int nr_grupurile_mele2=0, nr_piesele_mele2[8]={0,0,0,0,0,0,0,0};
    locatie grupurile_mele2[8][8];

    setare_frecventa(0);
    for( unsigned int i = 0; i<8; i++ )
    {

        tabel[ piese_player[1][i].i ][ piese_player[1][i].j ].frec = 0;

    }

    //cout<<"\n pericol("<<sursa_i<<" "<<sursa_j<<", "<<dest_i<<" "<<dest_j<<" "<<")\n";


    //inregistram grupurile inamice
    for( unsigned int i = 0; i<8; i++ )
    {

        if( tabel[ piese_player[1][i].i ][ piese_player[1][i].j ].frec == 0 )
        {

            tabel[ piese_player[1][i].i ][ piese_player[1][i].j ].frec = 1;

            grupurile_mele2[nr_grupurile_mele2][ nr_piesele_mele2[nr_grupurile_mele2] ].i = piese_player[1][i].i;
            grupurile_mele2[nr_grupurile_mele2][ nr_piesele_mele2[nr_grupurile_mele2] ].j = piese_player[1][i].j;

            nr_piesele_mele2[nr_grupurile_mele2]++;

            gasire_grupuri( grupurile_mele2, nr_grupurile_mele2, nr_piesele_mele2, 8, 2, piese_player[1][i].i, piese_player[1][i].j  );


            nr_grupurile_mele2++;

        }


    }



    //gasesc spatii goale langa fiecare grup
    int nr_sp_goale_ale_mele2[8] = {0,0,0,0,0,0,0,0};
    locatie sp_goale_ale_mele2[8][100];


    int danger = -1;


    for( unsigned int i = 0; i<nr_grupurile_mele2; i++ )
    {
        setare_frecventa(0);

        //gasesc spatiile goale adiacente cu grupul i
        for( unsigned int j = 0; j<nr_piesele_mele2[i]; j++ )
        {

            int nr1=nr_sp_goale_ale_mele2[i];
            adaugare_spatii_goale(grupurile_mele2[i][j].i, grupurile_mele2[i][j].j, i, sp_goale_ale_mele2, nr1 );
            nr_sp_goale_ale_mele2[i] = nr1;
        }
        //am gasit aceste spatii adiacente cu grupul i.................................................................
    //cout<<"\n pericol("<<sursa_i<<" "<<sursa_j<<", "<<dest_i<<" "<<dest_j<<" "<<"), GRUPUL"<<i<<"***************************************************\n";


//        for( unsigned int j = 0; j<nr_piesele_mele2[i]; j++ )
//        {
//
//            cout<<grupurile_mele2[i][j].i<<" "<<grupurile_mele2[i][j].j<<"\n";
//
//        }


    //cout<<"Gata grup i pericol()....................................\n";

//cout<<"\n pericol("<<sursa_i<<" "<<sursa_j<<", "<<dest_i<<" "<<dest_j<<" "<<"), SPATII GOALE grupul"<<i<<"***************************************************\n";

    for( unsigned int j = 0; j<nr_sp_goale_ale_mele2[i]; j++ )
    {

        //cout<<sp_goale_ale_mele2[i][j].i<<" "<<sp_goale_ale_mele2[i][j].j<<"\n";

    }
    //cout<<"Gata SPATII GOALE grupul i pericol().........................................\n";



        if( nr_sp_goale_ale_mele2[i] == 1 )
        {

            //piesele deja adiacente cu grupul nu pot cuceri grupul fiindca lasa un spatiu gol in spate vecin cu grupul, le marchez cu frec=1
            setare_frecventa(0);
            for( unsigned int j = 0; j<nr_piesele_mele2[i]; j++ )
            {

                if( grupurile_mele2[i][0].j-1>=1 && tabel[ grupurile_mele2[i][0].i ][ grupurile_mele2[i][0].j-1 ].player == 1  )
                {

                    tabel[ grupurile_mele2[i][0].i ][ grupurile_mele2[i][0].j-1 ].frec = 1;

                }


                if( grupurile_mele2[i][0].j+1<=nr_col && tabel[ grupurile_mele2[i][0].i ][ grupurile_mele2[i][0].j+1 ].player == 1  )
                {

                    tabel[ grupurile_mele2[i][0].i ][ grupurile_mele2[i][0].j+1 ].frec = 1;

                }



                if( grupurile_mele2[i][0].i-1>=1 && tabel[ grupurile_mele2[i][0].i-1 ][ grupurile_mele2[i][0].j ].player == 1  )
                {

                    tabel[ grupurile_mele2[i][0].i-1 ][ grupurile_mele2[i][0].j ].frec = 1;

                }

                if( grupurile_mele2[i][0].i+1<=nr_linii && tabel[ grupurile_mele2[i][0].i + 1 ][ grupurile_mele2[i][0].j ].player == 1  )
                {

                    tabel[ grupurile_mele2[i][0].i + 1 ][ grupurile_mele2[i][0].j ].frec = 1;

                }



            }


            if( sp_goale_ale_mele2[i][0].j-1>=1 && tabel[ sp_goale_ale_mele2[i][0].i ][ sp_goale_ale_mele2[i][0].j-1 ].player == 1 && tabel[ sp_goale_ale_mele2[i][0].i ][ sp_goale_ale_mele2[i][0].j-1 ].frec == 0  )
            {
                tabel[sursa_i][sursa_j].player = 2;
                tabel[dest_i][dest_j].player = 0;

                piese_player[ 1 ][index_in_piese_player].i = sursa_i;
                piese_player[ 1 ][index_in_piese_player].j = sursa_j;
                return 0;

            }


            if( sp_goale_ale_mele2[i][0].j+1<=nr_col && tabel[ sp_goale_ale_mele2[i][0].i ][ sp_goale_ale_mele2[i][0].j+1 ].player == 1 && tabel[ sp_goale_ale_mele2[i][0].i ][ sp_goale_ale_mele2[i][0].j+1 ].frec == 0 )
            {
                tabel[sursa_i][sursa_j].player = 2;
                tabel[dest_i][dest_j].player = 0;

                piese_player[ 1 ][index_in_piese_player].i = sursa_i;
                piese_player[ 1 ][index_in_piese_player].j = sursa_j;

                return 0;
            }



            if( sp_goale_ale_mele2[i][0].i-1>=1 && tabel[ sp_goale_ale_mele2[i][0].i-1 ][ sp_goale_ale_mele2[i][0].j ].player == 1 && tabel[ sp_goale_ale_mele2[i][0].i-1 ][ sp_goale_ale_mele2[i][0].j ].frec == 0  )
            {
                tabel[sursa_i][sursa_j].player = 2;
                tabel[dest_i][dest_j].player = 0;

                piese_player[ 1 ][index_in_piese_player].i = sursa_i;
                piese_player[ 1 ][index_in_piese_player].j = sursa_j;
                return 0;

            }

            if( sp_goale_ale_mele2[i][0].i+1<=nr_linii && tabel[ sp_goale_ale_mele2[i][0].i + 1 ][ sp_goale_ale_mele2[i][0].j ].player == 1 && tabel[ sp_goale_ale_mele2[i][0].i + 1 ][ sp_goale_ale_mele2[i][0].j ].frec == 0 )
            {
                tabel[sursa_i][sursa_j].player = 2;
                tabel[dest_i][dest_j].player = 0;

                piese_player[ 1 ][index_in_piese_player].i = sursa_i;
                piese_player[ 1 ][index_in_piese_player].j = sursa_j;

                return 0;
            }




            int max_miscari_pana_la_inamic = 0;

            if( sp_goale_ale_mele2[i][0].i < nr_linii )
            {

                int vab = sp_goale_ale_mele2[i][0].i;
                int pasi = 0;


                while( vab < nr_linii  )
                {

                    if( tabel[ vab + 1 ][ sp_goale_ale_mele2[i][0].j ].player == 1 && tabel[ vab + 1 ][ sp_goale_ale_mele2[i][0].j ].frec == 0 )
                    {
                        break;

                    }else{

                        pasi++;

                    }

                    vab++;

                }

                if( pasi == 0 )
                {
                    //refacem ce-am modificat
                    tabel[sursa_i][sursa_j].player = 2;
                    tabel[dest_i][dest_j].player = 0;

                    piese_player[ 1 ][index_in_piese_player].i = sursa_i;
                    piese_player[ 1 ][index_in_piese_player].j = sursa_j;

                    return 0;

                }else max_miscari_pana_la_inamic = pasi;


            }






            if( sp_goale_ale_mele2[i][0].i > 1 )
            {

                int vab = sp_goale_ale_mele2[i][0].i;
                int pasi = 0;

                while( vab > 1  )
                {

                    if( tabel[ vab - 1 ][ sp_goale_ale_mele2[i][0].j ].player == 1 && tabel[ vab - 1 ][ sp_goale_ale_mele2[i][0].j ].frec == 0 )
                    {

                        break;

                    }else pasi++;

                    vab--;

                }
                if( pasi == 0 )
                {

                    //refacem ce-am modificat
                    tabel[sursa_i][sursa_j].player = 2;
                    tabel[dest_i][dest_j].player = 0;

                    piese_player[ 1 ][index_in_piese_player].i = sursa_i;
                    piese_player[ 2 ][index_in_piese_player].j = sursa_j;

                    return 0;

                }else if( max_miscari_pana_la_inamic < pasi ) max_miscari_pana_la_inamic = pasi;

            }



            if( sp_goale_ale_mele2[i][0].j < nr_col )
            {

                int vab = sp_goale_ale_mele2[i][0].j;
                int pasi = 0;
                while( vab < nr_col  )
                {

                    if( tabel[ sp_goale_ale_mele2[i][0].i][ vab + 1 ].player == 1 && tabel[ sp_goale_ale_mele2[i][0].i][ vab + 1 ].frec == 0  )
                    {


                    }else pasi++;

                    vab++;

                }

                if( pasi == 0 )
                {
                    //refacem ce-am modificat
                    tabel[sursa_i][sursa_j].player = tabel[dest_i][dest_j].player;
                    tabel[dest_i][dest_j].player = 0;

                    piese_player[ 1 ][index_in_piese_player].i = sursa_i;
                    piese_player[ 1 ][index_in_piese_player].j = sursa_j;

                    return 0;

                }else if( max_miscari_pana_la_inamic < pasi ) max_miscari_pana_la_inamic = pasi;


            }


            if( sp_goale_ale_mele2[i][0].j > 1 )
            {

                int vab = sp_goale_ale_mele2[i][0].j;
                int pasi = 0;

                while( vab > 1 )
                {

                    if( tabel[ sp_goale_ale_mele2[i][0].i][ vab - 1 ].player == 1 && tabel[ sp_goale_ale_mele2[i][0].i][ vab - 1 ].frec == 0 )
                    {

                        break;

                    }else pasi++;

                    vab--;

                }
                if( pasi == 0 ){

                    //refacem ce-am modificat
                    tabel[sursa_i][sursa_j].player = tabel[dest_i][dest_j].player;
                    tabel[dest_i][dest_j].player = 0;

                    piese_player[ 1 ][index_in_piese_player].i = sursa_i;
                    piese_player[ 1 ][index_in_piese_player].j = sursa_j;

                    return 0;

                }else if( max_miscari_pana_la_inamic < pasi ) max_miscari_pana_la_inamic = pasi;

            }


            danger += max_miscari_pana_la_inamic;


        }

    }


    //refacem ce-am modificat
    tabel[sursa_i][sursa_j].player = 2;
    tabel[dest_i][dest_j].player = 0;

    piese_player[ 1 ][index_in_piese_player].i = sursa_i;
    piese_player[ 1 ][index_in_piese_player].j = sursa_j;

    if(danger > -1)
        return danger;
    else return 100;

}


int calculare_distanta_in_miscari( int piesa_i, int piesa_j, int loc_gol_i, int loc_gol_j, int &poz_i, int &poz_j )
{
    poz_i = -3;
    poz_j = -3;

    if( piesa_i < loc_gol_i )
    {
        if( tabel[ piesa_i + 1 ][piesa_j].player == 0 )
        {
            poz_i = piesa_i + 1;
            poz_j = piesa_j;
        }

    }

    if( piesa_i > loc_gol_i )
    {

             if( tabel[ piesa_i - 1 ][piesa_j].player == 0 )
             {
                 poz_i = piesa_i - 1;
                 poz_j = piesa_j;
             }

    }

    if( piesa_j > loc_gol_j )
    {
        if( tabel[ piesa_i ][piesa_j-1].player == 0 )
        {
            poz_i = piesa_i;
            poz_j = piesa_j - 1;
        }
    }


    if( piesa_j < loc_gol_j )
    {
        if( tabel[ piesa_i ][piesa_j+1].player == 0 )
        {
            poz_i = piesa_i;
            poz_j = piesa_j + 1;
        }
    }

    //inseamna ca piesa e blocata de ceva si nu poate merge unde trebuie,
    // atunci incercam sa ocolim


    if( poz_i == -3 )
    {

            //caut un grupuri goale ale destinatiei si daca nicinul nu-i vecin cu i,j atunci nu se poate muta intr-acolo

        bool spatiu_inzapezit = true;

        if( loc_gol_j-1>=1 && tabel[ loc_gol_i ][ loc_gol_j-1 ].player == 0  )
        {
            spatiu_inzapezit = false;

        }


        if( loc_gol_j+1<=nr_col && tabel[ loc_gol_i ][ loc_gol_j+1 ].player == 0  )
        {
            spatiu_inzapezit = false;
        }


        if( loc_gol_i-1>=1 && tabel[ loc_gol_i-1 ][ loc_gol_j ].player == 0  )
        {

            spatiu_inzapezit = false;
        }

        if( loc_gol_i+1<=nr_linii && tabel[ loc_gol_i + 1 ][ loc_gol_j ].player == 0  )
        {

            spatiu_inzapezit = false;

        }

        if( spatiu_inzapezit )
        {

            cout<<"\n SPATIU GOL:" <<loc_gol_i<<" "<<loc_gol_j<<" INTR-O INSULA NEINCIDENTA CU PIESA DE MUTAT:"<<piesa_i<<" "<<piesa_j<<"\n";
            return -1;

        }



    }

    while( poz_i == -3 )
    {





        if( piesa_j-1>=1 && tabel[ piesa_i ][ piesa_j-1 ].player == 0  )
        {
            poz_i = piesa_i;
            poz_j = piesa_j-1;
            break;
        }


        if( piesa_j+1<=nr_col && tabel[ piesa_i ][ piesa_j+1 ].player == 0  )
        {
            poz_i = piesa_i;
            poz_j = piesa_j+1;break;
        }



        if( piesa_i-1>=1 && tabel[ piesa_i-1 ][ piesa_j ].player == 0  )
        {
            poz_i = piesa_i - 1;
            poz_j = piesa_j;break;

        }

        if( piesa_i+1<=nr_linii && tabel[ piesa_i + 1 ][ piesa_j ].player == 0  )
        {
            poz_i = piesa_i + 1;
            poz_j = piesa_j;
            break;

        }

        cout<<"while mergeee";


    }

    if( poz_i == loc_gol_i && poz_j == loc_gol_j )
        return 1;

    int nr = 0;

    if( poz_j > loc_gol_j )
    {

        for( unsigned int j = poz_j - 1; j>=loc_gol_j; j--  )
        {

            if( tabel[loc_gol_i][j].player > 0 )
                nr+=3;
            else nr+=1;

        }

    }else{

        for( unsigned int j = poz_j + 1; j<=loc_gol_j; j++ )
            if( tabel[loc_gol_i][j].player > 0)
                nr+=3;
            else nr+=1;
    }


    if( poz_i > loc_gol_i )
    {

        for( unsigned int i = poz_i - 1; i>=loc_gol_i; i--  )
        {

            if( tabel[i][piesa_j].player > 0 )
            {
                nr+=3;

            }else nr+=1;

        }

    }else{

        for( unsigned int i = poz_i + 1; i<=loc_gol_i; i++ )
        {
            if(tabel[i][piesa_j].player > 0)
            {
                nr+=3;
            }else nr+=1;
        }

    }


    return nr;

}


void plasare_piesa_computer_runda2( int &linia, int &coloana )
{

    //gasesc grupuri inamice
    int nr_grupuri_inamice=0, nr_piese_inamice[8]={0,0,0,0,0,0,0,0};
    locatie grupuri_inamice[8][8];

    for( unsigned int i = 0; i<8; i++ )
    {

        tabel[ piese_player[0][i].i ][ piese_player[0][i].j ].frec = 0;

    }


//INREGISTRAM GRUPURILE INAMICE##############################################################################################################################
    for( unsigned int i = 0; i<8; i++ )
    {

        if( tabel[ piese_player[0][i].i ][ piese_player[0][i].j ].frec == 0 )
        {

            tabel[ piese_player[0][i].i ][ piese_player[0][i].j ].frec = 1;

            grupuri_inamice[nr_grupuri_inamice][ nr_piese_inamice[nr_grupuri_inamice] ].i = piese_player[0][i].i;
            grupuri_inamice[nr_grupuri_inamice][ nr_piese_inamice[nr_grupuri_inamice] ].j = piese_player[0][i].j;

            nr_piese_inamice[nr_grupuri_inamice]++;

            gasire_grupuri( grupuri_inamice, nr_grupuri_inamice, nr_piese_inamice, 8, 1, piese_player[0][i].i, piese_player[0][i].j  );


            nr_grupuri_inamice++;

        }

    }
//GATA INREGISTRAT GRUPURILE INAMICE................................................................................................INREGISTRAT GRUPURILE INAMICE




    cout<<'\n'<<".........................RUNDA_2_MUTARE NOUA...............................................................................................................";
    cout<<'\n'<<"grupuri inamice:\n";

    for( unsigned int i = 0; i<nr_grupuri_inamice; i++ )
    {
        cout<<"Grup "<<i<<'\n';
        for(unsigned int j  = 0; j<nr_piese_inamice[i]; j++)
        {
            cout<<grupuri_inamice[i][j].i<<" "<<grupuri_inamice[i][j].j<<'\n';
        }

    }

    cout<<"..............gata...................\n";


    //gasesc spatii goale langa fiecare grup
    int nr_sp_goale[8] = {0,0,0,0,0,0,0,0};
    locatie sp_goale[8][100];

    int nr_miscari_grup[8]={0,0,0,0,0,0,0,0}, index_grup_ales = 0;
    locatie poz_de_mutat_grup[8], poz_in_care_se_muta_grup[8];


//i-au FIECARE GRUP si ii gasesc nr_miscari si toate cele#########################################################################
    for( unsigned int i = 0; i<nr_grupuri_inamice; i++ )
    {

        setare_frecventa(0);


        //gasesc spatiile goale adiacente cu grupul i
        for( unsigned int j = 0; j<nr_piese_inamice[i]; j++ )
        {

            int nr1=nr_sp_goale[i];
            adaugare_spatii_goale(grupuri_inamice[i][j].i, grupuri_inamice[i][j].j, i, sp_goale, nr1 );
            nr_sp_goale[i] = nr1;
        }
//GATA spatii adiacente cu grupul i...............................................................................................




        cout<<'\n'<<"SPATII GOALE GRUP "<<i<<'\n';
        for( unsigned int ind = 0; ind<nr_sp_goale[i]; ind++ )
        {

            cout<<sp_goale[i][ind].i<<" "<<sp_goale[i][ind].j<<'\n';

        }

        cout<<"..............gata_spatii_goale....................................\n";




//prima data vad daca am destule piese ca sa inconjor grupul###########################################################
        int nr_piese_mutabile = 0;

        for( unsigned int k = 0; k<8; k++ )
        {

            if( tabel[ piese_player[1][k].i ][ piese_player[1][k].j ].frec == 0 )
            {

                bool mut = false;

                //alea care sunt blocate de piesele lor
                if( piese_player[1][k].j-1>=1 && tabel[ piese_player[1][k].i ][ piese_player[1][k].j-1 ].player == 0  )
                {

                    mut = true;

                }


                if( piese_player[1][k].j+1<=nr_col && tabel[ piese_player[1][k].i ][ piese_player[1][k].j+1 ].player == 0  )
                {
                    mut = true;
                }



                if( piese_player[1][k].i-1>=1 && tabel[ piese_player[1][k].i-1 ][ piese_player[1][k].j ].player == 0  )
                {
                    mut = true;

                }

                if( piese_player[1][k].i+1<=nr_linii && tabel[ piese_player[1][k].i + 1 ][ piese_player[1][k].j ].player == 0  )
                {

                    mut = true;
                }

                if(mut)
                    nr_piese_mutabile +=1;
                else tabel[ piese_player[1][k].i ][ piese_player[1][k].j ].frec = 2;
            }

        }

        cout<<"\n piese mutabile pt grupul "<<i<<": "<<nr_piese_mutabile<<'\n';


        //daca n-am destule piese cat sa acopar spatiile goale atunci I dismiss it
        if( nr_piese_mutabile < nr_sp_goale[i] )
        {

            nr_miscari_grup[i] = -2;


            cout<<"\n grupul "<<i<<" are prea putine piese mutabile \n";

            continue;
        }
//GATA.................................................................................................................





        int sp_goale_frec[100];

        for( unsigned int k = 0; k<nr_sp_goale[i]; k++ )
        {
            sp_goale_frec[k] = 0;
        }


//alegerea miscarii propriu-zise ( fiecare grup are cate o miscare corespunzatoare - adica piesa de mutat si loc in care se muta - )
//care e luata din cel mai scurt traseu spatiu_gol - piesa_mutabila

        int distanta = -1;
        int index_spatiu_gol_ales=-1;


    //caut spatiul gol cel mai bun in combinatie o piesa care merge intr-acolo
        for( unsigned int j = 0; j<nr_sp_goale[i]; j++ )
        {

            for( unsigned int k = 0; k<8; k++ )
            {

                //vad distanta dintre spatiul gol j si fiecare piesa mutabila
                if( tabel[ piese_player[1][k].i ][ piese_player[1][k].j ].frec == 0 )
                {

                    //aici retin pozitia in care am vrea sa mutam piesa selectata
                    int poz_i=-1, poz_j=-1;

                    int dist2 = calculare_distanta_in_miscari( piese_player[1][k].i, piese_player[1][k].j, sp_goale[i][j].i, sp_goale[i][j].j, poz_i, poz_j );

                    if( dist2 > -1 )
                    {

                        if( dist2 < distanta || distanta == -1 )
                        {
                            distanta = dist2;

                            index_spatiu_gol_ales = j;

                            poz_de_mutat_grup[i].i = piese_player[1][k].i;
                            poz_de_mutat_grup[i].j = piese_player[1][k].j;

                            poz_in_care_se_muta_grup[i].i = poz_i;
                            poz_in_care_se_muta_grup[i].j = poz_j;

                        }


                    }


                }

            }

        }

        //nu toate piesele mutabile sunt mutabile catre pozitia goala
        if(index_spatiu_gol_ales == -1 )
            {
                nr_miscari_grup[i] = -2;

                cout<<"\n pozitie de mutat: nu avem"<<'\n';

                continue;
            }

        sp_goale_frec[index_spatiu_gol_ales] = 1;
        nr_miscari_grup[i] += distanta;
        tabel[ poz_de_mutat_grup[i].i ][ poz_de_mutat_grup[i].j ].frec = 1;


        cout<<"\n spatiul gol ales ca target: "<<sp_goale[i][index_spatiu_gol_ales].i<<" "<<sp_goale[i][index_spatiu_gol_ales].j<<'\n';
        cout<<"\n pozitie de mutat:"<<poz_de_mutat_grup[i].i<<" "<<poz_de_mutat_grup[i].j<<'\n';
        cout<<"\n pozitie in care se muta:"<<poz_in_care_se_muta_grup[i].i<<" "<<poz_in_care_se_muta_grup[i].j<<'\n';





//gasirea numarului de miscari necesare pt ca grupul inamic sa fie inconjurat, se retine pt fiecare grup inamic
//dupa criteriul asta alegem miscarea finala
        bool gata = false;

        if( nr_sp_goale[i] > 1 )
        {

                 do{

                    index_spatiu_gol_ales = -1;
                    int dist = 300;
                    int index_piesa_i, index_piesa_j;

                    //vezi ca am marcat piesele mele deja adiacente cu grupul

                    //i-au fiecare spatiu gol disponibil
                    //gasesc cel mai bun traseu piesa mutabila(disponibila) - spatiu gol
                    //dupa retin piesa de mutat, locul unde o mut si distanta
                    for( unsigned int j = 0; j<nr_sp_goale[i]; j++ )
                    {

                        if( sp_goale_frec[j] > 0 )
                            continue;




                        for( unsigned int k = 0; k<8; k++ )
                        {

                            if( tabel[ piese_player[1][k].i ][ piese_player[1][k].j ].frec == 0 )
                            {

                                    int poz_i, poz_j;

                                    int dist2 = calculare_distanta_in_miscari( piese_player[1][k].i, piese_player[1][k].j, sp_goale[i][j].i, sp_goale[i][j].j, poz_i, poz_j );

                                    if( dist2 > -1 )
                                    {

                                        if( dist2 < dist )
                                        {
                                            dist = dist2;

                                            index_spatiu_gol_ales = j;

                                            index_piesa_i = piese_player[1][k].i;
                                            index_piesa_j = piese_player[1][k].j;

                                        }

                                    }


                            }

                        }

                    }

                    if( index_spatiu_gol_ales>-1 )
                    {
                        sp_goale_frec[index_spatiu_gol_ales] = 1;
                        nr_miscari_grup[i] += dist;
                        tabel[ index_piesa_i ][ index_piesa_j ].frec = 1;
                    }else{

                        nr_miscari_grup[i] = -2;

                        cout<<"\n grupul "<<i<<" nu are o strategie pt a umple spatiile goale \n";

                        break;
                    }


                    gata = true;
                    for( unsigned int k = 0; k<nr_sp_goale[i]; k++ )
                        {
                            if( sp_goale_frec[k] == 0 )
                            {
                                gata = false;
                                break;
                            }
                        }


                }while(!gata);

        }

        //daca am gasit o singura miscare direct castigatoare
        if( nr_miscari_grup[i] == 1 )
        {

            linia = poz_in_care_se_muta_grup[i].i;
            coloana = poz_in_care_se_muta_grup[i].j;


            locatie_de_mutat.i = poz_de_mutat_grup[i].i;
            locatie_de_mutat.j = poz_de_mutat_grup[i].j;

            cout<<"\n piesa din AM GASIT O PIESA CASTIGATOARE line 2157**********************************************\n";

            return;
        }

    }



//vedem daca avem grup expus ca sa vedem daca facem o miscare de SALVARE#######################################################################################

    //gasesc grupurile mele################################################################################################################
    int nr_grupurile_mele=0, nr_piesele_mele[8]={0,0,0,0,0,0,0,0};
    locatie grupurile_mele[8][8];

    for( unsigned int i = 0; i<8; i++ )
    {

        tabel[ piese_player[1][i].i ][ piese_player[1][i].j ].frec = 0;

    }




    for( unsigned int i = 0; i<8; i++ )
    {

        if( tabel[ piese_player[1][i].i ][ piese_player[1][i].j ].frec == 0 )
        {

            tabel[ piese_player[1][i].i ][ piese_player[1][i].j ].frec = 1;

            grupurile_mele[nr_grupurile_mele][ nr_piesele_mele[nr_grupurile_mele] ].i = piese_player[1][i].i;
            grupurile_mele[nr_grupurile_mele][ nr_piesele_mele[nr_grupurile_mele] ].j = piese_player[1][i].j;

            nr_piesele_mele[nr_grupurile_mele]++;

            gasire_grupuri( grupurile_mele, nr_grupurile_mele, nr_piesele_mele, 8, 2, piese_player[1][i].i, piese_player[1][i].j  );


            nr_grupurile_mele++;

        }

    }
    //GATA gasire grupurile mele...................................................................................................................



    cout<<'\n'<<"grupurile MELE: \n";

    for( unsigned int i = 0; i<nr_grupurile_mele; i++ )
    {
        cout<<"Grup "<<i<<'\n';
        for(unsigned int j  = 0; j<nr_piesele_mele[i]; j++)
        {
            cout<<grupurile_mele[i][j].i<<" "<<grupurile_mele[i][j].j<<'\n';
        }

    }

    cout<<"..............gata Grupurile MELE...................\n";






    //aici vrea sa memorez vreun grup care e mai atacat ca atunci cand
    //ca atunci cand numai am o strategie de atac sa am o mutare buna, MUTARI STRONG PT FIECARE GRUP
    locatie mutare_strong_piesa_de_mutat[8][4], mutare_strong_locatie_in_care_se_muta[8];
    int mutare_strong_piesa_de_mutat_NR[8] = {0,0,0,0,0,0,0,0};
    //...................................................................................






    //gasesc spatii goale langa fiecare grup
    int nr_sp_goale_ale_mele[8] = {0,0,0,0,0,0,0,0};
    locatie sp_goale_ale_mele[8][100];

    for( unsigned int i = 0; i<nr_grupurile_mele; i++ )
    {
        setare_frecventa(0);

        //gasesc spatiile goale adiacente cu grupul i
        for( unsigned int j = 0; j<nr_piesele_mele[i]; j++ )
        {

            int nr1=nr_sp_goale_ale_mele[i];
            adaugare_spatii_goale(grupurile_mele[i][j].i, grupurile_mele[i][j].j, i, sp_goale_ale_mele, nr1 );
            nr_sp_goale_ale_mele[i] = nr1;
        }
        //am gasit aceste spatii adiacente cu grupul i.................................................................



        cout<<'\n'<<"SPATII GOALE GRUPUL MEU "<<i<<'\n';
        for( unsigned int ind = 0; ind<nr_sp_goale_ale_mele[i]; ind++ )
        {

            cout<<sp_goale_ale_mele[i][ind].i<<" "<<sp_goale_ale_mele[i][ind].j<<'\n';

        }

        cout<<"..............gata_spatii_goale....................................\n";






            //initializam cu -1 mutarile "strong", care nu vor fi neaparat folosite
            mutare_strong_locatie_in_care_se_muta[i].i = -1;
            mutare_strong_locatie_in_care_se_muta[i].j = -1;
            //......................................................................................




        if( nr_sp_goale_ale_mele[i] == 1 )
        {

            linia = sp_goale_ale_mele[i][0].i;
            coloana = sp_goale_ale_mele[i][0].j;


            setare_frecventa(0);
            //daca o piesa e deja adiacent cu grupul atunci va lasa un spatiu gol langa grup si nu va inconjura grupul chiar daca ocupa spatiul gol[0]
            for( unsigned int j = 0; j<nr_piesele_mele[i]; j++ )
            {

                if( grupurile_mele[i][j].j-1>=1 && tabel[ grupurile_mele[i][j].i ][ grupurile_mele[i][j].j-1 ].player == 1  )
                {

                    tabel[ grupurile_mele[i][j].i ][ grupurile_mele[i][j].j-1 ].frec = 9;

                }


                if( grupurile_mele[i][j].j+1<=nr_col && tabel[ grupurile_mele[i][j].i ][ grupurile_mele[i][j].j+1 ].player == 1  )
                {
                    tabel[ grupurile_mele[i][j].i ][ grupurile_mele[i][j].j+1 ].frec = 9;
                }



                if( grupurile_mele[i][j].i-1>=1 && tabel[ grupurile_mele[i][j].i-1 ][ grupurile_mele[i][j].j ].player == 1  )
                {
                    tabel[ grupurile_mele[i][j].i - 1 ][ grupurile_mele[i][j].j ].frec = 9;

                }

                if( grupurile_mele[i][j].i+1<=nr_linii && tabel[ grupurile_mele[i][j].i + 1 ][ grupurile_mele[i][j].j ].player == 1  )
                {

                    tabel[ grupurile_mele[i][j].i + 1 ][ grupurile_mele[i][j].j].frec = 9;
                }

            }

            //retinem in orice caz, poate vom muta ceva aici chiar daca nu e amenintata indeaproape
            mutare_strong_locatie_in_care_se_muta[i].i = sp_goale_ale_mele[i][0].i;
            mutare_strong_locatie_in_care_se_muta[i].j = sp_goale_ale_mele[i][0].j;
            //......................................................................................



            //o folosesc doar in caz de nevoie
            locatie piesa_salvatoare;
            bool pericol = false;

            if( sp_goale_ale_mele[i][0].i < nr_linii  )
            {

                //daca e inamic tre sa verific daca nu e deja adiacent cu grupul pt ca daca e atunci va lasa un spatiu gol langa grup si nu-l va acoperi
                if( tabel[ sp_goale_ale_mele[i][0].i + 1 ][ sp_goale_ale_mele[i][0].j ].player == 1 && tabel[ sp_goale_ale_mele[i][0].i + 1 ][ sp_goale_ale_mele[i][0].j ].frec == 0 )
                {
                    //pericolul e real!!! avem o piesa inamica gata sa ne inchidda
                    pericol = true;


                }else{


                        if( tabel[ sp_goale_ale_mele[i][0].i + 1 ][ sp_goale_ale_mele[i][0].j ].player == 2  )
                        {

                            piesa_salvatoare.i = sp_goale_ale_mele[i][0].i + 1;
                            piesa_salvatoare.j = sp_goale_ale_mele[i][0].j;


                            mutare_strong_piesa_de_mutat[i][ mutare_strong_piesa_de_mutat_NR[i] ].i = sp_goale_ale_mele[i][0].i + 1;
                            mutare_strong_piesa_de_mutat[i][ mutare_strong_piesa_de_mutat_NR[i] ].j = sp_goale_ale_mele[i][0].j;
                            mutare_strong_piesa_de_mutat_NR[i]++;

                        }


                }

            }


            if( sp_goale_ale_mele[i][0].i > 1  )
            {

                //daca e inamic tre sa verific daca nu e deja adiacent cu grupul pt ca daca e atunci va lasa un spatiu gol langa grup si nu-l va acoperi
                if( tabel[ sp_goale_ale_mele[i][0].i - 1 ][ sp_goale_ale_mele[i][0].j ].player == 1 && tabel[ sp_goale_ale_mele[i][0].i - 1 ][ sp_goale_ale_mele[i][0].j ].frec == 0 )
                {
                    //pericolul e real!!! avem o piesa inamica gata sa ne inchidda
                    pericol = true;


                }else{



                        if( tabel[ sp_goale_ale_mele[i][0].i - 1 ][ sp_goale_ale_mele[i][0].j ].player == 2  )
                        {
                            piesa_salvatoare.i = sp_goale_ale_mele[i][0].i - 1;
                            piesa_salvatoare.j = sp_goale_ale_mele[i][0].j;


                            mutare_strong_piesa_de_mutat[i][ mutare_strong_piesa_de_mutat_NR[i] ].i = sp_goale_ale_mele[i][0].i - 1;
                            mutare_strong_piesa_de_mutat[i][ mutare_strong_piesa_de_mutat_NR[i] ].j = sp_goale_ale_mele[i][0].j;
                            mutare_strong_piesa_de_mutat_NR[i]++;
                        }


                }

            }

            if( sp_goale_ale_mele[i][0].j < nr_col  )
            {

                //daca e inamic tre sa verific daca nu e deja adiacent cu grupul pt ca daca e atunci va lasa un spatiu gol langa grup si nu-l va acoperi
                if( tabel[ sp_goale_ale_mele[i][0].i][ sp_goale_ale_mele[i][0].j + 1 ].player == 1 && tabel[ sp_goale_ale_mele[i][0].i][ sp_goale_ale_mele[i][0].j + 1 ].frec == 0  )
                {
                    //pericolul e real!!! avem o piesa inamica gata sa ne inchidda
                    pericol = true;


                }else{




                        if( tabel[ sp_goale_ale_mele[i][0].i][ sp_goale_ale_mele[i][0].j + 1 ].player == 2  )
                        {
                            piesa_salvatoare.i = sp_goale_ale_mele[i][0].i;
                            piesa_salvatoare.j = sp_goale_ale_mele[i][0].j + 1;

                            mutare_strong_piesa_de_mutat[i][ mutare_strong_piesa_de_mutat_NR[i] ].i = sp_goale_ale_mele[i][0].i;
                            mutare_strong_piesa_de_mutat[i][ mutare_strong_piesa_de_mutat_NR[i] ].j = sp_goale_ale_mele[i][0].j + 1;
                            mutare_strong_piesa_de_mutat_NR[i]++;

                        }


                }

            }

            if( sp_goale_ale_mele[i][0].j > 1 )
            {

                //daca e inamic tre sa verific daca nu e deja adiacent cu grupul pt ca daca e atunci va lasa un spatiu gol langa grup si nu-l va acoperi
                if( tabel[ sp_goale_ale_mele[i][0].i][ sp_goale_ale_mele[i][0].j - 1 ].player == 1 && tabel[ sp_goale_ale_mele[i][0].i][ sp_goale_ale_mele[i][0].j - 1 ].frec == 0  )
                {
                    //pericolul e real!!! avem o piesa inamica gata sa ne inchidda
                    pericol = true;


                }else{



                        if( tabel[ sp_goale_ale_mele[i][0].i][ sp_goale_ale_mele[i][0].j - 1 ].player == 2  )
                        {
                            piesa_salvatoare.i = sp_goale_ale_mele[i][0].i;
                            piesa_salvatoare.j = sp_goale_ale_mele[i][0].j - 1;

                            mutare_strong_piesa_de_mutat[i][ mutare_strong_piesa_de_mutat_NR[i] ].i = sp_goale_ale_mele[i][0].i;
                            mutare_strong_piesa_de_mutat[i][ mutare_strong_piesa_de_mutat_NR[i] ].j = sp_goale_ale_mele[i][0].j - 1;
                            mutare_strong_piesa_de_mutat_NR[i]++;

                        }


                }

            }


            if( pericol )
            {


                locatie_de_mutat.i = piesa_salvatoare.i;
                locatie_de_mutat.j = piesa_salvatoare.j;

                cout<<"\n e PERICOL, avem piesa salvatoare ********************************************** \n";

                return;
            }


        }

    }


//gata cu miscarea de salvare................................................................................................................................................


    cout<<"\n MISCARI DE ATAC: \n";
    for( unsigned int i = 0; i<nr_grupuri_inamice; i++ )
    {

        if( nr_miscari_grup[i] > 1 )
        {
            cout<<"nr_miscari: "<<nr_miscari_grup[i]<<" ";
            cout<<"pozitie de mutat: "<<poz_de_mutat_grup[i].i<<" "<<poz_de_mutat_grup[i].j<<" pozitie in care se muta: "<<poz_in_care_se_muta_grup[i].i<<" "<<poz_in_care_se_muta_grup[i].j<<"\n";


        }

    }
    cout<<" gata miscari de atac \n";


    cout<<"\n PERICOL GRAD DE 0 MISCARI: \n";
    //prima data vad daca vreo miscare de atac a unui grup MA PUNE IN VULNERABILITATE TOTALA
    for( unsigned int i = 0; i<nr_grupuri_inamice; i++ )
    {

        if( nr_miscari_grup[i] > 1  )
        {

            if( pericolul_miscarii( poz_de_mutat_grup[i].i, poz_de_mutat_grup[i].j, poz_in_care_se_muta_grup[i].i, poz_in_care_se_muta_grup[i].j ) == 0 )
            {

                nr_miscari_grup[i] = -1;
                cout<<"pozitie de mutat: "<<poz_de_mutat_grup[i].i<<" "<<poz_de_mutat_grup[i].j<<" pozitie in care se muta: "<<poz_in_care_se_muta_grup[i].i<<" "<<poz_in_care_se_muta_grup[i].j<<"\n";
            }

        }

    }
    cout<<" gata pericol miscari \n";


    //aleg acuma cea mai avantajoasa miscare care are ca taget un grup anume
    int min = 1000;
    int index_bun = -1;

    for( unsigned int i = 0; i<nr_grupuri_inamice; i++ )
    {

        if( nr_miscari_grup[i] > 0 && nr_miscari_grup[i] < min )
        {

            min = nr_miscari_grup[i];
            index_bun = i;

        }

    }

if(index_bun>-1)
{
        cout<<"\n e setat din index_bun line 2386********************************************** \n";
        linia = poz_in_care_se_muta_grup[index_bun].i;
        coloana = poz_in_care_se_muta_grup[index_bun].j;


        locatie_de_mutat.i = poz_de_mutat_grup[index_bun].i;
        locatie_de_mutat.j = poz_de_mutat_grup[index_bun].j;

}else{

        cout<<"\n index_bun nu e bun, nu avem nicio miscare \n";





//alegem o miscare "STRONG" care nu ma baga in vulnerabilitate totala ( defapt acum aflam cat de strong e )
//****miscare "STRONG" e o miscare prin care facem ca un grup cu un singur spatiu de umplut dar care nu poate fi cucerit direct
// isi acopera spatiu gol cu o piesa vea pt a se apara****
cout<<"\n PERICOL MISCARI ****STRONG*****: \n";
            int max_miscari_pana_cucereste_inamicul = 0, i_exp = -1, j_exp = -1;
           for( unsigned int i = 0; i<nr_grupurile_mele; i++ )
           {

               if( mutare_strong_locatie_in_care_se_muta[i].i > 0 )
               {


                   for( unsigned int j = 0; j<mutare_strong_piesa_de_mutat_NR[i]; j++ )
                   {


                        int danger = pericolul_miscarii(mutare_strong_piesa_de_mutat[i][j].i, mutare_strong_piesa_de_mutat[i][j].j, mutare_strong_locatie_in_care_se_muta[i].i, mutare_strong_locatie_in_care_se_muta[i].j);

                cout<<"PERICOL "<<danger<<" pozitie de mutat: "<<mutare_strong_piesa_de_mutat[i][j].i<<" "<<mutare_strong_piesa_de_mutat[i][j].j<<" pozitie in care se muta: "<<mutare_strong_locatie_in_care_se_muta[i].i<<" "<<mutare_strong_locatie_in_care_se_muta[i].j<<"\n";

                        if( danger > max_miscari_pana_cucereste_inamicul )
                        {

                            max_miscari_pana_cucereste_inamicul = danger;

                            i_exp = i;
                            j_exp = j;

                        }

                   }


               }

           }
cout<<"\n GATA pericol miscari ****strong*****: \n";

           if( max_miscari_pana_cucereste_inamicul > 0 )
           {

                locatie_de_mutat.i = mutare_strong_piesa_de_mutat[i_exp][j_exp].i;
                locatie_de_mutat.j = mutare_strong_piesa_de_mutat[i_exp][j_exp].j;

                linia = mutare_strong_locatie_in_care_se_muta[i_exp].i;
                coloana = mutare_strong_locatie_in_care_se_muta[i_exp].j;

                return;

           }


//GATA miscari STRONG ................................................................................................gata miscari strong




//cautam o miscare INOVENSIVA, si daca nu gasim, inseamana ca am pierdut oricum si atunci punem una la nimereala

         //cautam sa o piesa dintr-un grup strong, adica daca mutam ceva din ea sigur vor fi mai mult de 1 spatiu gol de umplut pt inamic ca sa castige
         for( unsigned int i = 0; i<nr_grupurile_mele; i++ )
         {

             if( nr_sp_goale_ale_mele[i] > 2 )
             {

                 linia = sp_goale_ale_mele[i][0].i;
                 coloana = sp_goale_ale_mele[i][0].j;

                 for( unsigned int j = 0; j < nr_piesele_mele[i]; j++ )
                 {

                     if( adiacente( grupurile_mele[i][j].i, grupurile_mele[i][j].j, sp_goale_ale_mele[i][0].i, sp_goale_ale_mele[i][0].j ) )
                     {
                        cout<<"\n cautam o piesa dintr-un grup strong line 2415********************************************** \n";
                         locatie_de_mutat.i = grupurile_mele[i][j].i;
                         locatie_de_mutat.j = grupurile_mele[i][j].j;


                         return;
                     }

                 }

             }

         }
//GATA miscare inovensiva ............................................................................................GATA miscare inovensiva





//daca n-am gasit pun o mutare OARECARE din primul grup
        linia = sp_goale_ale_mele[0][0].i;
        coloana = sp_goale_ale_mele[0][0].j;

        for( unsigned int j = 0; j < nr_piesele_mele[0]; j++ )
        {

            if( adiacente( grupurile_mele[0][j].i, grupurile_mele[0][j].j, sp_goale_ale_mele[0][0].i, sp_goale_ale_mele[0][0].j ) )
            {
                cout<<"\n //daca n-am gasit pun o mutare oarecare din primul grup line 2438********************************************** \n";
                locatie_de_mutat.i = grupurile_mele[0][j].i;
                locatie_de_mutat.j = grupurile_mele[0][j].j;

                return;

            }

        }


    }





}


int runda2(bool computer)
{

    //locatii pt dreptunghiul colorat care indica ce player trb sa faca miscarea
    int poly[8], poly2[8];
    poly[0] = stanga + width;
    poly[1] = buton_meniu[0].j;

    poly[2] = buton_meniu[1].i+5;
    poly[3] =  buton_meniu[0].j;

    poly[4] = buton_meniu[1].i+5;
    poly[5] =  buton_meniu[1].j;

    poly[6] = stanga + width;
    poly[7] =  buton_meniu[1].j;

    if( stanga+width - buton_meniu[1].i+5 < 300 )
    {
        poly[0] = buton_meniu[1].i+5 + 300;
        poly[6] = buton_meniu[1].i+5 + 300;

    }

    //..............................


    for( unsigned int i = 0; i<8; i++ ) { poly2[i] = poly[i]; }



    if( stanga+width - restart[1].i+5 < 300 )
    {
        poly[0] = restart[1].i+5 + 300;
        poly[6] = restart[1].i+5 + 300;
    }

    int win = 0;

    while(win == 0 )
        {

            bool gata = false;

            bool exista_locatii = false;//sunt stabilite niste pozitii in care se poate muta/ a fost selectata o piesa de mutat
            int nr_de_locatii = 0;// nr de locatii in care se poate muta piesa care va fi selectata


            //facem un dreptunghi colorat in culoarea playerului care trb sa mute o piesa in prezent
            setcolor(0);
            setlinestyle(0, 1, 0);
            setfillstyle(1, culoare_player[1]);
            fillpoly(4, poly);


            //text cu numele playerului1, care urmeaza sa puna mute o piesa
            setcolor(culoare_player[1]);
            settextjustify(1, 1);
            settextstyle(0, HORIZ_DIR, 3);
            outtextxy(poly[0]-(poly[0] - poly[2])/2, poly[5]-(poly[5]-poly[1])/2+6, jucatori[0]);


            int i_mouse_anterior=-2, j_mouse_anterior=-2;

            locatie_de_mutat.i = -1; locatie_de_mutat.j = -1;

            //limita tre sa fie 100, poti sa schimbi in schimb ritmul de crestere


            float durata = 5000; //5000;
            float start_time = clock(), finish_time = start_time + durata;

//.........................cautare input player1..........................................................................................................
            while(!gata)
                {
                    //memoram timpul curent, clock() inregistreaza timpul care a trecut decat a inceput compilarea
                    int time = clock();

                    //daca timpul prezent dapaseste timpul final atunci stergem tot si oprim cautare input
                    if( time>finish_time )
                    {
                        if(nr_de_locatii)
                            {
                                stergere_efect(locatie_de_mutat.i, locatie_de_mutat.j);
                                stergere_pozitii(nr_de_locatii);
                            }

                        if( i_mouse_anterior > -2 )
                        {

                            stergere_efect(i_mouse_anterior, j_mouse_anterior);

                        }

                        break;

                    }

                 //desenare limita de timp.............................................................


                    //finish_time - time ........... procent%
                    //durata ......... 100%,      => procent% == ( 100*( finish_time - time ) ) / durata
                    int procent = ( 100*( finish_time - time ) )/durata;

                    //width.......1%
                    //width_dreptunghi ........... procent% => width_dreptunghi == ( procent*width )/100
                    int width_dreptunghi = ( procent * (poly[0] - poly[2]) ) /100 ;

                    poly2[2] = poly[2] + width_dreptunghi;

                    poly2[4] = poly[2] + width_dreptunghi;


                    //facem un dreptunghi colorat in culoare neagra
                    setcolor(0);
                    setlinestyle(0, 1, 0);
                    setfillstyle(1, 0);
                    fillpoly(4, poly2);


//                    //text cu numele playerului1, care urmeaza sa puna mute o piesa,
//                    //nu pune codul asta ca face urat graphic.h cand se repeta in cotinuu adaugarea de text
//                    setcolor(culoare_player[2]);
//                    settextjustify(1, 1);
//                    settextstyle(0, HORIZ_DIR, 3);
//                    outtextxy(poly[0]-(poly[0] - poly[2])/2, poly[5]-(poly[5]-poly[1])/2+6, jucatori[1]);

                //...............................................................................





                if(ismouseclick(WM_LBUTTONDOWN))
                {
                    clearmouseclick(WM_LBUTTONDOWN);


                    int x_mouse = mousex();
                    int y_mouse = mousey();



                    if(x_mouse<restart[1].i && x_mouse>restart[0].i && y_mouse<restart[1].j && y_mouse>restart[0].j )
                    {

                        PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                        return -1;
                    }


                    if(x_mouse<buton_meniu[1].i && x_mouse>buton_meniu[0].i && y_mouse<buton_meniu[1].j && y_mouse>buton_meniu[0].j )
                    {

                        PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                        return -2;
                   }

                    //verific prima data daca e in cadran
                    if (x_mouse>stanga && x_mouse<stanga+width && y_mouse>sus && y_mouse<sus+height)
                    {

                            int linia=(y_mouse-sus)/latura+1;
                            int coloana=(x_mouse-stanga)/latura+1;

                            //daca playerul isi selecteaza o piesa pe care doreste sa o mute
                            if( tabel[linia][coloana].player == 1 && ( linia!=locatie_de_mutat.i || coloana!=locatie_de_mutat.j ) )
                            {

                                PlaySound("selectare_piesa.wav", NULL, SND_FILENAME|SND_ASYNC);
                                //daca exista deja o piesa selectata, ii eliminam acest statut
                                if(exista_locatii)
                                    {

                                        stergere_efect( locatie_de_mutat.i, locatie_de_mutat.j );
                                        stergere_pozitii( nr_de_locatii );

                                    }




                                //vedem daca piesa noua selectata se poate muta undeva
                                //si daca se poate atunci vom hasura pozitiile adiacente in care se poate muta
                                //vom marca aceste locatii cu 3


                                exista_locatii = adaugare_pozitii_in_care_se_poate_muta(linia, coloana, nr_de_locatii );
                                if( exista_locatii )
                                {
                                    i_mouse_anterior = -2;
                                    j_mouse_anterior = -2;


                                }

                            }else{

                                //daca casuta  e 3, asta marcheaza o pozitie
                                //in care se poate muta piesa
                                if( tabel[linia][coloana].player == 3 )
                                {

                                    //PlaySound("mutare_piesa.wav", NULL, SND_FILENAME|SND_ASYNC);

                                    mciSendString("play mutare_piesa.wav", NULL, 0, NULL);

                                    if( i_mouse_anterior == locatie_de_mutat.i && j_mouse_anterior == locatie_de_mutat.j )
                                    {

                                        i_mouse_anterior = -2;
                                        j_mouse_anterior = -2;

                                    }


                                    mutare( linia, coloana, nr_de_locatii, 1 );

                                    exista_locatii = false;

                                    gata = true;

                                    win = castigaRunda2(linia, coloana, 2);

                                    inregistrare_miscare_in_fisier();

                                    if( win>0 )
                                        return win;


                                     break;

                                }



                            }



                    }



                }else{



                    int x_mouse = mousex();
                    int y_mouse = mousey();

                    //verific daca cursorul e deasupra spatiului tablei de joc
                    if (x_mouse>stanga && x_mouse<stanga+width && y_mouse>sus && y_mouse<sus+height)
                    {

                            int linia = (y_mouse-sus)/latura+1;
                            int coloana = (x_mouse-stanga)/latura+1;


                            //avem grija ca sa nu repetam colorarea de mai multe ori
                            //nu coloram locatia care a fost selectata pt a fi mutata
                        if( tabel[linia][coloana].player == 1 )
                            if( ( linia !=locatie_de_mutat.i || coloana!=locatie_de_mutat.j ) && (linia != i_mouse_anterior || coloana != j_mouse_anterior) )
                                {

                                    //desenam
                                    desenare_efect(linia, coloana);

                                    if( i_mouse_anterior > -2 )
                                        stergere_efect(i_mouse_anterior, j_mouse_anterior);


                                    i_mouse_anterior = linia;
                                    j_mouse_anterior = coloana;

                                }



                    }




                }




              //verific prima data daca e in cadra
                //verific ce fel de casuta a apasat
                        //daca e al unui inamic sau e goala nu se mai intampla nimic
                        //daca e de acelasi fel cu playerul -> afisez alte butoane pe pozitii vecine goale pe care sa le apese ca sa o mute,
                                                            //-> punand 3 pe pozitiile disponibile in care se poate muta
                                                            // sterg 3 din pozitiile anteriore
                        //daca e casuta are casuta.player=3 atunci mut piesa acolo si ii dau lui casuta.player = player,
                                                            //sterg casuta actuala si toate valori de 3 din alte casuta



            }



             if( i_mouse_anterior>-2 )
             {


                setcolor(culoare_player[1]);
                setfillstyle(1, culoare_player[1]);
                fillellipse(tabel[i_mouse_anterior][j_mouse_anterior].x, tabel[i_mouse_anterior][j_mouse_anterior].y, latura/4, latura/4);

                i_mouse_anterior = -2;
                j_mouse_anterior = -2;

             }




//................................................................................................................................................................







            // randul playerului 2 sa mute o piesa
            //facem un dreptunghi colorat in culoarea playerului care trb sa mute o piesa in prezent
            setcolor(0);
            setlinestyle(0, 1, 0);
            setfillstyle(1, culoare_player[2]);
            fillpoly(4, poly);

            //text cu numele playerului1, care urmeaza sa puna mute o piesa
            setcolor(culoare_player[2]);
            settextjustify(1, 1);
            settextstyle(0, HORIZ_DIR, 3);
            outtextxy(poly[0]-(poly[0] - poly[2])/2, poly[5]-(poly[5]-poly[1])/2+6, jucatori[1]);

            gata = false;

            //sunt stabilite niste pozitii in care se poate muta/ a fost selectata o piesa de mutat
            exista_locatii = false;
            nr_de_locatii = 0;// nr de locatii in care se poate muta piesa care va fi selectata

            i_mouse_anterior = -2;
            j_mouse_anterior = -2;

            locatie_de_mutat.i = -1;
            locatie_de_mutat.j = -1;


            start_time = clock();
            finish_time = start_time + durata;

//.....................cautare input player2..........................................................................................
            while(!gata)
            {


                    int time = clock();

                    if( time>finish_time )
                    {
                        if(nr_de_locatii)
                            {
                                stergere_efect(locatie_de_mutat.i, locatie_de_mutat.j);
                                stergere_pozitii(nr_de_locatii);
                            }

                        if( i_mouse_anterior > -2 )
                        {

                            stergere_efect(i_mouse_anterior, j_mouse_anterior);

                        }

                        break;

                    }
                 //desenare limita de timp.............................................................


                    //finish_time - time ........... procent%
                    //durata ......... 100%,      => procent% == ( 100*( finish_time - time ) ) / durata
                    int procent = ( 100*( finish_time - time ) )/durata;

                    //width.......1%
                    //width_dreptunghi ........... procent% => width_dreptunghi == ( procent*width )/100
                    int width_dreptunghi = ( procent * (poly[0] - poly[2]) ) /100 ;

                    poly2[2] = poly[2] + width_dreptunghi;

                    poly2[4] = poly[2] + width_dreptunghi;


                    //facem un dreptunghi colorat in culoare neagra
                    setcolor(0);
                    setlinestyle(0, 1, 0);
                    setfillstyle(1, 0);
                    fillpoly(4, poly2);


//                    //text cu numele playerului1, care urmeaza sa puna mute o piesa,
//                    //nu pune codul asta ca face urat graphic.h cand se repeta in cotinuu adaugarea de text
//                    setcolor(culoare_player[2]);
//                    settextjustify(1, 1);
//                    settextstyle(0, HORIZ_DIR, 3);
//                    outtextxy(poly[0]-(poly[0] - poly[2])/2, poly[5]-(poly[5]-poly[1])/2+6, jucatori[1]);

                //...............................................................................




if( !computer )
{

                if(ismouseclick(WM_LBUTTONDOWN))
                {

                    clearmouseclick(WM_LBUTTONDOWN);//
                    int x_mouse = mousex();
                    int y_mouse = mousey();


                    if(x_mouse<=restart[1].i && x_mouse>=restart[0].i && y_mouse<=restart[1].j && y_mouse>=restart[0].j )
                    {

                        PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                        return -1;
                    }

                   if(x_mouse<buton_meniu[1].i && x_mouse>buton_meniu[0].i && y_mouse<=buton_meniu[1].j && y_mouse>buton_meniu[0].j )
                   {

                        PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                        return -2;
                   }

                    //verific prima data daca e in cadran
                    if (x_mouse>stanga && x_mouse<stanga+width && y_mouse>sus && y_mouse<sus+height)
                    {

                            int linia=(y_mouse-sus)/latura+1;
                            int coloana=(x_mouse-stanga)/latura+1;

                            //daca playerul isi selecteaza o piesa pe care doreste sa o mute
                            if( tabel[linia][coloana].player == 2   && ( linia!=locatie_de_mutat.i || coloana!=locatie_de_mutat.j ) )
                            {

                                PlaySound("selectare_piesa.wav", NULL, SND_FILENAME|SND_ASYNC);
                                if(exista_locatii)
                                    {

                                        stergere_pozitii( nr_de_locatii );
                                    }

                                //vedem daca piesa noua selectata se poate muta undeva
                                //si daca se poate atunci vom hasura pozitiile adiacente in care se poate muta
                                //vom marca aceste locatii cu 3
                                exista_locatii = adaugare_pozitii_in_care_se_poate_muta(linia, coloana, nr_de_locatii );

                                if( exista_locatii )
                                {
                                    i_mouse_anterior = -2;
                                    j_mouse_anterior = -2;
                                }

                            }else{

                                //daca exista deja o piesa selectata si playerul apasa pe o casuta marcata hasurat( marcata cu 3 in tabel)
                                //in care se poate muta piesa selectata
                               if( tabel[linia][coloana].player == 3 )
                                  {

                                    PlaySound("mutare_piesa.wav", NULL, SND_FILENAME|SND_ASYNC);
                                    if( i_mouse_anterior == locatie_de_mutat.i && j_mouse_anterior == locatie_de_mutat.j )
                                    {

                                        i_mouse_anterior = -2;
                                        j_mouse_anterior = -2;

                                    }

                                    mutare( linia, coloana, nr_de_locatii, 2 );

                                    exista_locatii = false;

                                    gata = true;


                                    win = castigaRunda2(linia, coloana, 1);
                                    inregistrare_miscare_in_fisier();
                                    if(win>0)
                                        return win;

                                  }

                                }



                    }



                }else{



                    int x_mouse = mousex();
                    int y_mouse = mousey();

                    //verific daca cursorul e deasupra spatiului tablei de joc
                    if (x_mouse>stanga && x_mouse<stanga+width && y_mouse>sus && y_mouse<sus+height)
                    {

                            int linia = (y_mouse-sus)/latura+1;
                            int coloana = (x_mouse-stanga)/latura+1;


                            //avem grija ca sa nu repetam colorarea de mai multe ori
                            //nu coloram locatia care a fost selectata pt a fi mutata
                        if( tabel[linia][coloana].player == 2 )
                            if( ( linia !=locatie_de_mutat.i || coloana!=locatie_de_mutat.j ) && (linia != i_mouse_anterior || coloana != j_mouse_anterior) )
                                {

                                    //desenam
                                    desenare_efect(linia, coloana);

                                    if( i_mouse_anterior > -2 )
                                        stergere_efect(i_mouse_anterior, j_mouse_anterior);


                                    i_mouse_anterior = linia;
                                    j_mouse_anterior = coloana;

                                }



                    }




                }

}else{

        //daca joaca computerul
        int linia=1, coloana=1;

        plasare_piesa_computer_runda2(linia, coloana );

        cout<<"\n computer: linia: "<<linia<<", coloana:  "<<coloana<<'\n';
        cout<<" computer: locatie_de_mutat.i: "<<locatie_de_mutat.i<<", locatie_de_mutat.j: "<<locatie_de_mutat.j<<'\n'<<'\n';

        if( tabel[linia][coloana].player > 0 )
        {
            cout<<"\n EROARE!!! PIESA SE MUTA PESTE ALTA \n";
        }

        //linia = 1;
        //coloana = 2;

        //locatie_de_mutat.i = 1;
        //locatie_de_mutat.j = 2;


        PlaySound("erase.wav", NULL, SND_FILENAME|SND_ASYNC);


        mutare( linia, coloana, nr_de_locatii, 2 );

        exista_locatii = false;

        gata = true;

        win = castigaRunda2(linia, coloana, 1);

        inregistrare_miscare_in_fisier();

        if(win>0)
            return win;



    }

            }
//.........................................................................................

             if( i_mouse_anterior>-2 )
             {

                locatie_de_mutat.i = -1;//fiindca efecte_cursor_ are o conditie ca ant_
                locatie_de_mutat.j = -1;

                setcolor(culoare_player[2]);
                setfillstyle(1, culoare_player[2]);
                fillellipse(tabel[i_mouse_anterior][j_mouse_anterior].x, tabel[i_mouse_anterior][j_mouse_anterior].y, latura/4, latura/4);

                i_mouse_anterior = -2;
                j_mouse_anterior = -2;

             }





    }


    return 0;

}

void stabilirea_latura_casuta_tabla_dupa_fereastra()
{
    int border_tabel = 30;

    int lat_verticala = ( getmaxy() - 60 )/nr_linii;
    int lat_orizontala = getmaxx()/nr_col;

    if( lat_orizontala<lat_verticala )
    {

        latura = lat_orizontala;

    }else{

        latura = lat_verticala;

    }

}

void setare_coordonate_butoane_meniu_restart()
{

//border_tabel = 30;

    //....desenare buton restart...........................


    restart[0].i = stanga;
    restart[0].j = sus-latimeButonStart;

    restart[1].i = restart[0].i + width/6;
    restart[1].j = sus - 2;

    if(width/6<100)
    {

        restart[1].i = getmaxx()/2 - 100;
        restart[0].i = getmaxx()/2 - 200;

    }

    //butonul de meniu
    buton_meniu[0].i = restart[1].i;
    buton_meniu[0].j = sus-latimeButonStart;

    buton_meniu[1].i = restart[1].i + width/6;
    buton_meniu[1].j = sus - 2;

    if(width/6<100)
    {
        buton_meniu[0].i = restart[1].i;
        buton_meniu[1].i = restart[1].i + 70;

    }

}

void joc( bool &restartare, bool &menu, bool computer, int index_miscare )
{
    cout<<latura;

    desenare_tabla(nr_col, nr_linii, latura);

    if( index_miscare < 0 )
    {

        nr_miscari = 0;
        ofstream fout;

        fout.open("istoric_mutari.txt");
        if (fout)
        {

            fout<<0<<"\n";


        }
        fout.close();

        fout.open("istoric_mutari_aux.txt");
        if (fout)
        {

            fout<<0<<"\n";


        }
        fout.close();









    }else{

        nr_miscari = index_miscare+1;


        ofstream fout;
        ifstream fin;

        fin.open("istoric_mutari.txt");
        fout.open("istoric_mutari_aux.txt");

        if (fout && fin)
        {
            //bag toate mutarile din istoric pana la index_miscare in aux, dupa care le mut inapoi cu nr_miscari actualizat
            int player_ist;
            fin>>player_ist;

            fout<<nr_miscari;

            for( int k = 0; k<nr_miscari; k++ )
            {


                for( int i = 1; i<=nr_linii; i++ )
                {
                    fout<<"\n";
                    for( int j = 1; j<=nr_col; j++ )
                    {

                        fin>>player_ist;

                        fout<<player_ist<<" ";

                    }

                }


            }


        }


        misc_ind[0] = 0;
        misc_ind[1] = 0;



        fin.close();
        fout.close();

        fin.open("istoric_mutari_aux.txt");
        fout.open("istoric_mutari.txt");

        if( fout && fin )
        {
            int nr;

            fout<<nr_miscari;

            fin>>nr;

            for( int k = 0; k<nr_miscari; k++ )
            {


                for( int i = 1; i<=nr_linii; i++ )
                {
                    fout<<"\n";
                    for( int j = 1; j<=nr_col; j++ )
                    {



                        fin>>nr;

                        fout<<nr<<"\n";

                        if( nr!=0 && k == index_miscare )
                        {

                            tabel[i][j].player = nr;


                            setfillstyle(1, 1);
                            drawX( i, j, 12 );

                            setcolor(culoare_player[tabel[i][j].player]);
                            setfillstyle(1,culoare_player[tabel[i][j].player]);
                            fillellipse(tabel[i][j].x, tabel[i][j].y, latura/4, latura/4);



                            piese_player[ nr - 1 ][ misc_ind[ nr-1 ] ].i = i;
                            piese_player[ nr - 1 ][ misc_ind[ nr-1 ] ].j = j;

                            misc_ind[nr-1]++;

                        }


                    }

                }


            }

        }


        fin.close();
        fout.close();



    }



    //....desenare buton restart...........................
    setcolor(14);
    setlinestyle(1, 0, 3);
    rectangle( restart[0].i, restart[0].j, restart[1].i, restart[1].j );

    settextjustify(1,1);
    settextstyle(0, HORIZ_DIR, 2);
   // char restartText[]="RESTART";
    outtextxy( (restart[1].i - restart[0].i)/2 + restart[0].i, restart[1].j-latimeButonStart/2 + 2,  cuvinte[ 33 ] );




    //butonul de meniu
    setcolor(14);
    setlinestyle(1, 0, 3);
    rectangle( buton_meniu[0].i, buton_meniu[0].j, buton_meniu[1].i, buton_meniu[1].j );

    settextjustify(1,1);
    settextstyle(0, HORIZ_DIR, 2);
    //char menuText[]="Menu";
    outtextxy( (buton_meniu[1].i - buton_meniu[0].i)/2 + buton_meniu[0].i, buton_meniu[1].j-latimeButonStart/2 + 2,  cuvinte[ 8 ] );



    cout<<"\n";
    cout<<" Stabilire*** buton restart: "<<restart[0].i<<"  "<<restart[0].j<<" "<<restart[1].i<<"  "<<restart[1].j<<"\n";
 cout<<" STABILIRE*** buton meniu: "<<buton_meniu[0].i<<"  "<<buton_meniu[0].j<<" "<<buton_meniu[1].i<<"  "<<buton_meniu[1].j<<"\n";


    if( nr_miscari>=16 )
    {

        int rez = runda2(computer);

        if( rez > -1)
        {
            scrie_castigator(rez);
        }else{
            if( rez == -1 )
                restartare = true;
            else menu = true;

        }
        return;
    }


    //................................................

    //daca rez = 0 inseamna ca soarta jocului nu e hotarata si rulam runda 2
    //daca rez = -1 inseamna ca s-a apasat butonul de restart
    //daca rez = 1,2 inseamna ca a castigat cineva runda
    //daca rez = 3 atunci e remiza, remiza poate avea loc dor in prima runda
                //pt ca piesa poate fi pusa direct intr-un grup inconjurat de inamici
    int rez;
    if( index_miscare > -1 )
        rez = prima_runda(computer, true);
    else rez = prima_runda(computer, false);

    if( rez == 0 )
    {

            //daca rez = -1 inseamna ca s-a apasat butonul de restart
            //daca rez = 1,2 inseamna ca a castigat cineva runda
            //daca rez = 3
            rez = runda2(computer);

            if( rez > -1)
            {
                scrie_castigator(rez);
            }else{
                if( rez == -1 )
                    restartare = true;
                else menu = true;

            }

    }else {

            if(rez < 0)
                {
                    if( rez == -1 )
                        restartare = true;
                    else menu = true;

                }else scrie_castigator(rez);
          }



}

//mi se modifica coordonatele butoanelor for some reason,
//am urmarit fiecare linie de cod care contine variabilele si nici una nu e are nimic rau
//probabil e de la librarie
void reparare_butoane_restart_meniu(int rezerva_butoane[])
{
    restart[0].i = rezerva_butoane[0];
    restart[0].j = rezerva_butoane[1];
    restart[1].i = rezerva_butoane[2];
    restart[1].j = rezerva_butoane[3];

    buton_meniu[0].i = rezerva_butoane[4];
    buton_meniu[0].j = rezerva_butoane[5];
    buton_meniu[1].i = rezerva_butoane[6];
    buton_meniu[1].j = rezerva_butoane[7];


}

void desenare_piese_din_istoric(int index_mutare)
{

    ifstream fin;

    fin.open("istoric_mutari.txt");

    //bag mutarile precedente in auxiliar
    if (fin)
    {

        int nr, player_val;

        fin>>nr;

        for( int k = 0; k<=index_mutare; k++ )
        {

            for( int i = 1; i<=nr_linii; i++ )
            {

                for( int j = 1; j<=nr_col; j++ )
                {

                    fin>>player_val;


                    if( k != index_mutare )
                        continue;

                    desenare_loc_de_mutat(i, j, SOLID_FILL, 10);
                    if( player_val == 0 )
                        {

                            continue;
                        }

                    setcolor(1);
                    setfillstyle(1, 1);

                    drawX( i, j, 12 );

                    setcolor( culoare_player[player_val] );
                    setfillstyle(1, culoare_player[player_val] );
                    fillellipse(tabel[i][j].x, tabel[i][j].y, latura/4, latura/4);



                }


            }

        }

    }

    fin.close();


}

void play(bool computer)
{

    bool restartare = false;
    bool menu = false;


         //lungimea si latime intregii table de joc
     height = latura*nr_linii;
     width = latura*nr_col;

     sus = border_tabel; // coordonata y de la care porneste construirea de casute
     stanga = ( getmaxx() - width )/2; // coordonata x de la care porneste construirea de casute
    cout<<"\n stanga:"<<stanga<<"\n";
    cout<<"\n width:"<<width<<"\n";

    stabilirea_latura_casuta_tabla_dupa_fereastra();
    setare_coordonate_butoane_meniu_restart();

    //nu inteleg de ce se modifica butoanele cand se joaca cu pc, asa random
    int rezerva_butoane[8];

    rezerva_butoane[0] = restart[0].i;
    rezerva_butoane[1] = restart[0].j;
    rezerva_butoane[2] = restart[1].i;
    rezerva_butoane[3] = restart[1].j;

    rezerva_butoane[4] = buton_meniu[0].i;
    rezerva_butoane[5] = buton_meniu[0].j;
    rezerva_butoane[6] = buton_meniu[1].i;
    rezerva_butoane[7] = buton_meniu[1].j;


    joc( restartare, menu, computer, -1 );

    reparare_butoane_restart_meniu(rezerva_butoane);





    locatie buton_istoric_miscari[2];

    buton_istoric_miscari[0].i = getmaxx()/4;
    buton_istoric_miscari[1].i = getmaxx() - getmaxx()/4;

    buton_istoric_miscari[0].j = getmaxy()-29;
    buton_istoric_miscari[1].j = getmaxy()-1;

    deseneaza_buton(buton_istoric_miscari[0].i,buton_istoric_miscari[1].i,buton_istoric_miscari[0].j,buton_istoric_miscari[1].j, cuvinte[36], 14);

    locatie buton_next[2];
    buton_next[0].i = (buton_istoric_miscari[0].i + buton_istoric_miscari[1].i)/2 + 5;
    buton_next[1].i = buton_istoric_miscari[1].i;

    buton_next[0].j = buton_istoric_miscari[0].j;
    buton_next[1].j = buton_istoric_miscari[1].j;


    locatie buton_prev[2];
    buton_prev[0].i = buton_istoric_miscari[0].i;
    buton_prev[1].i = (buton_istoric_miscari[0].i + buton_istoric_miscari[1].i)/2 -  5 ;

    buton_prev[0].j = buton_istoric_miscari[0].j;
    buton_prev[1].j = buton_istoric_miscari[1].j;

    int index_miscare = 0;
    bool istoric_buton_active = false;



    bool poti_reicepe_de_aici = false;

    locatie buton_reincepe_de_aici[2];
    buton_reincepe_de_aici[0].i = buton_istoric_miscari[1].i+5;
    buton_reincepe_de_aici[1].i = getmaxx()-10;

    buton_reincepe_de_aici[0].j = buton_istoric_miscari[0].j;
    buton_reincepe_de_aici[1].j = buton_istoric_miscari[1].j;


    bool gata = false;

    do{

        if( menu )
        {
            return;
        }

        if( restartare )
        {
            clearviewport();
            restartare = false;
            menu = false;
            joc(restartare, menu, computer, -1);
            reparare_butoane_restart_meniu(rezerva_butoane);
        }

        //daca a castigat cineva functia va cauta input pt restartare sau intoarcere la meniu
       if(ismouseclick(WM_LBUTTONDOWN))
        {
            clearmouseclick(WM_LBUTTONDOWN);
            int x_mouse=mousex();
            int y_mouse=mousey();

            if(x_mouse<=restart[1].i && x_mouse>=restart[0].i && y_mouse<=restart[1].j && y_mouse>=restart[0].j )
             {
                //acopar butonul istoric mutari sau next si/sa prev
                setfillstyle(1, 0);
                bar(buton_istoric_miscari[0].i, getmaxx()-5, buton_istoric_miscari[0].j,buton_istoric_miscari[1].j);

                cout<<"\n BUTON RESTART Play() \n";
                PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);

                clearviewport();
                restartare = false;
                menu = false;

                istoric_buton_active = false;
                poti_reicepe_de_aici = false;
                deseneaza_buton(buton_reincepe_de_aici[0].i, buton_reincepe_de_aici[1].i,buton_reincepe_de_aici[0].j,buton_reincepe_de_aici[1].j, cuvinte[10], 0);
                index_miscare = 0;


                deseneaza_buton(buton_istoric_miscari[0].i,buton_istoric_miscari[1].i,buton_istoric_miscari[0].j,buton_istoric_miscari[0].j, cuvinte[36], 0);



                joc(restartare, menu, computer, -1);
                reparare_butoane_restart_meniu(rezerva_butoane);


                deseneaza_buton(buton_istoric_miscari[0].i,buton_istoric_miscari[1].i,buton_istoric_miscari[0].j,buton_istoric_miscari[1].j, cuvinte[36], 14);

             }

            if(x_mouse<=buton_meniu[1].i && x_mouse>=buton_meniu[0].i && y_mouse<=buton_meniu[1].j && y_mouse>=buton_meniu[0].j )
             {
                cout<<"\n BUTON MENIU in Play() \n";
                 PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                 gata = true;
             }

                                                         cout<<"\n caut input dupa ce a castigat cineva in Play() click x, y: "<<x_mouse <<" "<<y_mouse<<"\n";
                                                         cout<<" buton restart: "<<restart[0].i<<"  "<<restart[0].j<<" "<<restart[1].i<<"  "<<restart[1].j<<"\n";
                                                         cout<<" buton meniu: "<<buton_meniu[0].i<<"  "<<buton_meniu[0].j<<" "<<buton_meniu[1].i<<"  "<<buton_meniu[1].j<<"\n";
                                                         cout<<" latura: "<<latura<<" width: "<<width<<" stanga: "<<stanga<<" sus: "<<sus<<"\n";
                                                         cout<<" nr_linii: "<<nr_linii<<" nr_col: "<<nr_col<<"\n";


            if(x_mouse<=buton_istoric_miscari[1].i && x_mouse>=buton_istoric_miscari[0].i && y_mouse<=buton_istoric_miscari[1].j && y_mouse>=buton_istoric_miscari[0].j )
             {


                    if( index_miscare < nr_miscari - 1 )
                    {



                        if( istoric_buton_active )
                        {



                            if( x_mouse>=buton_next[0].i )
                            {
                                PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);



                                index_miscare++;

                                desenare_piese_din_istoric(index_miscare);


                                deseneaza_buton(buton_prev[0].i, buton_prev[1].i,buton_prev[0].j,buton_prev[1].j, cuvinte[6], 14);
                                char text[]="        ";

                                int n1=index_miscare, poz = strlen(text)-1;

                                do{
                                    text[poz] = n1%10 + 48;
                                    n1/=10;
                                    poz--;

                                }while(n1>0 && poz>=0);

                                setcolor(14);
                                outtextxy(100, 20, text);

                            }


                        }



                    }else{

                        deseneaza_buton(buton_next[0].i, buton_next[1].i,buton_next[0].j,buton_next[1].j, cuvinte[5], 0);
                    }


                    if( index_miscare > 0 )
                    {


                        if( istoric_buton_active )
                        {


                            if( x_mouse<buton_next[0].i )
                            {
                                PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                                index_miscare--;

                                //desenare_tabla(nr_linii, nr_col, latura);

                                desenare_piese_din_istoric(index_miscare);

                                deseneaza_buton(buton_next[0].i, buton_next[1].i,buton_next[0].j,buton_next[1].j, cuvinte[5], 14);



                                char text[]="        ";

                                int n1=index_miscare, poz = strlen(text)-1;

                                do{
                                    text[poz] = n1%10 + 48;
                                    n1/=10;
                                    poz--;

                                }while(n1>0 && poz>=0);

                                setcolor(14);
                                outtextxy(100, 20, text);

                            }

                        }

                    }else{
                        deseneaza_buton(buton_prev[0].i, buton_prev[1].i,buton_prev[0].j,buton_prev[1].j, cuvinte[6], 0);
                    }


                    if( !istoric_buton_active )
                        {
                            istoric_buton_active = true;

                            setfillstyle(1, 0);
                            bar( 1, getmaxy()/2, getmaxx(), getmaxy());


                                //desenare_tabla(nr_linii, nr_col, latura);

                                desenare_piese_din_istoric(0);

                            setfillstyle(1, 0);
                            bar(buton_istoric_miscari[0].i, buton_istoric_miscari[0].j,buton_istoric_miscari[1].i,buton_istoric_miscari[1].j);

                            deseneaza_buton(buton_next[0].i, buton_next[1].i,buton_next[0].j,buton_next[1].j, cuvinte[5], 14);
                            PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                        }


                    if( istoric_buton_active && index_miscare<nr_miscari-1 )
                    {
                            poti_reicepe_de_aici = true;

                            deseneaza_buton(buton_reincepe_de_aici[0].i, buton_reincepe_de_aici[1].i,buton_reincepe_de_aici[0].j,buton_reincepe_de_aici[1].j, cuvinte[10], 14);


                    }else {
                        poti_reicepe_de_aici = false;
                        deseneaza_buton(buton_reincepe_de_aici[0].i, buton_reincepe_de_aici[1].i,buton_reincepe_de_aici[0].j,buton_reincepe_de_aici[1].j, cuvinte[10], 0);

                    }

             }



            if( poti_reicepe_de_aici )
            {

                if(x_mouse<=buton_reincepe_de_aici[1].i && x_mouse>=buton_reincepe_de_aici[0].i && y_mouse<=buton_reincepe_de_aici[1].j && y_mouse>=buton_reincepe_de_aici[0].j )
                {

                    poti_reicepe_de_aici = false;

                    //acopar butonul istoric mutari sau next si/sa prev
                    setfillstyle(1, 0);
                    bar(buton_istoric_miscari[0].i, getmaxx()-5,buton_istoric_miscari[0].j,buton_istoric_miscari[1].j);

                    cout<<"\n BUTON RESTART Play() \n";
                    PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);

                    clearviewport();
                    restartare = false;
                    menu = false;

                    istoric_buton_active = false;
                    poti_reicepe_de_aici = false;
                    deseneaza_buton(buton_reincepe_de_aici[0].i, buton_reincepe_de_aici[1].i,buton_reincepe_de_aici[0].j,buton_reincepe_de_aici[1].j, cuvinte[10], 0);



                    deseneaza_buton(buton_istoric_miscari[0].i,buton_istoric_miscari[1].i,buton_istoric_miscari[0].j,buton_istoric_miscari[0].j, cuvinte[36], 0);



                    joc(restartare, menu, computer, index_miscare);
                    reparare_butoane_restart_meniu(rezerva_butoane);
                    index_miscare = 0;

                    deseneaza_buton(buton_istoric_miscari[0].i,buton_istoric_miscari[1].i,buton_istoric_miscari[0].j,buton_istoric_miscari[1].j, cuvinte[36], 14);




                }



            }


        }




    }while(!gata);


}

unsigned int text_to_number( char text[], int poz )
{
    unsigned int num = 0;
    for( unsigned int k = 0; k<poz; k++)
        num = num * 10 + (text[k]-48);

    return num;
}

//+nume player1 si nume player2
void input_linie_si_coloana(bool computer)
{

    char text2[] = ".................................~ C U G O ~.................................";
    setcolor(9);
    settextstyle(3, HORIZ_DIR, 70);
    outtextxy(getmaxx()/2, getmaxy()/7, text2 );



   // char cuvinte[11][] = "Numar de linii ( maxim 40, apasati ENTER ca sa inserati )  : ";

    setcolor(15);
    settextjustify(0, 0);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( 1, getmaxy()/4, cuvinte[11] );

//vezi sa pui intodeauna spatii in plus la text ca face crize cand il stergi si-l afisezi in graphics
    char t2[5]="    ";


    int poz = 0;
    nr_linii = -1;
    nr_col = -1;
    bool gata;

do{

    gata = false;

    do{
        //actualizam pe ecran in continuu inputul dat de player
        outtextxy ( getmaxx()-100, getmaxy()/4, t2);

        //inregistram caracterul scris de utilizator
        char c = getch();

        if( c==8 )//daca e brackfast stergem ultimul caracter
        {

                if (poz)
                {
                    t2[poz] = ' ';
                    poz--;
                    t2[poz] = ' ';

                    PlaySound("erase.wav", NULL, SND_FILENAME|SND_ASYNC);
                }

        }else{

                //daca e enter si s-a introdus macar un caracter atunci oprim scrierea
                if( c==13 && poz )
                {


                    if(  text_to_number(t2, poz)>0  )
                        gata = true;
                    else {


                        PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);


                                setcolor(9);
                                settextstyle(3, HORIZ_DIR, 3);

                                outtextxy(getmaxx()/2, 100, cuvinte[17] );

                                setcolor(15);
                                settextjustify(0, 0);
                                settextstyle(4, HORIZ_DIR, 2);


                    }


                }else{


                        if( c-48>=0 && c-48<=9)
                        {

                            if (poz < 2)
                            {
                                t2[poz] = c;
                                poz++;
                                t2[poz] = 0;


                                PlaySound("keyboard.wav", NULL, SND_FILENAME|SND_ASYNC);


                            }else{

                                //char atentionare[] = "prea MULTE cifre! !";


                                PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);


                                setcolor(9);
                                settextstyle(3, HORIZ_DIR, 3);

                                outtextxy(getmaxx() - getmaxx()/3, 100, cuvinte[18] );

                                setcolor(15);
                                settextjustify(0, 0);
                                settextstyle(4, HORIZ_DIR, 2);

                            }

                        }else{
                            //daca nu s-au introdus cifre punem o atentionare

                            //char atentionare[] = "scrieti DOAR cifre !";


                            PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);

                            setcolor(9);
                            settextstyle(3, HORIZ_DIR, 3);

                            outtextxy(getmaxx() - getmaxx()/3, 50, cuvinte[19] );

                            setcolor(15);
                            settextjustify(0, 0);
                            settextstyle(4, HORIZ_DIR, 2);

                        }

                     }



            }



    }while(!gata);



    //sirul obtinut il transformam in numar, memorandu-l
    nr_linii = 0;
    for(int k = 0; k<poz; k++)
        {
            nr_linii = nr_linii*10 + ( t2[k]-48 );
        }


    if( nr_linii >40 )
    {
        ///char atentionare[] = "Numar prea mare !";


        PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);

        setcolor(9);
        settextstyle(3, HORIZ_DIR, 3);

        outtextxy(getmaxx() - getmaxx()/3, 20, cuvinte[20] );
        setcolor(15);
        settextjustify(0, 0);
        settextstyle(4, HORIZ_DIR, 2);


    }

}while( nr_linii<1 || nr_linii>40 );

//.................................................coloane
//vezi sa pui intodeauna spatii in plus la text ca face crize cand il stergi si-l afisezi in graphics
    //char t3[] = "Numar de coloane ( maxim 40, apasati ENTER ca sa inserati )  : ";

    setcolor(15);
    settextjustify(0, 0);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( 1, getmaxy() - getmaxy()/2, cuvinte[12] );

    char t4[5]="    ";


     poz = 0;


do{
        gata = false;
    do{

        outtextxy ( getmaxx()-100, getmaxy() - getmaxy()/2, t4);
        char c = getch();

        if( c==8 )
        {

                if (poz)
                {
                    PlaySound("erase.wav", NULL, SND_FILENAME|SND_ASYNC);
                    t4[poz] = ' ';
                    poz--;
                    t4[poz] = ' ';
                }

        }else{

                if( c==13 && poz )
                {
                    if(  text_to_number(t4, poz)>0  )
                        {
                            PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                            gata = true;
                    }else {

                        ///char atentionare[] = "0 chiar e prea putin !";



                                PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);

                                setcolor(9);
                                settextstyle(3, HORIZ_DIR, 3);

                                outtextxy(getmaxx() - getmaxx()/3, 50, cuvinte[ 17 ] );

                                setcolor(15);
                                settextjustify(0, 0);
                                settextstyle(4, HORIZ_DIR, 2);


                    }


                }else{
                        if( c-48>=0 && c-48<=9)
                        {

                            if (poz < 2)
                            {
                                t4[poz] = c;
                                poz++;
                                t4[poz] = 0;


                                PlaySound("keyboard.wav", NULL, SND_FILENAME|SND_ASYNC);


                            }else{

                                //char atentionare[] = "prea MULTE cifre! !";


                                PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);

                                setcolor(5);
                                settextstyle(3, HORIZ_DIR, 3);

                                outtextxy(getmaxx() - getmaxx()/3, 100,cuvinte[ 18 ] );

                                setcolor(15);
                                settextjustify(0, 0);
                                settextstyle(4, HORIZ_DIR, 2);

                            }

                        }else{

                            //char atentionare[] = "scrieti DOAR cifre !";


                            PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);

                            setcolor(5);
                            settextstyle(3, HORIZ_DIR, 3);

                            outtextxy(getmaxx() - getmaxx()/3, 50, cuvinte[ 19 ] );

                            setcolor(15);
                            settextjustify(0, 0);
                            settextstyle(4, HORIZ_DIR, 2);

                        }

                     }



            }



    }while(!gata);

    nr_col = 0;

    //transformam inputul intr-un integer
    for(int k = 0; k<poz; k++)
        {
            nr_col = nr_col*10 + ( t4[k]-48 );
        }
    if( nr_col > 40 )
    {
        //char atentionare[] = "Numar prea mare !";

        setcolor(7);
        settextstyle(3, HORIZ_DIR, 3);

        outtextxy(getmaxx() - getmaxx()/3, 20, cuvinte[ 20 ] );
        setcolor(15);
        settextjustify(0, 0);
        settextstyle(4, HORIZ_DIR, 2);


    }


}while( nr_col<1 || nr_col>40 );

    //..............................................input nume player1

   // char t5[] = "Nume player 1 (maxim 10 de caractere) : ";

    setcolor(15);
    settextjustify(0, 0);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( 1, getmaxy() - getmaxy()/2 + 50, cuvinte[ 13 ] );

    char t6[15]="              ";


     poz = 0;
     gata = false;

    do{

        setcolor(15);
        settextjustify(0, 0);
        settextstyle(4, HORIZ_DIR, 2);

        outtextxy ( getmaxx()-200, getmaxy() - getmaxy()/2 + 50, t6);
        char c = getch();

        if( c==8 )
        {

                if (poz>0)
                {
                    setcolor(0);
                    outtextxy ( getmaxx()-200, getmaxy() - getmaxy()/2 + 50, t6);

                    poz--;
                    t6[poz] = ' ';
                    t6[poz+1] = NULL;

                    PlaySound("erase.wav", NULL, SND_FILENAME|SND_ASYNC);
                }

        }else{

                if( c==13 && poz>0 )
                {


                        PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);

                        char comp[] = "Computer";
                        strcpy( jucatori[0], t6 );


                        if( strcmp(comp, jucatori[0])==0 )
                        {


                            PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);

                           /// char atentionare[] = "Nume rezervat .......!                 ";

                                setcolor(5);
                                settextstyle(3, HORIZ_DIR, 3);

                                outtextxy(getmaxx()/2, getmaxy() - getmaxy()/3 - 100, cuvinte[ 21 ] );

                                setcolor(15);
                                settextjustify(0, 0);
                                settextstyle(4, HORIZ_DIR, 2);


                        }else gata = true;



                }else{
                        if( poz>9 )
                        {

                                PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);

                                //char atentionare[] = "nu se pot pune mai multe caractere !";

                                setcolor(5);
                                settextstyle(3, HORIZ_DIR, 3);

                                outtextxy(getmaxx()/2, getmaxy() - getmaxy()/3 - 100, cuvinte[ 22 ] );

                                setcolor(15);
                                settextjustify(0, 0);
                                settextstyle(4, HORIZ_DIR, 2);


                        }else if( c==' ' || (c>=65 && c<=90) || (c>=97 && c<=122) || (c>=48 && c<=57) )
                        {


                                PlaySound("keyboard.wav", NULL, SND_FILENAME|SND_ASYNC);

                                t6[poz] = c;
                                poz++;
                                t6[poz] = NULL;

                        }

                     }



            }



    }while(!gata);


    strcpy( jucatori[0], t6);


//...............................................................input nume player2

//daca nu se joaca cu computerul
if( !computer )
{


//vezi sa pui intodeauna spatii in plus la text ca face crize cand il stergi si-l afisezi in graphics
    //char t7[] = "Numer player 2 (maxim 10 de caractere) : ";

    setcolor(15);
    settextjustify(0, 0);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( 1, getmaxy() - getmaxy()/2 + 100, cuvinte[ 14 ] );

    //vezi sa pui intodeauna spatii in plus la text ca face crize cand il stergi si-l afisezi in graphics
    char t8[15]="              ";


     poz = 0;
     gata = false;

    do{

        setcolor(15);
        settextjustify(0, 0);
        settextstyle(4, HORIZ_DIR, 2);
        outtextxy ( getmaxx()-200, getmaxy() - getmaxy()/2 + 100, t8);
        char c = getch();

        if( c==8 )
        {

                if (poz>0)
                {
                    setcolor(0);
                    outtextxy ( getmaxx()-200, getmaxy() - getmaxy()/2 + 100, t8);

                    poz--;
                    t8[poz] = ' ';
                    t8[poz+1] = NULL;

                    PlaySound("erase.wav", NULL, SND_FILENAME|SND_ASYNC);
                }

        }else{

                if( c==13 && poz>0 )
                {


                    strcpy( jucatori[1], t8 );


                    PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);


                    if( strcmp( jucatori[1], jucatori[0] ) == 0 )
                    {


                                PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);

                                //char atentionare[] = "Eroare: NUME IDENTICE PLAYERI!";

                                setcolor(5);
                                settextstyle(3, HORIZ_DIR, 3);

                                outtextxy(getmaxx()/2, getmaxy() - getmaxy()/4 - 100, cuvinte[ 21 ] );

                                setcolor(15);
                                settextjustify(0, 0);
                                settextstyle(4, HORIZ_DIR, 2);

                    }else{ gata = true; }


                    char comp[] = "Computer";

                    if( strcmp(comp, jucatori[1])==0 )
                    {

                        PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);

                       /// char atentionare[] = "Nume rezervat .......!                 ";

                        setcolor(5);
                        settextstyle(3, HORIZ_DIR, 3);

                        outtextxy(getmaxx()/2, getmaxy() - getmaxy()/3 - 100, cuvinte[ 21 ] );

                        setcolor(15);
                        settextjustify(0, 0);
                        settextstyle(4, HORIZ_DIR, 2);

                        gata = false;
                    }


                }else{
                        if( poz>9 )
                        {


                                PlaySound("horn.wav", NULL, SND_FILENAME|SND_ASYNC);

                                //char atentionare[] = "nu se pot pune mai multe caractere !";

                                setcolor(5);
                                settextstyle(3, HORIZ_DIR, 3);

                                outtextxy(getmaxx()/2, getmaxy() - getmaxy()/4 - 100, cuvinte[ 22 ] );

                                setcolor(15);
                                settextjustify(0, 0);
                                settextstyle(4, HORIZ_DIR, 2);


                        }else if( c==' ' || (c>=65 && c<=90) || (c>=97 && c<=122) || (c>=48 && c<=57) )
                        {


                                PlaySound("keyboard.wav", NULL, SND_FILENAME|SND_ASYNC);
                                t8[poz] = c;
                                poz++;
                                t8[poz] = NULL;

                        }

                     }



            }



    }while(!gata);


    strcpy( jucatori[1], t8 );


}else{

    char nume_Computer[] = "Computer";

    strcpy(jucatori[1], nume_Computer);

}



    ///char t9[] = "Alege culoare player1: ";

    setcolor(15);
    settextjustify(0, 0);
    settextstyle(4, HORIZ_DIR, 1);
    outtextxy( 1, getmaxy() - getmaxy()/2 +150, cuvinte[ 15 ] );

    //aici sunt sunt inregistrate pt ficare dreptunghi colorat locatiile de tip x, care marcheaza marginile
    locatie extremitari_x[14];
    int j = 0;
    for( int i = 1; i<=getmaxx() - getmaxx()/14; i+=getmaxx()/14 )
    {

            if( j+1 == 10 )
            {
                    //daca e culoarea tablei de joc atunci nu o i-au
                    extremitari_x[j].i = -1;
                    extremitari_x[j].j = -1;
                    j++;
                    continue;
            }

            setcolor(j+1);
            setfillstyle(0, j+1);
            rectangle( i, getmaxy() - getmaxy()/2 +170, i+getmaxx()/16,  getmaxy() - getmaxy()/2 +200 );

            extremitari_x[j].i = i;
            extremitari_x[j].j = i+getmaxx()/16;

            j++;

    }

    gata = false;

    do{

       if(ismouseclick(WM_LBUTTONDOWN))
            {
                clearmouseclick(WM_LBUTTONDOWN);
                int x_mouse=mousex();
                int y_mouse=mousey();

                if( y_mouse > getmaxy() - getmaxy()/2 + 170 && y_mouse < getmaxy() - getmaxy()/2 + 200 )
                {


                    PlaySound("keyboard.wav", NULL, SND_FILENAME|SND_ASYNC);

                    for( int i = 0; i<14; i++ )
                    {

                        if( x_mouse>extremitari_x[i].i && x_mouse<extremitari_x[i].j )
                        {

                                char t10[] = "X";

                                setcolor(i+1);
                                settextjustify(0, 0);
                                settextstyle(4, HORIZ_DIR, 2);
                                outtextxy( ( extremitari_x[i].i + extremitari_x[i].j )/2, getmaxy() - getmaxy()/2 +185 , t10 );

                                culoare_player[1] = i+1;

                                gata = true;

                                break;

                        }

                    }


                }

            }



    }while(!gata);






//daca nu jucam cu computer-ul
if(!computer)
{

    //char t11[] = "Alege culoare player2: ";

    setcolor(15);
    settextjustify(0, 0);
    settextstyle(4, HORIZ_DIR, 1);
    outtextxy( 1, getmaxy() - getmaxy()/6 + 40,cuvinte[ 16 ] );


    //j e indexul setului de coordonate
    // rectangle ul construit cu extremitati[j] ii corespunde culoarea j+1
j = 0;
    // i este coordonata x de unde se incepe ficare patratel colorat pe care il vom desena
    for( int i = 1; i<=getmaxx() - getmaxx()/14; i+=getmaxx()/14 )
    {

            //daca e culoarea tablei(verde) sau piesa e de aceeasi culoare cu celalt player
            if( j == culoare_player[1]-1 || j+1 == 10 )
            {
                    //extremitatile sunt coordonatele x1, x2, get rectangle(x1, y1, x1, y2)
                    extremitari_x[j].i = -1;
                    extremitari_x[j].j = -1;
                    j++;
                    continue;
            }

            setcolor(j+1);
            setfillstyle(0, j+1);
            rectangle( i, getmaxy() - getmaxy()/6 + 50, i+getmaxx()/16,  getmaxy() - getmaxy()/6 + 80 );

            extremitari_x[j].i = i;
            extremitari_x[j].j = i+getmaxx()/16;

            j++;

    }



    gata = false;

    do{

       if(ismouseclick(WM_LBUTTONDOWN))
            {
                clearmouseclick(WM_LBUTTONDOWN);
                int x_mouse=mousex();
                int y_mouse=mousey();

                if( y_mouse > getmaxy() - getmaxy()/6 + 50 && y_mouse < getmaxy() - getmaxy()/6 + 80 )
                {


                    PlaySound("keyboard.wav", NULL, SND_FILENAME|SND_ASYNC);

                    for( int i = 0; i<14; i++ )
                    {

                        if( x_mouse>extremitari_x[i].i && x_mouse<extremitari_x[i].j )
                        {

                                char t12[] = "X";

                                setcolor(i+1);
                                settextjustify(0, 0);
                                settextstyle(4, HORIZ_DIR, 2);
                                outtextxy( ( extremitari_x[i].i + extremitari_x[i].j )/2, getmaxy() - getmaxy()/6 + 65 , t12 );

                                culoare_player[2] = i+1;


                                gata  = true;
                                break;


                        }

                    }


                }

            }



    }while(!gata);



}else{
    int j = 0;
    for( unsigned int i = 0; i<14; i++ )
    {

        if( i != culoare_player[1]-1 && j+1 != 10 )
        {
            culoare_player[2] = i+1;

            break;
        }

    }

}
culoare_rezerva[1] = culoare_player[1];
culoare_rezerva[2] = culoare_player[2];

    //vedem daca exista in bilant confruntarea acestor 2 playeri

    index_bilant_activ = -1;
    for( unsigned int i = 0; i<nr_bilanturi; i++ )
    {

        if( (  strcmp( jucatori[0], bilanturi[i].player1  ) == 0 && strcmp( jucatori[1], bilanturi[i].player2 )==0  )  || (  strcmp( jucatori[1], bilanturi[i].player1  ) == 0 && strcmp( jucatori[0], bilanturi[i].player2 ) == 0  ) )
        {

            index_bilant_activ = i;
            break;
        }

    }
    cout<<"\n"<<index_bilant_activ<<"\n";
    if( index_bilant_activ == -1 && nr_bilanturi < max_library )
    {
        cout<<"\n bilant adaugat "<<nr_bilanturi<<"\n";
        strcpy( bilanturi[nr_bilanturi].player1, jucatori[0] );
        strcpy( bilanturi[nr_bilanturi].player2, jucatori[1] );

        bilanturi[nr_bilanturi].remize = 0;
        bilanturi[nr_bilanturi].scor_playeri[0] = 0;
        bilanturi[nr_bilanturi].scor_playeri[1] = 0;

        index_bilant_activ = nr_bilanturi;

        nr_bilanturi += 1;

        ofstream fout;

        fout.open("scoruri_arhiva.txt");

        if (fout)
        {

            fout<<nr_bilanturi<<"\n";
            for( unsigned int i = 0; i<nr_bilanturi; i++ )
            {
                fout<<bilanturi[i].player1<<" ";
                fout<<bilanturi[i].player2<<" ";
                fout<<bilanturi[i].scor_playeri[0]<<" ";
                fout<<bilanturi[i].scor_playeri[1]<<" ";
                fout<<bilanturi[i].remize<<"\n";

            }


        }

        fout.close();

    }


    clearviewport();


}

void int_to_text( int nr, char a[] )
{
    int poz = 0;

    while(nr>0)
    {

        a[poz] = nr%10 + 48;
        poz++;
        a[poz] = NULL;

        nr/=10;

    }

    int i = 0;
    poz = poz-1;

    while(i<poz)
    {

        char aux = a[i];
        a[i] = a[poz];
        a[poz] = aux;
        i++;
        poz--;
    }

}

void scor_bilanturi(int start_index )
{
    clearviewport();
    if( nr_bilanturi == 0 )
    {
        setcolor(14);
        setlinestyle(3, 0, 3);

        //char text[50]="NO PLAYERS FOR NOW...";
        settextjustify(1,1);
        settextstyle(4, HORIZ_DIR, 2);
        outtextxy(getmaxx()/2, getmaxy()/4, cuvinte[ 23 ]);

    }else{

        setcolor(14);
        setlinestyle(3, 0, 3);

        int spp = 100;

        //char text_player1[50]="PLAYER 1";
        settextjustify(LEFT_TEXT,1);
        settextstyle(4, HORIZ_DIR, 2);
        outtextxy(10, spp, cuvinte[34]);

        setcolor(14);
        setlinestyle(3, 0, 3);

        //char text_player2[50]="PLAYER 2";
        settextjustify(LEFT_TEXT,1);
        settextstyle(4, HORIZ_DIR, 2);
        outtextxy(getmaxx()/4, spp, cuvinte[35]);

       // char text_victories[50]="VICTORIES";
        settextjustify(1,1);
        settextstyle(4, HORIZ_DIR, 2);
        outtextxy( getmaxx()/2 + getmaxx()/8, 50, cuvinte[ 24 ]);


        //char text_1[50]="PL_1";
        settextjustify(LEFT_TEXT,1);
        settextstyle(4, HORIZ_DIR, 2);
        outtextxy( getmaxx()/2, spp, cuvinte[ 26 ]);

        //char text_2[50]="PL_2";
        settextjustify(LEFT_TEXT,1);
        settextstyle(4, HORIZ_DIR, 2);
        outtextxy( getmaxx()/2 + getmaxx()/6, spp, cuvinte[ 27 ]);

        //char text_3[50]="REM";
        settextjustify(LEFT_TEXT,1);
        settextstyle(4, HORIZ_DIR, 2);
        outtextxy( getmaxx() - getmaxx()/6, spp,cuvinte[ 25 ]);

        int expp = 50;



        for( unsigned int i = start_index; i<nr_bilanturi && i<=start_index+11; i++ )
        {

            setcolor(11);
            settextjustify(LEFT_TEXT,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy( 10, spp+expp, bilanturi[i].player1);

            setcolor(9);
            settextjustify(LEFT_TEXT,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy( getmaxx()/4, spp+expp, bilanturi[i].player2);

            char a[40]="0      ";
            char b[40]="0      ";
            char c[40]="0      ";

            int num1 = bilanturi[i].scor_playeri[0];
            int num2 = bilanturi[i].scor_playeri[1];
            int num3 = bilanturi[i].remize;

            //convertesc numerele in text

            int_to_text(num1, a);
            int_to_text(num2, b);
            int_to_text(num3, c);

            setcolor(10);
            settextjustify(LEFT_TEXT,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy( getmaxx()/2, spp+expp, a);

            setcolor(2);
            settextjustify(LEFT_TEXT,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy( getmaxx()/2 + getmaxx()/6, spp+expp, b);

            setcolor(8);
            settextjustify(LEFT_TEXT,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy( getmaxx() - getmaxx()/6, spp+expp, c);

            spp = spp+expp;

        }


    }


    setcolor(14);
    setlinestyle(3, 0, 3);
    rectangle( 10, 5, 100, 40 );

    //char text_menu[]="Menu";
    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy(55, 30, cuvinte[ 8 ]);

}

void instr()
{
    clearviewport();





      setcolor(9);
      settextstyle(3, HORIZ_DIR, 70);
      outtextxy(getmaxx()/2, getmaxy()/6, cuvinte[2] );

      //char text3[] = "CUGO e un joc intre doi adversari cu un set de 8 piese pentru fiecare jucator.";
      setcolor(9);
       settextjustify(LEFT_TEXT,1);
      settextstyle(3, HORIZ_DIR,80);
      outtextxy(32, 171, cuvinte[ 28 ] );

      //char text4[] = " Jocul are doua runde iar scopul acestuia este de a incercui piesele adversarului prin inserari repetate ale pieselor(runda 1),";
      setcolor(9);
      settextjustify(LEFT_TEXT,1);
      settextstyle(3, HORIZ_DIR,80);
      outtextxy(32, 214, cuvinte[ 29 ] );


      //char text5[] = " sau mutari repetate(runda 2).Desigur,pot exista cazuri de remiza,atunci cand fiecare jucator a incercuit acelasi numar de piese ";
      setcolor(9);
      settextjustify(LEFT_TEXT,1);
      settextstyle(3, HORIZ_DIR,80);
      outtextxy(32, 278, cuvinte[ 30 ] );

    //char text6[] = " ca si adversarul sau,caz posibil doar in prima runda.Se poate juca impotriva calculatorului sau impotriva altui jucator.";
      setcolor(9);
      settextjustify(LEFT_TEXT,1);
      settextstyle(3, HORIZ_DIR,80);
      outtextxy(32, 314, cuvinte[ 31 ] );



       ///desenare buton play
    setcolor(14);
    setlinestyle(3, 0, 3);

    rectangle( getmaxx()/1.4, getmaxy()/1.4, getmaxx()-getmaxx()/6, getmaxy() - getmaxy()/7);


    //char text[]="Meniu";
    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy(getmaxx()/1.3, getmaxy()/1.3, cuvinte[ 8 ]);
    bool gata = false;


    readimagefile("unu.bmp",35 ,387 ,435 ,695 );
readimagefile("doi.bmp",508 ,387 ,818 ,695 );


    char winner[50];
    strcpy(winner, cuvinte[ 32 ]);

    char name[] = "ION";

    strcat(winner, name);
    outtextxy( 215, 680, winner  );

    outtextxy( 662, 690, cuvinte[25] );

    do{
        ///schimbam niste lucruri cand playerul pune cursorul pe buton
        if( mousex()>=getmaxx()/1.4 && mousex()<=getmaxx()-getmaxx()/6 && mousey()>=getmaxy()/1.4 && mousey()<=getmaxy() - getmaxy()/7 )
        {
            setcolor(15);
            setlinestyle(3, 0, 3);
            rectangle( getmaxx()/1.4, getmaxy()/1.4, getmaxx()-getmaxx()/6, getmaxy() - getmaxy()/7 );

            //char text[]="Meniu";
            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy(getmaxx()/1.3, getmaxy()/1.3, cuvinte[ 8 ]);

        }else{

            setcolor(14);
            setlinestyle(3, 0, 3);
            setfillstyle(1, 2);
            rectangle( getmaxx()/1.4, getmaxy()/1.4, getmaxx()-getmaxx()/6, getmaxy() - getmaxy()/7 );

            //char text[50]="Meniu";
            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy(getmaxx()/1.3, getmaxy()/1.3, cuvinte[ 8 ]);


        }

       if(ismouseclick(WM_LBUTTONDOWN))
        {
                    clearmouseclick(WM_LBUTTONDOWN);
                    int x_mouse=mousex();
                    int y_mouse=mousey();
                    cout<<endl<<endl<< x_mouse<<" "<<y_mouse;
                    if( x_mouse>=getmaxx()/1.4 && x_mouse<=getmaxx()-getmaxx()/6 && y_mouse>=getmaxy()/1.4 && y_mouse<=getmaxy() - getmaxy()/7)
                        {
                            PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                            gata = true;

                            //cout<<"\n"<<x_mouse<<" "<<y_mouse<<"\n";
                        }


        }





}while(!gata);
}

char limbi_in_limba_lor[12][20];

void initializare_char_limbi_in_limba_lor()
{

    ifstream fin;

    int i = 0;
    fin.open("limbi_in_limba_lor.txt");

    if (fin)
    {
        cout<<"\n limbi in limba lor \n";
        fin>>limbi_in_limba_lor[i];

        while( fin>>limbi_in_limba_lor[i] )
        {
            cout<<limbi_in_limba_lor[i]<<endl;
            i++;


        }
        cout<<"gata \n limbi in limba lor \n";

    }

    fin.close();

}

void desenare_buton_limba( int x1, int x2, int y1, int y2, int ind_limba )
{


    setcolor(14);
    setlinestyle(3, 0, 3);
    rectangle( x1, y1, x2, y2 );

    char a[20];

    if( ind_limba>10 )
        strcpy(a, cuvinte[8]);
    else{
    strcpy(a,limbi_in_limba_lor[ind_limba]  );
    }

    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( (x1+x2)/2, (y1+y2)/2, a);

}

bool atinge_cursor( int x_mouse, int y_mouse, int latime, int butoane[], int ind_buton, int x1, int x2 )
{

    if( x_mouse>=x1 && x_mouse<=x2 && y_mouse>=butoane[ind_buton] && y_mouse<=butoane[ind_buton]+latime  )
    {
        setcolor(15);
        setlinestyle(3, 0, 3);
        rectangle( x1, butoane[ind_buton], x2, butoane[ind_buton]+latime );

        char a[20];

        if( ind_buton>10 )
            strcpy(a, cuvinte[8]);
        else{
        strcpy(a,limbi_in_limba_lor[ind_buton]  );


        }
        settextjustify(1,1);
        settextstyle(4, HORIZ_DIR, 2);
        outtextxy( (x1+x2)/2, (butoane[ind_buton]+butoane[ind_buton]+latime)/2, a);

        cout<<endl<<ind_buton<<"hhhhhhhhhh"<<endl;
        if(ind_buton<=10)
            {

                ofstream fout;

                fout.open("lista_limbi.txt");

                if (fout)
                {

                    fout<<ind_buton;
                    for( unsigned int i = 0; i<12; i++ )
                    {
                       fout<<"\n";
                       fout<<limbi[i];

                    }


                }

                fout.close();

            }
        delay(100);
        return true;
    }
    return false;
}

void afisare_limbi(int latime )
{
    clearviewport();
    int y = getmaxy()/20;
    int ind_limba = 0;

    int butoane[12];
    int ind_buton = 0;


    for( unsigned int i = 0; i<6; i++  )
    {

        desenare_buton_limba(getmaxx()/20, getmaxx()/2 - 5, y, y+latime, ind_limba );

        butoane[ind_buton] = y;
        ind_buton++;
        ind_limba++;
        y+=latime+10;

    }

    y = getmaxy()/20;
    for( unsigned int i = 0; i<6; i++  )
    {

        desenare_buton_limba(getmaxx()/2 + 5, getmaxx() - getmaxx()/20, y, y+latime, ind_limba );
        butoane[ind_buton] = y;

        ind_buton++;

        ind_limba++;
        y+=latime+10;

    }

    bool gata = false;

    do{

        if(ismouseclick(WM_LBUTTONDOWN))
        {

            clearmouseclick(WM_LBUTTONDOWN);
            int x_mouse=mousex();
            int y_mouse=mousey();
            int i =0;

            while( i<6 && !gata)
                {
                        gata = atinge_cursor(x_mouse, y_mouse, latime, butoane, i, getmaxx()/20, getmaxx()/2-5 );
                        i++;
                }

            while( i<12 && !gata)
                {
                        gata = atinge_cursor(x_mouse, y_mouse, latime, butoane, i, getmaxx()/2+5, getmaxx() - getmaxx()/20 );
                        i++;
                }


        }

    }while(!gata);

}


void meniu(bool &exit)
{

    clearviewport();
    //desenare buton play
    setcolor(14);
    setlinestyle(3, 0, 3);

    rectangle( getmaxx()/4, getmaxy()/4, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/2 );


    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy(getmaxx()/2, (getmaxy()/4 + getmaxy()-getmaxy()/2)/2, cuvinte[0]);



    char text2[] = ".................................~ C U G O ~.................................";
    setcolor(9);
    settextstyle(3, HORIZ_DIR, 70);
    outtextxy(getmaxx()/2, getmaxy()/6, text2 );


    //desenare buton single play
    setcolor(14);
    setlinestyle(3, 0, 3);

    rectangle( getmaxx()/4, getmaxy()/2 + 10, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/4 );



    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy(getmaxx()/2, (getmaxy()/2+10 + getmaxy()-getmaxy()/4)/2,cuvinte[1]);


    //desenare buton Ajutor
    setcolor(14);
    setlinestyle(3, 0, 3);

    rectangle( getmaxx()/4, getmaxy() - getmaxy()/4 + 10, getmaxx()-getmaxx()/2, getmaxy() - getmaxy()/8 );

    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy(getmaxx()/4+ getmaxx()/8, (getmaxy() - getmaxy()/4 + 10 + getmaxy() - getmaxy()/8)/2, cuvinte[2]);

    //desenare buton scoruri
    setcolor(14);
    setlinestyle(3, 0, 3);

    rectangle( getmaxx()/4, getmaxy() - getmaxy()/8 + 10, getmaxx()-getmaxx()/2, getmaxy() - getmaxy()/25 );

    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( (getmaxx()/4 + getmaxx()/2) / 2, (getmaxy() - getmaxy()/8 + 10 + getmaxy() - getmaxy()/25 )/2, cuvinte[4]);



    rectangle( getmaxx()/2+7, getmaxy() - getmaxy()/8 + 10, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/25 );


    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( (getmaxx()/2+getmaxx() - getmaxx()/4)/2, (getmaxy() - getmaxy()/8 + 10 + getmaxy() - getmaxy()/25 )/2, cuvinte[7]);






    rectangle( getmaxx()/2+7, getmaxy() - getmaxy()/4 + 10, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/8 );
    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( (getmaxx()/2+getmaxx() - getmaxx()/4)/2, (getmaxy() - getmaxy()/4 + 10 + getmaxy() - getmaxy()/8)/2, cuvinte[3]);

    bool gata = false;

    do{
        //schimbam niste lucruri cand playerul pune cursorul pe buton PLAY
        if( mousex()>=getmaxx()/4 && mousex()<=getmaxx()-getmaxx()/4 && mousey()>=getmaxy()/4 && mousey()<=getmaxy() - getmaxy()/2 )
        {
            setcolor(15);
            setlinestyle(3, 0, 3);
            rectangle( getmaxx()/4, getmaxy()/4, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/2 );

            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy(getmaxx()/2, (getmaxy()/4 + getmaxy()-getmaxy()/2)/2, cuvinte[0]);

        }else{

            setcolor(14);
            setlinestyle(3, 0, 3);
            setfillstyle(1, 2);
            rectangle( getmaxx()/4, getmaxy()/4, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/2 );

            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy(getmaxx()/2, (getmaxy()/4 + getmaxy()-getmaxy()/2)/2, cuvinte[0]);


        }


        //schimbam niste lucruri cand playerul pune cursorul pe buton SINGLE PLAY
        if( mousex()>=getmaxx()/4 && mousex()<=getmaxx()-getmaxx()/4 && mousey()>=getmaxy()/2 + 10 && mousey()<=getmaxy() - getmaxy()/4 )
        {
            setcolor(15);
            setlinestyle(3, 0, 3);
            rectangle( getmaxx()/4, getmaxy()/2 + 10, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/4 );

            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy(getmaxx()/2, (getmaxy()/2+10 + getmaxy()-getmaxy()/4)/2, cuvinte[1]);

        }else{

            setcolor(14);
            setlinestyle(3, 0, 3);
            setfillstyle(1, 2);
            rectangle( getmaxx()/4, getmaxy()/2 + 10, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/4 );

            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy(getmaxx()/2, (getmaxy()/2+10 + getmaxy()-getmaxy()/4)/2, cuvinte[1]);


        }

        //schimbam niste lucruri cand playerul pune cursorul pe buton INSTRUCTIUNI
        if( mousex()>=getmaxx()/4 && mousex()<=getmaxx()-getmaxx()/2 && mousey()>=getmaxy() - getmaxy()/4 + 10 && mousey()<=getmaxy() - getmaxy()/8 )
        {
            setcolor(15);
            setlinestyle(3, 0, 3);
            rectangle( getmaxx()/4, getmaxy() - getmaxy()/4 + 10, getmaxx()-getmaxx()/2, getmaxy() - getmaxy()/8 );

            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
             outtextxy(getmaxx()/4+ getmaxx()/8, (getmaxy() - getmaxy()/4 + 10 + getmaxy() - getmaxy()/8)/2, cuvinte[2]);

        }else{

            setcolor(14);
            setlinestyle(3, 0, 3);
            setfillstyle(1, 2);
            rectangle( getmaxx()/4, getmaxy() - getmaxy()/4 + 10, getmaxx()-getmaxx()/2, getmaxy() - getmaxy()/8 );



            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
             outtextxy(getmaxx()/4+ getmaxx()/8, (getmaxy() - getmaxy()/4 + 10 + getmaxy() - getmaxy()/8)/2, cuvinte[2]);


        }

        //schimbam niste lucruri cand pl                            afisare_limbi(100);ayerul pune cursorul pe buton SCORES
        if( mousex()>=getmaxx()/4 && mousex()<=getmaxx()-getmaxx()/2 && mousey()>=getmaxy() - getmaxy()/8 + 10 && mousey()<=getmaxy() - getmaxy()/25 )
        {
            setcolor(15);
            setlinestyle(3, 0, 3);

            rectangle( getmaxx()/4, getmaxy() - getmaxy()/8 + 10, getmaxx()-getmaxx()/2, getmaxy() - getmaxy()/25 );

            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy( (getmaxx()/4 + getmaxx()/2) / 2, (getmaxy() - getmaxy()/8 + 10 + getmaxy() - getmaxy()/25 )/2,cuvinte[4]);

        }else{

            setcolor(14);
            setlinestyle(3, 0, 3);
            setfillstyle(1, 2);

            rectangle( getmaxx()/4, getmaxy() - getmaxy()/8 + 10, getmaxx()-getmaxx()/2, getmaxy() - getmaxy()/25 );

            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy( (getmaxx()/4 + getmaxx()/2) / 2, (getmaxy() - getmaxy()/8 + 10 + getmaxy() - getmaxy()/25 )/2, cuvinte[4]);

        }

        //schimbam niste lucruri cand playerul pune cursorul pe buton EXIT
        if( mousex()>=getmaxx()/2+7 && mousex()<=getmaxx()-getmaxx()/4 && mousey()>=getmaxy() - getmaxy()/8 + 10 && mousey()<=getmaxy() - getmaxy()/25 )
        {
            setcolor(15);
            setlinestyle(3, 0, 3);

            rectangle( getmaxx()/2+7, getmaxy() - getmaxy()/8 + 10, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/25 );


            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy( (getmaxx()/2+getmaxx() - getmaxx()/4)/2, (getmaxy() - getmaxy()/8 + 10 + getmaxy() - getmaxy()/25 )/2, cuvinte[7]);

        }else{

            setcolor(14);
            setlinestyle(3, 0, 3);
            setfillstyle(1, 2);


            rectangle( getmaxx()/2+7, getmaxy() - getmaxy()/8 + 10, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/25 );

            settextjustify(1,1);
            settextstyle(4, HORIZ_DIR, 2);
            outtextxy( (getmaxx()/2+getmaxx() - getmaxx()/4)/2, (getmaxy() - getmaxy()/8 + 10 + getmaxy() - getmaxy()/25 )/2, cuvinte[7]);

        }

        //schimbam niste lucruri cand playerul pune cursorul pe buton limbi
        if( mousex()>=getmaxx()/2+7 && mousex()<=getmaxx()-getmaxx()/4 && mousey()>=getmaxy() - getmaxy()/4 + 10 && mousey()<=getmaxy() - getmaxy()/8 )
        {
            setcolor(15);
            setlinestyle(3, 0, 3);

    rectangle( getmaxx()/2+7, getmaxy() - getmaxy()/4 + 10, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/8 );
    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( (getmaxx()/2+getmaxx() - getmaxx()/4)/2, (getmaxy() - getmaxy()/4 + 10 + getmaxy() - getmaxy()/8)/2, cuvinte[3]);

        }else{

            setcolor(14);
            setlinestyle(3, 0, 3);
            setfillstyle(1, 2);


    rectangle( getmaxx()/2+7, getmaxy() - getmaxy()/4 + 10, getmaxx()-getmaxx()/4, getmaxy() - getmaxy()/8 );
    settextjustify(1,1);
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy( (getmaxx()/2+getmaxx() - getmaxx()/4)/2, (getmaxy() - getmaxy()/4 + 10 + getmaxy() - getmaxy()/8)/2, cuvinte[3]);

        }

       if(ismouseclick(WM_LBUTTONDOWN))
                {

                    clearmouseclick(WM_LBUTTONDOWN);
                    int x_mouse=mousex();
                    int y_mouse=mousey();

                    //buton play
                    if( x_mouse>=getmaxx()/4 && x_mouse<=getmaxx()-getmaxx()/4 && y_mouse>=getmaxy()/4 && y_mouse<=getmaxy() - getmaxy()/2 )
                    {

                            PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);

                            clearviewport();
                            delay(250);
                            input_linie_si_coloana(false);
                            stabilirea_latura_casuta_tabla_dupa_fereastra();
                            play(false);

                            gata = true;
                            break;

                    }

                    //buton single SINGLE PLAY
                    if( x_mouse>=getmaxx()/4 && x_mouse<=getmaxx()-getmaxx()/4 && y_mouse>=getmaxy()/2 + 10 && y_mouse<=getmaxy() - getmaxy()/4 )
                    {


                            PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);

                            clearviewport();
                            delay(250);

                            input_linie_si_coloana(true);
                            stabilirea_latura_casuta_tabla_dupa_fereastra();

                            play(true);

                            gata = true;

                            break;

                    }


                    //buton single INSTRUCTION
                    if( x_mouse>=getmaxx()/4 && x_mouse<=getmaxx()-getmaxx()/2 && y_mouse>=getmaxy() - getmaxy()/4 + 10 && y_mouse<=getmaxy() - getmaxy()/8 )
                    {


                            PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);
                            instr();
                            gata = true;

                    }

                    //buton scor-bilanturi
                    if( x_mouse>=getmaxx()/4 && x_mouse<=getmaxx()-getmaxx()/2 && y_mouse>=getmaxy() - getmaxy()/8 + 10 && y_mouse<=getmaxy() - getmaxy()/25 )
                    {


                        PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);

                        int start_index = 0;

                        scor_bilanturi(start_index);



                        setcolor(0);
                        rectangle( 105, 5, 200, 40 );

                        settextjustify(1,1);
                        settextstyle(4, HORIZ_DIR, 2);
                        outtextxy(147, 30, cuvinte[6]);


                        if( nr_bilanturi > start_index )
                            {
                                setcolor(14);

                                rectangle( 205, 5, 300, 40 );

                                settextjustify(1,1);
                                settextstyle(4, HORIZ_DIR, 2);
                                outtextxy(247, 30, cuvinte[5]);

                            }

                        bool gata2 = false;

                        do{

                            if(ismouseclick(WM_LBUTTONDOWN))
                            {

                                clearmouseclick(WM_LBUTTONDOWN);
                                int x_mouse=mousex();
                                int y_mouse=mousey();

                                if( x_mouse>=10 && x_mouse<=100 && y_mouse>=5 && y_mouse<=40 )
                                {


                                    PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);

                                    gata2 = true;

                                }


                                //buton "prev"
                                if( x_mouse>=105 && x_mouse<=200 && y_mouse>=5 && y_mouse<=40 )
                                {



                                    if( start_index > 11 )
                                    {

                                        PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);

                                        start_index -= 12;
                                        scor_bilanturi(start_index);



                                        if( start_index == 0 )
                                        {

                                            setcolor(0);
                                            rectangle( 105, 5, 200, 40 );
                                            settextjustify(1,1);
                                            settextstyle(4, HORIZ_DIR, 2);
                                            outtextxy(147, 30, cuvinte[6]);

                                        }else{

                                                setcolor(14);
                                                rectangle( 105, 5, 200, 40 );
                                                settextjustify(1,1);
                                                settextstyle(4, HORIZ_DIR, 2);
                                                outtextxy(147, 30, cuvinte[6]);

                                        }

                                        setcolor(14);
                                        rectangle( 205, 5, 300, 40 );
                                        settextjustify(1,1);
                                        settextstyle(4, HORIZ_DIR, 2);
                                        outtextxy(247, 30, cuvinte[5]);

                                    }

                                }

                                //butonul "next"
                                if( x_mouse>=205 && x_mouse<=300 && y_mouse>=5 && y_mouse<=40 )
                                {




                                    if( nr_bilanturi > start_index + 12 )
                                    {

                                        PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);

                                        start_index+=12;
                                        scor_bilanturi(start_index);


                                        cout<<"\n"<<start_index<<"\n";
                                        if( nr_bilanturi <= start_index + 12 )
                                        {

                                            setcolor(0);

                                            rectangle( 205, 5, 300, 40 );

                                            settextjustify(1,1);
                                            settextstyle(4, HORIZ_DIR, 2);
                                            outtextxy(247, 30, cuvinte[5]);

                                        }else{

                                                setcolor(14);
                                                rectangle( 205, 5, 300, 40 );
                                                settextjustify(1,1);
                                                settextstyle(4, HORIZ_DIR, 2);
                                                outtextxy(247, 30, cuvinte[5]);

                                        }

                                        setcolor(14);
                                        rectangle( 105, 5, 200, 40 );
                                        settextjustify(1,1);
                                        settextstyle(4, HORIZ_DIR, 2);
                                        outtextxy(147, 30, cuvinte[6]);


                                    }

                                }

                            }




                        }while(!gata2);



                        gata = true;
                    }

                    //EXIT BUTON
                    if( x_mouse>=getmaxx()/2+7 && x_mouse<=getmaxx()-getmaxx()/4 && y_mouse>=getmaxy() - getmaxy()/8 + 10 && y_mouse<=getmaxy() - getmaxy()/25 )
                    {

                        exit = true;

                        gata = true;

                    }

                    //buton single limbi
                    if( x_mouse>=getmaxx()/2+7 && x_mouse<=getmaxx()-getmaxx()/4 && y_mouse>=getmaxy() - getmaxy()/4 + 10 && y_mouse<=getmaxy() - getmaxy()/8 )
                    {


                            PlaySound("click.wav", NULL, SND_FILENAME|SND_ASYNC);

                            afisare_limbi(100);
                            gata = true;

                    }




                }

    }while(!gata);


}

int main()
{

    ifstream fin;

    fin.open("scoruri_arhiva.txt");

    if (fin)
    {

        fin>>nr_bilanturi;
        char a[100];
        int x;

        for( unsigned int i = 0; i<nr_bilanturi; i++ )
        {

            fin>>a;
            strcpy( bilanturi[i].player1, a );


            fin>>a;
            strcpy( bilanturi[i].player2, a );

            fin>>x;
            bilanturi[i].scor_playeri[0] = x;


            fin>>x;
            bilanturi[i].scor_playeri[1] = x;


            fin>>x;
            bilanturi[i].remize = x;

        }


    }

    fin.close();

    char lpszReturnString[100];

    //type mpegvideo E FOARTE IMPORTANT, pt ca numai asa se poate REPETA, repetitia nu merge cu wav
    mciSendString( "open muzica_de_fundal.wav type mpegvideo alias music", lpszReturnString, lstrlen(lpszReturnString), NULL);
    mciSendString("play music repeat", NULL, 0, NULL);

//    PlaySound("muzica_de_fundal.wav", NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
//    PlaySound("muzica_de_fundal.wav", NULL, SND_FILENAME|SND_ASYNC);

    //nr_linii=5, nr_col=10; // latura e adaptabila, depinde de n si m si cat loc avem pe ecran

    initwindow(1250, 730);

    //pt afisare butoane cu limbi
    initializare_char_limbi_in_limba_lor();

    //vezi sa intorci eroare daca max(n,m) < 100

    bool gata = false;

    //daca s-a apasat pe butonul meniu atunci functia meniu intoarce adevarat si se reapeleaza functia meniu() din nou
    do{

        inregistrare_limbi();
        inregistrare_cuvinte();

        meniu(gata);
        closegraph();

        if(!gata)
        {


            initwindow(1250, 730);

        }


    }while(!gata);


    return 0;

}

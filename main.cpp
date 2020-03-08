//
//  pairWar.cpp
//
//  Created by: Connor Widder and Jaylion McMillan
//  Course: CS4328
//  Assigned: 02/06/2020
//  Due/Submitted: 03/09/2020
//  Title: Assignment #1 - Pair War
//  Description:
    /*  Pair War is a simple card game with one dealer, 3 players, and a single deck of cards. The game is composed of three rounds.
        At the beginning of each round, the dealer shuffles the cards, deals the first three cards (one per player) and waits for the
        round to finish, before repeating the same process for the next round.
     
        A winner in a round is the first player to get a “pair”. In each round, a different player is given the privilege to start.
        Initially, the dealer shuffles the deck of cards and hands each player a single card in a round robin fashion (say, starting
        from player 1 for round 1). Once the dealer is done handling the cards, the dealer places the remaining deck of cards on the
        table and the first round begins. Each player (starting from player 1), draws a card from the deck and compares it to the card
        he/she has. If they are a pair, the player shows the hand, declares him/herself a winner and the round ends. Otherwise, the
        player will discard one card (at random) by placing it at the bottom of the deck of cards on the table, and the next player proceeds.
     
        Once a round ends, the dealer shuffles the deck again and hands a card to each player. In the second round, the second player
        starts drawing a card from the deck. In the third round, the third player starts    drawing a card from the deck.
     */


#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <time.h>

#define THREAD_NUM 4

using namespace std;

//Threads
pthread_t playerThreads[3];
pthread_t dealerThread;
pthread_cond_t cond_win = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_dealerExit = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_useDeck = PTHREAD_MUTEX_INITIALIZER;


//Varible Definitions
FILE *logFile;
int deck[52];
int *deckTop;
int *deckBottom;
int roundNum;
bool win = false;
int turn = 0;

struct hand{
    int card1, card2;
};
hand hand1, hand2, hand3;

//Function Definitions
void *dealer_Thread(void *arg);
void *player_Threads(void *arg);
void deckBuild();
void deckShuffle();
void deckDeal();
void deckPrint();
void deckDealer(long, hand);
void round();

//Main
int main(int argc, const char * argv[]) {
    logFile = freopen("pairWarOutput.txt","w", stdout);   //Create the log file

    cout << "Welcome to Pair War" << endl;
    cout << "See Log File" << endl;
    srand(time(NULL));
    cout << "----Start----" << endl;
    deckBuild();
    deckShuffle();
    
    while (roundNum < 4){
        round();
        roundNum++;
        win = false;
    }

    fprintf(logFile,"----End----\n");
    fclose(logFile);
    exit(EXIT_SUCCESS);
}

void deckBuild(){
    cout << "building deck..." << endl;
    
    int card = 0;
    for (int i = 0; i < 4; i++){
        for (int j =0; j < 13; j++){
            deck[card] = j;
            card++;
        }
    }
    deckTop = deck;
    deckBottom = deck + 51;
}

void deckShuffle(){
    for( int x = 0; x < (52 - 1); x++){
        int randomCard = x + (rand() % (52 - x));
        int cardHolder = deck[x];
        deck[x] = deck[randomCard];
        deck[randomCard] = cardHolder;
    }
}

void deckDeal(){
    hand1.card1 = *deckTop;
    deckTop = deckTop + 1;
    
    hand2.card1 = *deckTop;
    deckTop = deckTop + 1;
    
    hand3.card1 = *deckTop;
    deckTop = deckTop + 1;
}

void deckPrint(){
    cout << "DECK: ";
    int *ptr = deckTop;
    while (ptr != deckBottom){
        cout << *ptr << " ";
        ptr++;
    }
    cout << *ptr << endl;
}

void round(){
    cout << endl << "Beginning Round" << endl;
    
    pthread_create(&dealerThread,NULL,&dealer_Thread,NULL);     //Create dealer thread
    int player;
    for (long i = 1; i <= 3; i++){
        player = pthread_create(&playerThreads[i], NULL, &player_Threads, (void *)i);
    }
    
    pthread_join(dealerThread, NULL);
    for( int j = 0; j < 3; j++){
        pthread_join(playerThreads[j], NULL);
    }
    
}

void *dealer_Thread(void *arg){
    int pNum = 0;   //Dealers number
    turn = 0;
    hand dealerHand;
    deckDealer(pNum, dealerHand);
    
    pthread_mutex_lock(&mutex_dealerExit);
    while(!win){
        pthread_cond_wait(&cond_win, &mutex_dealerExit);
    }
    pthread_mutex_unlock(&mutex_dealerExit);
    cout << "DEALER: exits round" << endl;
    pthread_exit(NULL);
}

void *player_Threads(void *playerNum){
    long pNum = (long) playerNum;
    hand currentHand;
    
    if(roundNum == 1){
        
        if(pNum == 1) currentHand = hand1;
        else if (pNum == 2) currentHand = hand2;
        else currentHand = hand3;
        
    }else if( roundNum == 2){
        
        if(pNum == 2) currentHand = hand1;
        else if (pNum == 3) currentHand = hand2;
        else currentHand = hand3;
        
    }else if( roundNum == 3){
        
        if(pNum == 3) currentHand = hand1;
        else if (pNum == 1) currentHand = hand2;
        else currentHand = hand3;
    }
    
    while(win == 0){
        pthread_mutex_lock(&mutex_useDeck);
        while(pNum != turn && win == 0){
            pthread_cond_wait(&cond_var, &mutex_useDeck);
        }
        if (win == 0){
            deckDealer(pNum, currentHand);
        }
        pthread_mutex_unlock(&mutex_useDeck);
    }
    cout << "PLAYER " << pNum << ": exits round" << endl;
    pthread_exit(NULL);
}

void deckDealer(long pNum, hand currentHand){
    if (pNum == 0){    //Dealers turn
        cout << "DEALER: shuffle deck" << endl;
        deckShuffle();
        cout << "DEALER: deal cards" << endl;
        deckDeal();
    }else{  //Players turn
        
        cout << "PLAYER " << pNum << ": hand " << currentHand.card1 << endl;   //Show the players hand
        
        currentHand.card2 = *deckTop;   //Draw a card
        deckTop = deckTop + 1;          //Set new top of deck card
        cout << "PLAYER " << pNum << ": draws " << currentHand.card2 << endl;   //Player draws a card
        
        cout << "PLAYER " << pNum << ": hand " << currentHand.card1 << " " << currentHand.card2 << endl;   //Show the players hand
        
        if(currentHand.card1 == currentHand.card2){
            //Cards match, delare the winner
            cout << "WIN yes" << endl;
            win = true;
            pthread_cond_signal(&cond_win); //TODO: check cond_signal names
        }else{
            //Card don't match, discard the cards
            cout << "WIN no" << endl;
            
            deckTop = deckTop - 1;
            int *ptr = deckTop;
            while (ptr != deckBottom){
                *ptr = *(ptr + 1);
                ptr = ptr + 1;
            }
            
            int discard = rand()%2;
            if (discard == 0){
                cout << "PLAYER " << pNum << " discards " << currentHand.card1 << endl;
                *deckBottom = currentHand.card1;
                currentHand.card1 = currentHand.card2;
            }else{
                cout << "PLAYER " << pNum << " discards " << currentHand.card2 << endl;
                *deckBottom = currentHand.card2;
            }
            deckPrint();
        }
    }
    turn ++;
    if(turn > 3){
        turn = 1;
    }
    pthread_cond_broadcast(&cond_var);
}



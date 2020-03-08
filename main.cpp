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
#define THREAD_NUM 4

using namespace std;

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
void *player_Thread(void *arg);
void deckBuild();
void deckShuffle();
void deckDeal();
void deckDealer(long, hand);
void round();



pthread_t playerThreads[3];
pthread_t dealerThread;

//Main
int main(int argc, const char * argv[]) {
    cout << "Welcome to Pair War" << endl;
    cout << "See Log File" << endl;
    logFile = freopen("pairWarOutput.txt","w", stdout);   //Create the log file
    deckBuild();
    deckShuffle();
    
    while (roundNum < 4){
        round();
        roundNum++;
        win = false;
    }
    
    
    
    fprintf(logFile,"Pair War Log File");
    fclose(logFile);
    return 0;
}

void deckBuild(){
    cout << "buidling deck..." << endl;
    
    int card = 0;
    for (int i = 0; i < 4; i++){
        for (int j =0; j < 13; j++){
            deck[card] = j;
            card++;
        }
    }
    deckTop = &deck[0];
    deckBottom = &deck[51];
    
    std::cout << "Card deck: ";
    for (int j = 0; j < 52; j++){
        if (j == 51){
            cout << deck[j] << endl;
        }else{
            cout << deck[j] << ", ";
        }
    }
}

void deckShuffle(){
    for( int x = 0; x < (52 - 1); x++){
        int randomCard = x + (rand() % (52 - x));
        int cardHolder = deck[x];
        deck[x] = deck[randomCard];
        deck[randomCard] = cardHolder;
    }
    
    std::cout << "Shuffled card deck: ";
    for (int j = 0; j < 52; j++){
        if (j == 51){
            cout << deck[j] << endl;
        }else{
            cout << deck[j] << ", ";
        }
    }

}

void deckDeal(){
    
}

void round(){
    cout << "Beginging Round..." << endl;
    
    int dealer = pthread_create(&dealerThread,NULL,&dealer_Thread,NULL);     //Create dealer thread
    int player;
    for (long i = 0; i < 3; i++){
        pthread_create(&playerThreads[i], NULL, &player_Thread, (void *)i);
    }
    
    pthread_join(dealerThread, NULL);
    for( int j = 0; j < 3; j++){
        pthread_join(playerThreads[j], NULL);
    }
    
}

void *dealer_Thread(void *arg){
    int playerNum = 0;
    turn = 0;
    hand dealerHand;
    deckDealer(playerNum, dealerHand);
}

void *player_Thread(void *arg){
    
}

void deckDealer(long playerNum, hand currentHand){
    if (playerNum == 0){    //First hand
        cout << "DEALER: shuffle deck" << endl;
        deckShuffle();
        cout << "DEALER: deal cards" << endl;
        deckDeal();
    }
}



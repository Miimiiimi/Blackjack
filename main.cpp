// Case 4: Blackjack
// Karla Valenzuela 
// Course Project Part 2

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <iomanip>

using namespace std;

// Represents a single playing card
struct Card {
    int value;
    string name;
    string suit;
};

// Represents a hand of cards
class Hand {
public:
    vector<Card> cards;
    int getValue() const {
        int total = 0;
        int aceCount = 0;
        for (const auto& card : cards) {
            total += card.value;
            if (card.name == "Ace") {
                aceCount++;
            }
        }
        // Adjust for Aces if possible
        while (aceCount > 0 && total <= 11) {
            total += 10; // Count one Ace as 11
            aceCount--;
        }
        return total;
    }
};

// Represents the deck and manages cards
class Deck {
private:
    vector<Card> cards;

    void populate() {
        string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
        string names[] = {"Ace", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King"};
        int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

        for (int s = 0; s < 4; ++s) {
            for (int n = 0; n < 13; ++n) {
                cards.push_back({values[n], names[n], suits[s]});
            }
        }
    }

public:
    Deck() {
        populate();
        shuffle();
    }

    void shuffle() {
        // Seed the random number generator
        srand(time(0));
        std::random_shuffle(cards.begin(), cards.end());
    }

    Card dealCard() {
        if (cards.empty()) {
            cout << "Reshuffling the deck..." << endl;
            populate();
            shuffle();
        }
        Card card = cards.back();
        cards.pop_back();
        return card;
    }

    int remainingCards() const {
        return cards.size();
    }
};

// Function to calculate winning probability using Monte Carlo simulation
double calculateWinProbability(const Hand& playerHand, const Hand& dealerHand, Deck currentDeck) {
    int simulations = 10000; // Number of simulations for accuracy
    int wins = 0;
    int pushes = 0;

    for (int i = 0; i < simulations; ++i) {
        Deck simDeck = currentDeck; // Use a copy of the remaining deck
        Hand simPlayerHand = playerHand;
        Hand simDealerHand = dealerHand;

        // Player turn simulation
        while (simPlayerHand.getValue() < 17) { // Simple strategy: hit on < 17
            simPlayerHand.cards.push_back(simDeck.dealCard());
            if (simPlayerHand.getValue() > 21) break;
        }

        if (simPlayerHand.getValue() > 21) {
            continue; // Player busts, simulation loop continues
        }

        // Dealer turn simulation (dealer hits on 16 or less)
        while (simDealerHand.getValue() < 17) {
            simDealerHand.cards.push_back(simDeck.dealCard());
        }

        // Determine winner
        if (simDealerHand.getValue() > 21) {
            wins++;
        } else if (simPlayerHand.getValue() > simDealerHand.getValue()) {
            wins++;
        } else if (simPlayerHand.getValue() == simDealerHand.getValue()) {
            pushes++;
        }
    }

    // Win probability (pushes can be considered as half wins in terms of expected value in a casino game)
    return (double)(wins + pushes / 2.0) / simulations * 100.0;
}

void displayHands(const Hand& player, const Hand& dealer, bool showAllDealer = false) {
    cout << "\n--- Hands ---" << endl;
    cout << "Dealer's Hand:" << endl;
    if (showAllDealer) {
        for (const auto& card : dealer.cards) {
            cout << "  " << card.name << " of " << card.suit << endl;
        }
        cout << "  Total Value: " << dealer.getValue() << endl;
    } else {
        cout << "  " << dealer.cards[0].name << " of " << dealer.cards[0].suit << " (one card hidden)" << endl;
        cout << "  Visible Value: " << dealer.cards[0].value << endl;
    }
    cout << "Player's Hand:" << endl;
    for (const auto& card : player.cards) {
        cout << "  " << card.name << " of " << card.suit << endl;
    }
    cout << "  Total Value: " << player.getValue() << endl;
    cout << "-------------" << endl;
}

int main() {
    Deck gameDeck;
    Hand playerHand;
    Hand dealerHand;
    char choice;

    cout << "Welcome to Single Deck Blackjack!" << endl;

    // Initial deal
    playerHand.cards.push_back(gameDeck.dealCard());
    dealerHand.cards.push_back(gameDeck.dealCard());
    playerHand.cards.push_back(gameDeck.dealCard());
    dealerHand.cards.push_back(gameDeck.dealCard());

    // Check for initial Blackjacks, the probability calculation assumes the game continues past the initial deal
    // The probability calculation is for the current player hand against dealer up card.

    while (true) {
        displayHands(playerHand, dealerHand);

        // Calculate probability before any action
        double probBeforeHit = calculateWinProbability(playerHand, dealerHand, gameDeck);
        cout << fixed << setprecision(2);
        cout << "Winning Probability (before hit/stand decision): " << probBeforeHit << "%" << endl;


        cout << "Do you want to Hit (press h) or Stand (press s)? ";
        cin >> choice;

        if (choice == 'h' || choice == 'H') {
            playerHand.cards.push_back(gameDeck.dealCard());
            if (playerHand.getValue() > 21) {
                displayHands(playerHand, dealerHand, true);
                cout << "Player busted! Dealer wins." << endl;
                break;
            }
            // Recalculate probability after hit
            double probAfterHit = calculateWinProbability(playerHand, dealerHand, gameDeck);
            cout << "Winning Probability (after hitting): " << probAfterHit << "%" << endl;
        } else if (choice == 's' || choice == 'S') {
            // Dealer's turn
            while (dealerHand.getValue() < 17) {
                dealerHand.cards.push_back(gameDeck.dealCard());
            }
            displayHands(playerHand, dealerHand, true);

            // Determine final winner
            if (dealerHand.getValue() > 21) {
                cout << "Dealer busted! Player wins." << endl;
            } else if (playerHand.getValue() > dealerHand.getValue()) {
                cout << "Player wins!" << endl;
            } else if (playerHand.getValue() < dealerHand.getValue()) {
                cout << "Dealer wins." << endl;
            } else {
                cout << "It's a push....." << endl;
            }
            break;
        } else {
            cout << "Invalid input. Please enter 'h' or 's'." << endl;
        }
    }

    return 0;
}
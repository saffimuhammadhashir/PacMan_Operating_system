#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;


struct score_body {
    int score;
    string name;
    score_body* next;

    score_body(int s, string n) : score(s), name(n), next(nullptr) {}
};


class ScoreList {
private:
    string filename = "scores.txt";


    void insertscore_body(int score, string name) {
        score_body* newscore_body = new score_body(score, name);
        if (head == nullptr) {
            head = newscore_body;
        }
        else {
            score_body* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newscore_body;
        }
    }


    void swap(score_body* a, score_body* b) {
        int tempScore = a->score;
        string tempName = a->name;
        a->score = b->score;
        a->name = b->name;
        b->score = tempScore;
        b->name = tempName;
    }
public:
    score_body* head;
    ScoreList() : head(nullptr) {}
    void insert(int score,string name="Guest") {
        
        insertscore_body(score, name);
        printscores();
        writeToFile();
    }

    void readFromFile() {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Unable to open file " << filename << endl;
            return;
        }

        int score;
        string name;
        while (file >> name >> score) {
            insertscore_body(score, name);
        }

        file.close();
        sortList();
        writeToFile();
    }


    void sortList() {
        if (head == nullptr || head->next == nullptr) {
            return;
        }

        bool swapped;
        score_body* ptr1;
        score_body* lptr = nullptr;

        do {
            swapped = false;
            ptr1 = head;
            while (ptr1->next != lptr) {
                if (ptr1->score < ptr1->next->score) {
                    swap(ptr1, ptr1->next);
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while (swapped);
    }


    void writeToFile() {
        sortList();
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Unable to create file " << filename << endl;
            return;
        }

        score_body* temp = head;
        while (temp != nullptr) {
            file << temp->name << " " << temp->score << endl;
            temp = temp->next;
        }

        file.close();
    }
    void printscores() {
        score_body* temp = head;
        while (temp != nullptr) {
            cout << temp->name << " " << temp->score << endl;
            temp = temp->next;
        }
        cout << endl;
    }
};

//int main() {
//    ScoreList list;
//    list.readFromFile("scores.txt");
//
//    cout << "List before sorting:" << endl;
//    list.writeToFile("scores.txt"); // Write unsorted list to file
//    system("type scores.txt");
//
//    list.insert();
//
//    cout << "\nList after sorting:" << endl;
//    list.writeToFile("scores.txt"); // Write sorted list to file
//    system("type scores.txt");
//
//    return 0;
//}

#include <iostream>
#include <list>
#include <algorithm>

using namespace std;

class Observer
{
public:
    virtual void Update(const int &a, const int &b) = 0;
    virtual ~Observer(){};
};

class Score
{
private:
    int team_A_score = 0;
    int team_B_score = 0;
    list<Observer *> observers;

public:
    void Notify()
    {
        for (auto it = observers.begin(); it != observers.end(); it++)
        {
            (*it)->Update(team_A_score, team_B_score);
        }
    }

    void RoundFinished()
    {
        Notify();
    }

    void AddObserver(Observer &observer)
    {
        observers.push_back(&observer);
    }

    void RemoveObserver(Observer &observer)
    {
        observers.remove(&observer);
    }

    void SetScore(const int &a, const int &b)
    {
        team_A_score = a;
        team_B_score = b;
        RoundFinished();
    }

    int GetScoreA()
    {
        return team_A_score;
    }

    int GetScoreB()
    {
        return team_B_score;
    }

    ~Score()
    {
        observers.clear();
    }
};

class CurrentScore : public Observer
{
private:
    list<Score *> scores;
    int scoreA = 0;
    int scoreB = 0;

public:
    void Display()
    {
        cout << "Current score: (Team A) " << scoreA << " : " << scoreB << " (Team B)" << endl;
    }

    void Update(Score &score)
    {
        scoreA = score.GetScoreA();
        scoreB = score.GetScoreB();
    }

    void Update(const int &a, const int &b)
    {
        scoreA = a;
        scoreB = b;
    }

    void Subscribe(Score &score)
    {
        scores.push_back(&score);
        Update(score);
    }

    void Unsubscribe(Score &score)
    {
        scores.remove(&score);
        Update(0, 0);
    }

    ~CurrentScore()
    {
        scores.clear();
    }
};

class PredictedScore : public Observer
{
private:
    list<Score *> scores;
    int scoreA = 0;
    int scoreB = 0;

public:
    void
    Display()
    {
        if (scoreA == 16 || scoreB == 16)
        {
            cout << "Match finished: (Team A) " << scoreA << " : " << scoreB << " (Team B)" << endl;
            return;
        }

        if (scoreA > scoreB)
        {
            cout << "Predicted score: (Team A) " << scoreA + 1 << " : " << scoreB << " (Team B)" << endl;
            return;
        }

        cout << "Predicted score: (Team A) " << scoreA << " : " << scoreB + 1 << " (Team B)" << endl;
        return;
    }

    void Update(Score &score)
    {
        scoreA = score.GetScoreA();
        scoreB = score.GetScoreB();
    }

    void Update(const int &a, const int &b)
    {
        scoreA = a;
        scoreB = b;
    }

    void Subscribe(Score &score)
    {
        scores.push_back(&score);
        Update(score);
    }

    void Unsubscribe(Score &score)
    {
        scores.remove(&score);
        Update(0, 0);
    }

    ~PredictedScore()
    {
        scores.clear();
    }
};

int main()
{
    CurrentScore currentScore;
    PredictedScore predictedScore;
    Score score1;
    Score score2;

    score1.SetScore(16, 13);
    score2.SetScore(7, 8);

    currentScore.Subscribe(score1);
    currentScore.Display();

    currentScore.Subscribe(score2);
    currentScore.Display();

    currentScore.Unsubscribe(score1);
    currentScore.Unsubscribe(score2);
    currentScore.Display();

    predictedScore.Subscribe(score1);
    predictedScore.Display();

    predictedScore.Subscribe(score2);
    predictedScore.Display();

    return 0;
}

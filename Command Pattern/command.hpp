#pragma once
#include <iostream>
#include <string>
#include <stack>

// Receiver
class Lamp
{
private:
    unsigned int currentBrightness;

public:
    void SetBrightness(const int &brightness)
    {
        currentBrightness = brightness;
    }

    unsigned int GetBrightness() const
    {
        return currentBrightness;
    }
};

class Command
{
public:
    virtual ~Command() = default;
    virtual void Execute() const = 0;
    virtual void Unexecute() const = 0;
};

class TurnOn : public Command
{
private:
    Lamp *lamp_;

public:
    TurnOn(Lamp *lamp)
    {
        lamp_ = lamp;
    }

    void Execute() const
    {
        this->lamp_->SetBrightness(1);
        std::cout << "The lamp was turned on. Current brightness is equal to 1\n";
    }

    void Unexecute() const
    {
        this->lamp_->SetBrightness(0);
        std::cout << "The lamp was turned off. Current brightness is equal to 0.\n";
    }
};

class MakeBrighter : public Command
{
private:
    Lamp *lamp_;

public:
    MakeBrighter(Lamp *lamp)
    {
        lamp_ = lamp;
    }

    void Execute() const
    {
        auto currentBrightness = this->lamp_->GetBrightness();
        if (currentBrightness < 5)
        {
            this->lamp_->SetBrightness(currentBrightness + 1);
            std::cout << "The lamp was made brighter. Current brightness is equal to " << currentBrightness + 1 << "\n";
            return;
        }
        std::cout << "The lamp was NOT made brighter (maximum brightness reached).\n";
    }

    void Unexecute() const
    {
        auto currentBrightness = this->lamp_->GetBrightness();
        // При попытке сделать лампу тусклее, когда ее яркость составляет 1, лампа будет выключаться.
        if (currentBrightness == 1)
        {
            this->lamp_->SetBrightness(currentBrightness - 1);
            std::cout << "The lamp was turned off. Current brightness is equal to 0.\n";
            return;
        }
        if (currentBrightness > 1)
        {
            this->lamp_->SetBrightness(currentBrightness - 1);
            std::cout << "The lamp was made dimmer. Current brightness is equal to " << currentBrightness - 1 << "\n";
            return;
        }
        std::cout << "The lamp was NOT made dimmer (lamp is turned off).\n";
    }
};

class TurnOff : public Command
{
private:
    Lamp *lamp_;

public:
    TurnOff(Lamp *lamp)
    {
        lamp_ = lamp;
    }

    void Execute() const
    {
        this->lamp_->SetBrightness(0);
        std::cout << "The lamp was turned off. Current brightness is equal to 0.\n";
    }

    void Unexecute() const
    {
        this->lamp_->SetBrightness(1);
        std::cout << "The lamp was turned on. Current brightness is equal to 1.\n";
    }
};

class Invoker
{
private:
    std::stack<Command *> executedCommands;

public:
    ~Invoker()
    {
        while (!executedCommands.empty())
        {
            delete executedCommands.top();
            executedCommands.pop();
        }
    }

    void DoCommand(Command *command)
    {
        command->Execute();
        executedCommands.push(command);
    }

    void Redo()
    {

        if (!executedCommands.empty())
        {
            std::cout << "Invoker: the last action was repeated.\n";
            DoCommand(executedCommands.top());
            return;
        }
        std::cout << "Invoker: the last action was NOT repeated (stack is empty).\n";
    }

    void Undo()
    {
        if (!executedCommands.empty())
        {
            std::cout << "Invoker: the last action was undone.\n";
            executedCommands.top()->Unexecute();
            executedCommands.pop();
            return;
        }
        std::cout << "Invoker: the last action was NOT undone (stack is empty).\n";
    }
};

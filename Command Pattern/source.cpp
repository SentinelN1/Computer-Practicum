#include "command.hpp"

int main()
{
    Invoker *invoker = new Invoker;
    Lamp *lamp = new Lamp;

    invoker->DoCommand(new TurnOn(lamp));
    invoker->DoCommand(new MakeBrighter(lamp));
    invoker->Redo();
    invoker->Undo();
    invoker->DoCommand(new TurnOff(lamp));

    if (invoker)
    {
        delete invoker;
    }

    if (lamp)
    {
        delete lamp;
    }

    return 0;
}

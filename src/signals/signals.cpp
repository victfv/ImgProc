#include "signals.h"
#include <algorithm>


std::vector<Signal*> receivers;


Signal::Signal(bool receiver)
{
    if (receiver)
    {
        receivers.push_back(this);
    }
}


void Signal::send_simple(std::string message, int num)
{
    for (Signal* signal : receivers)
    {
        signal->receive_simple(message, num);
    }
}

void Signal::send(void* sender, std::string t_sender, void* data, std::string t_data)
{
    for (Signal* signal : receivers)
    {
        signal->receive(sender, t_sender, data, t_data);
    }
}

Signal::~Signal()
{
    auto iterator = std::find(receivers.begin(), receivers.end(), this);
    if (iterator != receivers.end())
    {
        receivers.erase(iterator);
    }
}


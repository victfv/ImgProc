#ifndef SIGNALS
#define SIGNALS

#include <vector>
#include <string>


class Signal
{
    public:
        Signal(bool receiver = true);
        virtual ~Signal();

        virtual void receive_simple(std::string message, int num) = 0;
        static void send_simple(std::string message, int num);

        virtual void receive(void* sender, std::string t_sender, void* data, std::string t_data) = 0;
        static void send(void* sender, std::string t_sender, void* data, std::string t_data);
};


#endif
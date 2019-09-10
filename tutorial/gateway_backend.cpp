#include <iostream>

#include <zmq.hpp>

using namespace std;

int main(int argc, char** argv)
{
    zmq::context_t ctx(1);
    zmq::socket_t server(ctx, ZMQ_DEALER);
    server.bind("tcp://127.0.0.1:20802");

    zmq::pollitem_t items[] = {
            { static_cast<void*>(server), 0, ZMQ_POLLIN, 0 } 
        };
    while(true)
    {
        try {
            zmq::poll(items, 1, 0);
            if (items[0].revents & ZMQ_POLLIN) {
                // printf("\n%s ", identity);
                // s_dump(client_socket_);
                zmq::message_t message;
                server.recv(&message);
                int size = message.size();
                std::string str_client_id(static_cast<char*>(message.data()), sizeof(uint32_t));
                uint32_t client_id = atol(str_client_id.c_str());

                std::string data(static_cast<char*>(message.data()+sizeof(uint32_t)), size-sizeof(uint32_t));
                printf("msg[%u]:%s\n", client_id, data.c_str());
                std::string rsp = data+" rsp!";
                server.send(&client_id, sizeof(client_id), ZMQ_SNDMORE);
                server.send(rsp.data(), rsp.size());

            }
        }
        catch (std::exception &e) {}
    }
    return 0;
}
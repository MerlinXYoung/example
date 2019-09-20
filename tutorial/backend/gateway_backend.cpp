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
            zmq::poll(items, 1, 50);
            if (items[0].revents & ZMQ_POLLIN) {
                // printf("\n%s ", identity);
                // s_dump(client_socket_);
                zmq::message_t message_client;
                server.recv(&message_client);
                int size = message_client.size();
                int more = 0;
                size_t more_size = sizeof (more);
                server.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                if(!more || size != sizeof(uint32_t))
                {
                    printf("fuck message not more or  not client id\n");
                    continue;
                }
                uint32_t client_id = *reinterpret_cast<uint32_t*>(message_client.data());//atol(str_client_id.c_str());
                zmq::message_t message;
                server.recv(&message);
                size = message.size();
                std::string data(reinterpret_cast<char*>(message.data()), size);
                printf("msg[%u]:%s\n", client_id, data.c_str());
                std::string rsp = data +" rsp!";
                std::string notify1("notify1");
                std::string notify2("notify2");
                server.send(&client_id, sizeof(client_id), ZMQ_SNDMORE);                
                server.send(notify1.data(), notify1.size());
                server.send(&client_id, sizeof(client_id), ZMQ_SNDMORE);                                
                server.send(notify2.data(), notify2.size());
                server.send(&client_id, sizeof(client_id), ZMQ_SNDMORE);
                server.send(rsp.data(), rsp.size());
                server.send(&client_id, sizeof(client_id), ZMQ_SNDMORE);                                                
                server.send(notify2.data(), notify2.size());

            }
        }
        catch (std::exception &e) {}
    }
    return 0;
}
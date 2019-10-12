#include <iostream>

#include <zmq.hpp>
#include "../../build/gateway/ss_gateway.pb.h"

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
                zmq::message_t message_head;
                server.recv(&message_head);
                int size = message_head.size();
                gw::ss::Head head;
                if(!head.ParseFromArray(message_head.data(), message_head.size()))
                {
                    printf("parse head error");
                    break;
                }
                printf("msgid[%d] client[%u] uid[%lu]\n", head.msgid(), head.client_id(), head.uid());

                int more = 0;
                size_t more_size = sizeof (more);
                server.getsockopt(ZMQ_RCVMORE, &more, &more_size);
                
                if(more)
                {
                    zmq::message_t message;
                    server.recv(&message);
                    size = message.size();
                    std::unique_ptr<google::protobuf::Message> body;
                    switch(head.msgid()){
                    case gw::ss::EMsgID::ClientNew:
                        body.reset(new gw::ss::ClientNewReq);
                        break;
                    case gw::ss::EMsgID::ClientAuth:
                        body.reset(new gw::ss::ClientAuthReq);
                        break;
                    case gw::ss::EMsgID::ClientClose:
                        body.reset(new gw::ss::ClientCloseReq);
                        break;
                    case gw::ss::EMsgID::Other:
                        break;
                    default:
                        break;
                    }
                    if(body)
                    {
                        if(!body->ParseFromArray(message.data(), message.size()))
                        {
                            printf("fuck parse body error\n");
                            break;
                        }
                        printf("body[%s]\n",body->DebugString().c_str());
                    }
                    else
                    {
                        std::string data(reinterpret_cast<char*>(message.data()), size);
                        printf("msg:%s\n",  data.c_str());
                        std::string rsp = data +" rsp!";
                        std::string notify1("notify1");
                        std::string notify2("notify2");
                        uint32_t client_id = head.client_id();
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

            }
        }
        catch (std::exception &e) {}
    }
    return 0;
}
//
// Created by thpn on 05/09/18.
//

#ifndef AYETH_SERVER_H
#define AYETH_SERVER_H

#include <string>
#include "./Event.h"

class Server {
public:
    Server(int) {//int:capacity
        };
//    void setLogger();
    void onStart();
    void stop();
    void subscribe(std::string subscriber);//, Channel out);
    void unsubscribe(std::string subscriber);
    void unsubscribeAll(std::string subscriber);
    void publishWithTags(Event eventData);
};


#endif //AYETH_SERVER_H

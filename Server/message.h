//
//  Message.h
//  DistributedProject1
//
//  Created by Malak Sadek on 10/27/17.
//  Copyright © 2017 Malak Sadek. All rights reserved.
//

#ifndef Message_h
#define Message_h
#include <string>
#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <cstring>
//enum MessageType {Request, Reply};

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

class Message {
private:
    int message_type;
    int operation;
    char* message;
    size_t message_size;
    int rpc_id;
    int fraged;
    int frag_count;
    int frag_total;
    int nOv;
public:
    Message(int message_type, int operation, char* p_message, size_t p_message_size, int p_rpc_id, int fraged, int frag_count, int frag_total, int nOv) {
        this->message_type = message_type;
        this-> message = new char[p_message_size+1];
        this->operation = operation;
        this->message = p_message;
        this->message_size = p_message_size;
        this->rpc_id = p_rpc_id;
        this->fraged = fraged;
        this->frag_count = frag_count;
        this->frag_total = frag_total;
        this->nOv = nOv;
    };


    Message(char* marshalled_base64) {
        //std:: cout << "i am in ya man" << std::endl;
      /*  if((int)marshalled_base64[strlen(marshalled_base64)-1]-'0' != 0) {
           nOv = (int)marshalled_base64[strlen(marshalled_base64)-1]-'0';
           marshalled_base64[strlen(marshalled_base64)-1] = '\0';
        }*/
        char header[50];
        char* payload = new char[1000000];
        int done = 0;
        int f = 0;
        int k = 0;

        for (int x = 0; x < 1000000; x++) {
            if (x < 50)
                header[x] = '\0';
            payload[x] = '\0';
        }
        for (unsigned long x = 0; x < strlen(marshalled_base64)+1; x++) {
            if((marshalled_base64[x] == '=')&&(!done)) {
                for (unsigned long y = x; y < strlen(marshalled_base64)+1; y++) {
                    if(marshalled_base64[y] != '=') {
                        payload[f] = marshalled_base64[y];
                        f++;
                    }
                }
                done = 1;
            }
            else if (!done) {
                header[k] = marshalled_base64[x];
                k++;
            }
        }
        char* payloadloc = payload;
        unsigned long in_len_header = strlen(header);
        unsigned long in_len_data = strlen(payload);
        int i = 0;
        int j = 0;
        int in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::string ret;

        while (in_len_header-- && (header[in_] != '=') && is_base64(header[in_])) {
            char_array_4[i++] = header[in_]; in_++;
            if (i ==4) {
                for (i = 0; i <4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);

                char_array_3[0] = ( char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

                for (i = 0; (i < 3); i++)
                    ret += char_array_3[i];
                i = 0;
            }
        }

        if (i) {
            for (j = 0; j < i; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

            for (j = 0; (j < i - 1); j++)
                ret += char_array_3[j];
        }
        i = 0;
        j = 0;
        in_ = 0;
        for (int x = 0; x < 3; x++)
            char_array_3[x] = '\0';
        for (int x = 0; x < 4; x++)
            char_array_4[x] = '\0';

        while (in_len_data-- && (payloadloc[in_] != '=') && is_base64(payloadloc[in_])) {
            char_array_4[i++] = payloadloc[in_]; in_++;
            if (i ==4) {
                for (i = 0; i <4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);

                char_array_3[0] = ( char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

                for (i = 0; (i < 3); i++)
                    ret += char_array_3[i];
                i = 0;
            }
        }

        if (i) {
            for (j = 0; j < i; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

            for (j = 0; (j < i - 1); j++)
                ret += char_array_3[j];
        }
        char * comp = new char;
        *comp = '\0';
        if (strncmp(marshalled_base64, comp, strlen(marshalled_base64)+1)) {
        int opdone = 0;
        int novdone = 0;
        int frageddone = 0;
        int fragcountdone = 0;
        int fragtotaldone = 0;
        int msgdone = 0;
        char* msg = new char [1000000];
        int msgsizedone = 0;
        int rpciddone = 0;
        std::string op = "";
        std::string msgsize = "";
        std::string nov = "";
        std::string rpcid = "";
        std::string frgd = "";
        std::string fragcnt = "";
        std::string fragttl = "";
        i = 2;
        int ii = 0;
        char msgtype = ret.at(0);

        while (i < ret.length()) {
            if (!opdone) {
                if (ret.at(i) != '-') {
                    op += ret.at(i);
                    i++;
                }
                else {
                    opdone = 1;
                    i++;
                }
            }
            else if (!msgsizedone) {
                if (ret.at(i) != '-') {
                    msgsize += ret.at(i);
                    i++;
                }
                else {
                    msgsizedone = 1;
                    i++;
                }
            }
            else if (!rpciddone) {
                if (ret.at(i) != '-') {
                    rpcid += ret.at(i);
                    i++;
                }
                else {
                    rpciddone = 1;
                    i++;
                }
            }
            else if (!frageddone) {
                if (ret.at(i) != '-') {
                    frgd += ret.at(i);
                    i++;
                }
                else {
                    frageddone = 1;
                    i++;
                }
            }
            else if (!fragcountdone) {
                if (ret.at(i) != '-') {
                    fragcnt += ret.at(i);
                    i++;
                }
                else {
                    fragcountdone = 1;
                    i++;
                }
            }
            else if (!fragtotaldone) {
                if (ret.at(i) != '-') {
                    fragttl += ret.at(i);
                    i++;
                }
                else {
                    fragtotaldone = 1;
                    i++;
                }
            }
            else if (!novdone) {
                if (ret.at(i) != '*') {
                    nov += ret.at(i);
                    i++;
                }
                else {
                    novdone = 1;
                    i++;
                }
            }
            else if (!msgdone) {
                msg[ii] = ret.at(i);
                ii++;
                i++;
            }
        }

        for (int x = 2; x < 1000000; x++)
            msg[x-2] = msg[x];

        if (msgtype == '0')
            message_type = 0;
        else
            message_type = 1;

        operation = std::stoi(op);
        message = msg;
        message_size = ii;
        rpc_id = std::stoi(rpcid);
        fraged = std::stoi(frgd);
        frag_count = std::stoi(fragcnt);
        frag_total = std::stoi(fragttl);
        nOv = std::stoi(nov);
        }
    };

    char* marshal(int datamarsh) {

        std::string headerinfo = std::to_string(message_type)+"-"+std::to_string(operation)+"-"+std::to_string(message_size)+"-"+std::to_string(rpc_id)+"-"+std::to_string(fraged)+"-"+std::to_string(frag_count)+"-"+std::to_string(frag_total)+"-"+std::to_string(nOv)+"***";

        char header [30];

        for (int x = 0; x < 30; x++)
            header[x] = '\0';

        for (int x = 0; x < headerinfo.length(); x++)
            header[x] = headerinfo.at(x);

        void * headerloc = &header;
        char const* bytes_to_encode_header = static_cast<char* const>(headerloc);
        char const* bytes_to_encode_data = static_cast<char* const>(message);
        unsigned long in_len_header = strlen(bytes_to_encode_header);
        unsigned long in_len_data = message_size;

        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];
        std::string ret = "";
        while (in_len_header--) {
            char_array_3[i++] = *(bytes_to_encode_header++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for(i = 0; (i <4) ; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for(j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while((i++ < 3))
                ret += '=';

        }
        ret+= '=';
        i = 0;
        j = 0;

        if (datamarsh) {
        for (int x = 0; x < 3; x++)
            char_array_3[x] = '\0';

        for (int x = 0; x < 4; x++)
            char_array_4[x] = '\0';

        while (in_len_data--) {
            char_array_3[i++] = *(bytes_to_encode_data++);

            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for(i = 0; (i <4) ; i++)
                    ret += base64_chars[char_array_4[i]];

                i = 0;
            }
        }

        if (i)
        {
            for(j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while((i++ < 3))
                ret += '=';
        }
        }
        else {
            ret+=message;
        }
        ret += '\0';
        char* temp;
        temp = new char[ret.size()+1];

        strcpy(temp,ret.c_str());
        setMessage(temp, ret.size()+1);
        std::cout << "Message Marshalled!\n";
        return temp;
    };

    int getOperation() {
        return operation;
    };
    int getRPCId() {
        return rpc_id;
    };
    char * getMessage() {
        return message;
    };
    size_t getMessageSize() {
        return message_size;
    };
    int getMessageType() {
        return message_type;
    }
    int getFraged() {
        return fraged;
    }
    int getFragCount() {
        return frag_count;
    }
    int getFragTotal() {
        return frag_total;
    }
    void setFraged(int f) {
        this->fraged = f;
    }

    int getnOv() {
        return nOv;
    }
    void setnOv(int n) {
        this->nOv = n;
    }
    void setFragCount(int f) {
        this->frag_count = f;
    }
    void setFragTotal(int f) {
        this->frag_total = f;
    }
    void setOperation (int _operation) {
        this->operation = _operation;
    }
    void setMessage (char * message, size_t message_size) {
        this->message = message;
        this->message_size = message_size;
    }
    void setMessageType (int message_type) {
        this->message_type = message_type;
    }
    ~Message(){}
};
#endif /* Message_h */


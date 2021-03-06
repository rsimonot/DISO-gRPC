#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "disogrpc.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using disogrpc::HandShake;
using disogrpc::HandShakeInit;
using disogrpc::HandShakeReply;

class HandShakeClient {
    public:
        HandShakeClient(std::shared_ptr<Channel> channel) : stub_(HandShake::NewStub(channel)) {}

        // Assembles the client's payload, sends it and presents the response back
        // from the server.
        std::string performHandShake(const std::string& user) {
            // Data we are sending to the server.
            HandShakeInit request;
            request.set_name(user);

            // Container for the data we expect from the server.
            HandShakeReply reply;

            // Context for the client. It could be used to convey extra information to
            // the server and/or tweak certain RPC behaviors.
            ClientContext context;

            // The actual RPC.
            Status status = stub_->performHandShake(&context, request, &reply);

            // Act upon its status.
            if (status.ok()) {
                return reply.message();
            } else {
                std::cout << status.error_code() << ": " << status.error_message() << std::endl;
                return "RPC failed";
            }
        }

    private:
    std::unique_ptr<HandShake::Stub> stub_;
};

int main(int argc, char** argv) {
    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint specified by
    // the argument "--target=" which is the only expected argument.
    // We indicate that the channel isn't authenticated (use of
    // InsecureChannelCredentials()).
    std::string target_str;
    std::string arg_str("--target");
    if (argc > 1)
    {
        std::string arg_val = argv[1];
        size_t start_pos = arg_val.find(arg_str);
        if (start_pos != std::string::npos)
        {
            start_pos += arg_str.size();
            if (arg_val[start_pos] == '=')
            {
                target_str = arg_val.substr(start_pos + 1);
            } else
            {
                std::cout << "The only correct argument syntax is --target="
                        << std::endl;
                return 0;
            }
        } else
        {
            std::cout << "The only acceptable argument is --target=" << std::endl;
            return 0;
        }
    } else
    {
        target_str = "localhost:5000";
    }
    HandShakeClient hs(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    std::string user("DISO HandShake Client");
    std::string reply = hs.performHandShake(user);
    std::cout << "\033[1;35mHandShake message received : " << reply << "\033[0m" << std::endl;

    return 0;
}

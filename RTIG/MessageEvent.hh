// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#ifndef CERTI_MESSAGEEVENT_H
#define CERTI_MESSAGEEVENT_H

#include <memory>
#include <vector>

#include <NetworkMessage.hh>
#include <Socket.hh>

namespace certi {

namespace rtig {

template <class NM>
class MessageEvent {
public:
    // cppcheck-suppress noExplicitConstructor
    MessageEvent(Socket* socket, std::unique_ptr<NM>&& message)
        : my_socket{socket}, my_message{std::forward<decltype(my_message)>(message)}
    {
    }

    MessageEvent(const MessageEvent<NM>& other) = delete;
    MessageEvent& operator=(const MessageEvent<NM>& rhs) = delete;

    MessageEvent(MessageEvent<NM>&& other) : my_socket{other.my_socket}
    {
        my_message.swap(other.my_message);
    }

    MessageEvent& operator=(MessageEvent<NM>&& other)
    {
        my_socket = other.my_socket;
        my_message.swap(other.my_message);
        return *this;
    }

    template <typename>
    friend class MessageEvent;

    template <typename Base>
    explicit MessageEvent(MessageEvent<Base>&& other) : my_message{static_cast<NM*>(other.my_message.release())}
    {
        my_socket = other.my_socket;
    }

    ~MessageEvent() = default;

    inline const Socket* socket() const
    {
        return my_socket;
    }

    inline Socket* socket()
    {
        return my_socket;
    }

    inline const NM* message() const
    {
        return my_message.get();
    }

    inline NM* message()
    {
        return my_message.get();
    }

private:
    Socket* my_socket;
    std::unique_ptr<NM> my_message;
};

using Responses = std::vector<MessageEvent<NetworkMessage>>;
}
}

#endif // CERTI_MESSAGEEVENT_H

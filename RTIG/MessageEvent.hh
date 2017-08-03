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

#include <NetworkMessage.hh>
#include <Socket.hh>

namespace {

template <typename Base, typename Derived, typename Del>
std::unique_ptr<Derived, Del> static_unique_ptr_cast(std::unique_ptr<Base, Del>&& p)
{
    auto d = static_cast<Derived*>(p.release());
    return std::unique_ptr<Derived, Del>(d, std::move(p.get_deleter()));
}

template <typename Base, typename Derived, typename Del>
std::unique_ptr<Derived, Del> dynamic_unique_ptr_cast(std::unique_ptr<Base, Del>&& p)
{
    if (Derived* result = dynamic_cast<Derived*>(p.get())) {
        p.release();
        return std::unique_ptr<Derived, Del>(result, std::move(p.get_deleter()));
    }
    return std::unique_ptr<Derived, Del>(nullptr, p.get_deleter());
}
}

namespace certi {

namespace rtig {

template <class NM>
class MessageEvent {
public:
    MessageEvent(Socket* socket, std::unique_ptr<NM> message) : my_socket{socket}, my_message{std::move(message)}
    {
    }

    MessageEvent(MessageEvent<NM>&& other) : my_socket{other.my_socket}
    {
        auto d = other.my_message.release();
        my_message = std::unique_ptr<NM>(d, std::move(other.my_message.get_deleter()));
    }

    template <typename>
    friend class MessageEvent;

    template <typename Base>
    MessageEvent(MessageEvent<Base>&& other)
    : my_socket{other.my_socket}
    , my_message{static_cast<NM*>(other.my_message.release())}
    {
    }

    ~MessageEvent()
    {
    }

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

    template <typename Derived>
    MessageEvent<Derived> static_cast_to()
    {
        return {socket(), static_cast<Derived*>(message())};
    }

    /*template <typename Derived>
    friend MessageEvent<Derived> message_event_cast(MessageEvent<NM>&& p);*/

private:
    Socket* my_socket;
    std::unique_ptr<NM> my_message;
};
}
}

#endif // CERTI_MESSAGEEVENT_H

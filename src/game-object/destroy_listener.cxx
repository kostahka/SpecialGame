#include "game-object/destroy_listener.hxx"

void destroyable::add_destroy_listener(destroy_listener* listener,
                                       int               object_id)
{
    listeners.push_back({ listener, object_id });
    listener->mark_listener(this);
}

void destroyable::remove_destroy_listener(destroy_listener* listener)
{
    listeners.erase(std::find_if(listeners.begin(),
                                 listeners.end(),
                                 [&](destroy_listener_info& info)
                                 { return info.listener == listener; }));
}

void destroyable::destroy()
{
    for (destroy_listener_info& listener_info : listeners)
    {
        listener_info.listener->on_destroy(listener_info.object_id);
        listener_info.listener->unmark_listener(this);
    }
}

void destroy_listener::mark_listener(destroyable* obj)
{
    destroyable_objects.push_back(obj);
}

destroy_listener::~destroy_listener()
{
    for (destroyable* obj : destroyable_objects)
    {
        obj->remove_destroy_listener(this);
    }
}
void destroy_listener::unmark_listener(destroyable* obj)
{
    destroyable_objects.erase(std::remove(
        destroyable_objects.begin(), destroyable_objects.end(), obj));
}

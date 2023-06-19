#pragma once

#include <vector>
class destroy_listener;
struct destroy_listener_info;

class destroyable
{
public:
    virtual void destroy() = 0;

    void add_destroy_listener(destroy_listener* listener, int object_id);
    void remove_destroy_listener(destroy_listener* listener);

    virtual ~destroyable();

private:
    std::vector<destroy_listener_info> listeners;
};

class destroy_listener
{
public:
    virtual void on_destroy(int object_id) = 0;

    void mark_listener(destroyable* obj);
    void unmark_listener(destroyable* obj);
    virtual ~destroy_listener();

private:
    std::vector<destroyable*> destroyable_objects;
};

struct destroy_listener_info
{
    destroy_listener* listener;
    int               object_id;
};
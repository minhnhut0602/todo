#pragma once

#include <coreds/nana/pager.h>

#include "../assets.h"
#include "../util.h"
#include "../todo.h"

namespace todo {
namespace user {

struct TodoNew : ui::SubForm
{
    const bool close_on_success;
    util::RequestQueue* rq{ nullptr };
private:
    todo::TodoStore& store;
    std::string errmsg;
    
    std::function<void(void* res)> $submit$${
        std::bind(&TodoNew::submit$$, this, std::placeholders::_1)
    };
    std::function<void()> $submit{
        std::bind(&TodoNew::submit, this)
    };
    
    std::function<void(const nana::arg_keyboard& arg)> $key_press{
        std::bind(&TodoNew::key_press, this, std::placeholders::_1)
    };
    
    int flex_height{ 0 };
    
    ui::w$::Input title_{ *this, &flex_height, "Title *", fonts::lg(), &colors::border_darken };
    ui::MsgPanel msg_{ *this, ui::MsgColors::DEFAULT };
    nana::button submit_{ *this, "Submit" };
    
    ui::Place place{ *this,
        "vert margin=10"
        "<title_>"
        "<weight=10>"
        "<submit_ weight=32>"
        "<weight=10>"
        "<msg_ weight=40>"
    };
public:
    TodoNew(TodoStore& store_, const char* title, bool close_on_success = false):
        ui::SubForm({0, 0, 360, 112}, title),
        close_on_success(close_on_success),
        store(store_)
    {
        resizeY(flex_height);
        
        place["title_"] << title_;
        title_.$.events().key_press($key_press);
        
        place["msg_"] << msg_;
        
        place["submit_"] << submit_;
        styles::apply_default(submit_);
        submit_.events().click($submit);
        
        place.collocate();
        ui::visible(msg_, false);
    }
private:
    void focus()
    {
        title_.$.focus();
    }
    void submit$$(void* res)
    {
        nana::internal_scope_guard lock;
        
        store.loading(false);
        title_.$.editable(true);
        
        if (res == nullptr)
        {
            msg_.update(errmsg);
        }
        else
        {
            store.prependAll(flatbuffers::GetRoot<todo::user::Todo_PList>(res)->p(), true);
            
            if (close_on_success)
                close();
            else
                focus();
            
            title_.$.caption("");
        }
    }
    void submit()
    {
        if (store.loading())
            return;
        
        auto title = title_.$.caption();
        auto title_sz = title.size();
        util::trim(title);
        if (title.empty())
        {
            title_.$.focus();
            // clear input if every char was whitespace
            if (title_sz != 0)
                title_.$.caption("");
            
            msg_.update(msgs::validation_required);
            return;
        }
        
        auto lastSeen = store.front();
        std::string buf;
        util::appendCreateReqTo(buf,
                lastSeen == nullptr ? nullptr : lastSeen->key.c_str(),
                title);
        
        rq->queue.emplace("/todo/user/Todo/create", buf, "Todo_PList", &errmsg, $submit$$);
        rq->send();
        
        title_.$.editable(false);
        ui::visible(msg_, false);
        store.loading(true);
    }
    void key_press(const nana::arg_keyboard& arg)
    {
        switch (arg.key)
        {
            case nana::keyboard::enter:
                submit();
                break;
            case nana::keyboard::escape:
                if (msg_.visible())
                    ui::visible(msg_, false);
                else
                    close();
                break;
        }
    }
public:
    void popTo(nana::widget& target)
    {
        ui::visible(msg_, false);
        ui::SubForm::popTo(target, util::POP_OFFSET);
        focus();
    }
};

// ==================================================

struct TodoUpdate : ui::SubForm
{
    const bool close_on_success;
    util::RequestQueue* rq{ nullptr };
private:
    todo::TodoStore& store;
    std::string errmsg;
    coreds::MultiCAS mc;
    todo::Todo* pojo{ nullptr };
    coreds::HasState<int>* item{ nullptr };
    std::vector<int> updated_fields;
    
    std::function<void(void* res)> $submit$${
        std::bind(&TodoUpdate::submit$$, this, std::placeholders::_1)
    };
    std::function<void()> $submit{
        std::bind(&TodoUpdate::submit, this)
    };
    
    std::function<void(const nana::arg_keyboard& arg)> $key_press{
        std::bind(&TodoUpdate::key_press, this, std::placeholders::_1)
    };
    
    int flex_height{ 0 };
    int height;
    
    ui::w$::Input title_{ *this, &flex_height, "Title *", fonts::lg(), &colors::border_darken };
    ui::w$::Checkbox completed_{ *this, &flex_height, false, "Completed", fonts::lg(), icons::square_checked, icons::square_empty };
    ui::MsgPanel msg_{ *this, ui::MsgColors::DEFAULT };
    nana::button submit_{ *this, "Update" };
    
    ui::Place place{ *this,
        "vert margin=10"
        "<title_>"
        "<weight=10>"
        "<completed_>"
        "<weight=10>"
        "<submit_ weight=32>"
        "<weight=10>"
        "<msg_ weight=40>"
    };
public:
    TodoUpdate(TodoStore& store_, const char* title, bool close_on_success = false):
        ui::SubForm({0, 0, 360, 112}, title),
        close_on_success(close_on_success),
        store(store_)
    {
        height = resizeY(flex_height);
        
        place["title_"] << title_;
        title_.$.events().key_press($key_press);
        
        place["completed_"] << completed_;
        
        place["msg_"] << msg_;
        
        place["submit_"] << submit_;
        styles::apply_default(submit_);
        submit_.events().click($submit);
        
        place.collocate();
        ui::visible(msg_, false);
    }
protected:
    void onClose() override
    {
        item->update(-2);
    }
private:
    void focus()
    {
        title_.$.focus();
    }
    bool update(int field)
    {
        switch (field)
        {
            case 3:
                pojo->title.assign(title_.$.caption());
                return true;
            case 4:
                pojo->completed = !pojo->completed;
                return true;
            default:
                return false;
        }
    }
    void submit$$(void* res)
    {
        nana::internal_scope_guard lock;
        
        store.loading(false);
        title_.$.editable(true);
        
        if (res == nullptr)
        {
            msg_.update(errmsg);
        }
        else
        {
            msg_.update(msgs::successful, ui::Msg::$SUCCESS);
            
            if (close_on_success)
                close();
            else
                focus();
            
            for (auto field : updated_fields)
            {
                if (update(field))
                    item->update(field);
            }
        }
        
        updated_fields.clear();
        mc.clear();
    }
    void submit()
    {
        if (store.loading())
            return;
        
        updated_fields.clear();
        mc.clear();
        
        auto title = title_.$.caption();
        auto title_sz = title.size();
        util::trim(title);
        if (title.empty())
        {
            title_.$.focus();
            // clear input if every char was whitespace
            if (title_sz != 0)
                title_.$.caption("");
            
            msg_.update(msgs::validation_required);
            return;
        }
        else if (title == pojo->title)
        {
            // reset input if there was whitespace
            if (title_sz != title.size())
                title_.$.caption(title);
        }
        else
        {
            // set input if there was whitespace
            if (title_sz != title.size())
                title_.$.caption(title);
            
            updated_fields.push_back(3);
            mc.add(3, title, pojo->title);
        }
        
        if (pojo->completed != completed_.value())
        {
            updated_fields.push_back(4);
            mc.add(4, !pojo->completed);
        }
        
        if (mc.empty())
        {
            focus();
            msg_.update(msgs::no_changes, ui::Msg::$WARNING);
            return;
        }
        
        std::string buf;
        util::appendUpdateReqTo(buf,
                pojo->key.c_str(),
                mc);
        
        rq->queue.emplace("/todo/user/Todo/update", buf, nullptr, &errmsg, $submit$$);
        rq->send();
        
        title_.$.editable(false);
        ui::visible(msg_, false);
        store.loading(true);
    }
    void key_press(const nana::arg_keyboard& arg)
    {
        switch (arg.key)
        {
            case nana::keyboard::enter:
                submit();
                break;
            case nana::keyboard::escape:
                if (msg_.visible())
                    ui::visible(msg_, false);
                else
                    close();
                break;
        }
    }
    void fill()
    {
        title_.$.caption(pojo->title);
        completed_.value(pojo->completed);
    }
public:
    void popTo(nana::widget& target, todo::Todo* pojo, coreds::HasState<int>* item, int x = 0)
    {
        this->item = item;
        this->pojo = pojo;
        
        ui::visible(msg_, false);
        fill();
        
        auto pos = nana::API::window_position(target);
        pos.x += x;
        pos.y = util::sc->resolvePopupY(pos.y, height);
        
        ui::SubForm::popTo(pos);
        
        focus();
    }
    coreds::HasState<int>* getItem()
    {
        return item;
    }
};

} // user
} // todo

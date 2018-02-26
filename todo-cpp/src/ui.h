#pragma once

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/picture.hpp>

namespace ui {

/* msvc2015 fails to compile this -> C3805
enum class Msg
{
    SUCCESS,
    ERROR,
    WARNING
};
*/

enum class Msg
{
    $SUCCESS,
    $ERROR,
    $WARNING
};

struct Form : nana::form
{
    Form(nana::rectangle rect, unsigned bg, const char* title = nullptr) : nana::form(rect)
    {
        bgcolor(nana::color_rgb(bg));
        if (title)
            caption(title);
    }
};

struct Place : nana::place
{
    Place(nana::widget& owner, const char* layout) : nana::place(owner)
    {
        div(layout);
    }
};

struct Icon : nana::picture
{
    Icon(nana::widget& owner, const char* icon, bool cursor_hand = false) : nana::picture(owner)
    {
        load(nana::paint::image(icon));
        transparent(true);
        
        if (!cursor_hand)
            return;
        
        events().mouse_move([this](const nana::arg_mouse& arg) {
            cursor(nana::cursor::hand);
        });
    }
};

struct Panel : nana::panel<false>
{
    nana::place place{ *this };
    
    Panel(nana::widget& owner, const char* layout) : nana::panel<false>(owner)
    {
        place.div(layout);
    }
};

struct DeferredPanel : nana::panel<false>
{
    const char* const layout;
    nana::place place;
    
    DeferredPanel(const char* layout) : nana::panel<false>(), layout(layout)
    {
        
    }
    
private:
    void _m_complete_creation() override
    {
        place.bind(*this);
        place.div(layout);
        
        //transparent(true);
    }
};

// not in the widget api
inline void visible(nana::widget& w, bool on)
{
    nana::API::show_window(w.handle(), on);
}
    
} // ui

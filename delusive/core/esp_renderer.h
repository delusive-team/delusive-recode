#pragma once
#include "../DllMain.h"
#include <string>
#include <algorithm>
#include <cstdarg>

enum e_text_flags : uint32_t {
    text_none           = 0,
    text_outline        = 1 << 0,
    text_dropshadow     = 1 << 1,
    text_center_x       = 1 << 2,
    text_center_y       = 1 << 3,
    text_right          = 1 << 4
};

class c_esp_renderer {
private:
    #define draw_list ImGui::GetForegroundDrawList()

    c_esp_renderer() = default;

    bool is_cn_chars( const char* text ) {
        while ( *text ) {
            if ( ( *text & 0xE0 ) == 0xE0 && ( *( text + 1 ) & 0x80 ) && ( *( text + 2 ) & 0x80 ) ) {
                unsigned int codepoint = ( ( *text & 0x0F ) << 12 ) | ( ( *( text + 1 ) & 0x3F ) << 6 ) | ( *( text + 2 ) & 0x3F );
                if ( codepoint >= 0x4E00 && codepoint <= 0x9FFF )
                    return true;
                text += 3;
            } else {
                ++text;
            }
        }
        return false;
    }

public:
    static c_esp_renderer& get() {
        static c_esp_renderer instance;
        return instance;
    }

    c_esp_renderer(const c_esp_renderer&) = delete;
    c_esp_renderer& operator=(const c_esp_renderer&) = delete;

    void line( ImVec2 p1, ImVec2 p2, ImU32 color, float thickness = 1.0f ) {
        draw_list->AddLine( p1, p2, color, thickness );
    }

    void line( float x1, float y1, float x2, float y2, ImU32 color, float thickness = 1.0f ) {
        line( ImVec2( x1, y1 ), ImVec2( x2, y2 ), color, thickness );
    }

    void rect( float x, float y, float w, float h, ImU32 color, float thickness = 1.0f, float rounding = 0.0f ) {
        draw_list->AddRect( ImVec2( x, y ), ImVec2( x + w, y + h ), color, rounding, 0, thickness );
    }

    void rect_filled( float x, float y, float w, float h, ImU32 color, float rounding = 0.0f ) {
        draw_list->AddRectFilled( ImVec2( x, y ), ImVec2( x + w, y + h ), color, rounding, 0 );
    }

    void rect_filled_multi( float x, float y, float w, float h, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left ) {
        draw_list->AddRectFilledMultiColor( ImVec2( x, y ), ImVec2( x + w, y + h ), col_upr_left, col_upr_right, col_bot_right, col_bot_left );
    }

    void circle( float x, float y, float radius, ImU32 color, float thickness = 1.0f, int segments = 0 ) {
        draw_list->AddCircle( ImVec2( x, y ), radius, color, segments, thickness );
    }

    void circle_filled( float x, float y, float radius, ImU32 color, int segments = 0 ) {
        draw_list->AddCircleFilled( ImVec2( x, y ), radius, color, segments );
    }

    void triangle( float x1, float y1, float x2, float y2, float x3, float y3, ImU32 color, float thickness = 1.0f ) {
        draw_list->AddTriangle( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), color, thickness );
    }

    void triangle_filled( float x1, float y1, float x2, float y2, float x3, float y3, ImU32 color ) {
        draw_list->AddTriangleFilled( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), color );
    }

    void box( float x, float y, float w, float h, ImU32 color, float thickness = 1.0f ) {
        draw_list->AddRect( ImVec2( x, y ), ImVec2( x + w, y + h ), color, 0.0f, 0, thickness );
    }

    void corner_box( float x, float y, float w, float h, float length, ImU32 color, float thickness = 1.0f ) {
        ImVec2 top_left[3] = { ImVec2(x + length, y), ImVec2(x, y), ImVec2(x, y + length) };
        draw_list->AddPolyline(top_left, 3, color, 0, thickness);

        ImVec2 top_right[3] = { ImVec2(x + w - length, y), ImVec2(x + w, y), ImVec2(x + w, y + length) };
        draw_list->AddPolyline(top_right, 3, color, 0, thickness);

        ImVec2 bot_left[3] = { ImVec2(x, y + h - length), ImVec2(x, y + h), ImVec2(x + length, y + h) };
        draw_list->AddPolyline(bot_left, 3, color, 0, thickness);

        ImVec2 bot_right[3] = { ImVec2(x + w, y + h - length), ImVec2(x + w, y + h), ImVec2(x + w - length, y + h) };
        draw_list->AddPolyline(bot_right, 3, color, 0, thickness);
    }

    void health_bar( float x, float y, float w, float h, float health, float max_health ) {
        health = (std::max)( 0.0f, (std::min)( health, max_health ) );
        float health_percentage = health / max_health;

        rect_filled( x, y, w, h, IM_COL32( 0, 0, 0, 180 ) );
        rect( x - 1, y - 1, w + 2, h + 2, IM_COL32( 0, 0, 0, 255 ) );

        ImU32 health_color = IM_COL32(
            (int)( 255.0f * ( 1.0f - health_percentage ) ),
            (int)( 255.0f * health_percentage ),
            0,
            255
        );

        float bar_height = ( h - 2.0f ) * health_percentage;
        rect_filled( x + 1, y + h - 1.0f - bar_height, w - 2.0f, bar_height, health_color );
    }

    ImVec2 calc_text_size( const char* text, float size ) {
        ImFont* font = ImGui::GetIO().FontDefault;
        // if you add custom fonts later based on is_cn_chars, select it here
        return font->CalcTextSizeA( size, FLT_MAX, 0.0f, text );
    }

    void string_ex( float x, float y, float font_size, uint32_t flags, ImU32 color, ImU32 outline_color, const char* format, va_list args ) {
        char buffer[1024];
        int length = vsnprintf( buffer, sizeof( buffer ), format, args );
        if ( length <= 0 ) return;

        ImFont* font = ImGui::GetIO().FontDefault;
        // In the future, dynamically select font based on is_cn_chars(buffer) if a Chinese font is loaded

        ImVec2 size = font->CalcTextSizeA( font_size, FLT_MAX, 0.0f, buffer );

        if ( flags & text_center_x ) x -= size.x * 0.5f;
        else if ( flags & text_right ) x -= size.x;
        
        if ( flags & text_center_y ) y -= size.y * 0.5f;

        if ( flags & text_dropshadow ) {
            draw_list->AddText( font, font_size, ImVec2( x + 1.0f, y + 1.0f ), outline_color, buffer );
        }

        if ( flags & text_outline ) {
            draw_list->AddText( font, font_size, ImVec2( x + 1.0f, y + 1.0f ), outline_color, buffer );
            draw_list->AddText( font, font_size, ImVec2( x - 1.0f, y - 1.0f ), outline_color, buffer );
            draw_list->AddText( font, font_size, ImVec2( x + 1.0f, y - 1.0f ), outline_color, buffer );
            draw_list->AddText( font, font_size, ImVec2( x - 1.0f, y + 1.0f ), outline_color, buffer );
        }

        draw_list->AddText( font, font_size, ImVec2( x, y ), color, buffer );
    }

    void string( float x, float y, float font_size, uint32_t flags, ImU32 color, const char* format, ... ) {
        va_list args;
        va_start( args, format );
        ImU32 outline_color = IM_COL32( 0, 0, 0, ( color >> 24 ) & 0xFF );
        string_ex( x, y, font_size, flags, color, outline_color, format, args );
        va_end( args );
    }

    void string( float x, float y, float font_size, uint32_t flags, ImU32 color, ImU32 outline_color, const char* format, ... ) {
        va_list args;
        va_start( args, format );
        string_ex( x, y, font_size, flags, color, outline_color, format, args );
        va_end( args );
    }

    void image( void* texture, float x, float y, float w, float h, float rounding = 0.0f ) {
        if ( !texture ) return;
        
        if ( rounding > 0.0f ) {
            draw_list->AddImageRounded( (ImTextureID)texture, ImVec2( x, y ), ImVec2( x + w, y + h ), ImVec2( 0, 0 ), ImVec2( 1, 1 ), IM_COL32_WHITE, rounding );
        } else {
            draw_list->AddImage( (ImTextureID)texture, ImVec2( x, y ), ImVec2( x + w, y + h ), ImVec2( 0, 0 ), ImVec2( 1, 1 ), IM_COL32_WHITE );
        }
    }
};

#define esp_render c_esp_renderer::get()

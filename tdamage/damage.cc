#include <iostream>
#include <xcb/xcb.h>
#include <xcb/damage.h>
#include <unistd.h>

using std::cout;
using std::endl;

uint32_t gMask = XCB_GC_FOREGROUND;


xcb_point_t points[] = {
	
	{ 10, 10 },
	{ 40, 40 }
	
    };

int main()
{
    xcb_connection_t *c;
    xcb_window_t window;
    xcb_screen_t *screen;
    xcb_generic_event_t *e;
    const xcb_query_extension_reply_t *r;
    xcb_damage_query_version_reply_t *damageExt;
    xcb_damage_damage_t damage;
    xcb_gcontext_t ctx;
    bool run;
    
    xcb_rectangle_t rect = { 0, 0, 10, 20 };
    
    c = xcb_connect( 0, 0 );
    screen = xcb_setup_roots_iterator( xcb_get_setup( c ) ).data;
    window = xcb_generate_id( c );
    ctx = xcb_generate_id( c );
    
    uint32_t mask = XCB_CW_EVENT_MASK | XCB_CW_BACK_PIXEL;
    uint32_t value[] = { screen->white_pixel, XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE };
    
    xcb_create_window( c, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, 300, 300, 0, 
		       XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask, value );
    xcb_map_window( c, window );
    xcb_flush( c );    
    
    uint32_t gList = screen->black_pixel;
    xcb_create_gc( c, ctx, window, gMask, &gList );
    
    run = true;
    while( ( e = xcb_wait_for_event( c ) ) )
    {
	switch( e->response_type & ~0x80 )
	{
	    case XCB_EXPOSE:
		cout << ":: expose" << endl;
		
		xcb_poly_point( c, XCB_COORD_MODE_ORIGIN, window, ctx, 2, points );
		xcb_flush( c );
		
		break;
	    case XCB_KEY_PRESS:
		
		run = true;
		rect.x++;
		rect.y++;
		xcb_poly_rectangle( c, window, ctx, 1, &rect );
		xcb_flush( c );
		
		break;
	}
	
	delete e;
    }
    
    xcb_disconnect( c );
    
    return 0;
}
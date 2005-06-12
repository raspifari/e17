/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"

/* local subsystem functions */
typedef struct _E_Winlist_Win E_Winlist_Win;

struct _E_Winlist_Win
{
   Evas_Object *bg_object;
   Evas_Object *icon_object;
   E_Border *border;
};

static void _e_winlist_size_adjust(void);
static void _e_winlist_border_add(E_Border *bd, E_Zone *zone, E_Desk *desk);
static void _e_winlist_border_del(E_Border *bd);
static void _e_winlist_activate_nth(int n);
static void _e_winlist_activate(void);
static void _e_winlist_deactivate(void);
static void _e_winlist_show_active(void);
static int _e_winlist_cb_event_border_add(void *data, int type,  void *event);
static int _e_winlist_cb_event_border_remove(void *data, int type,  void *event);
static int _e_winlist_cb_key_down(void *data, int type, void *event);
static int _e_winlist_cb_key_up(void *data, int type, void *event);

/* local subsystem globals */
static E_Popup *winlist = NULL;
static Evas_Object *bg_object = NULL;
static Evas_Object *list_object = NULL;
static Evas_List *wins = NULL;
static Evas_List *win_selected = NULL;
static int hold_count = 0;
static int hold_mod = 0;
static Evas_List *handlers = NULL;
static Ecore_X_Window input_window = 0;

/* FIXME: gfx are UGLY. theyare test gfx and nothng more atm */
/* FIXME: support optional warp pointer to window */
/* FIXME: add mouse downa nd up handlers and pass events to bindings from them incase mouse binding starst winlist */

/* externally accessible functions */
int
e_winlist_init(void)
{
   return 1;
}

int
e_winlist_shutdown(void)
{
   e_winlist_hide();
   return 1;
}

int
e_winlist_show(E_Zone *zone)
{
   int x, y, w, h;
   Evas_Object *o;
   Evas_List *l;
   E_Desk *desk;
   E_Border *bd;
   
   if (winlist) return 0;

   /* FIXME: sizes/pos should be config */
   w = zone->w / 2;
   if (w > 320) w = 320;
   h = zone->h / 2;
   if (h > 800) h = 800;
   else if (h < 400) h = 400;
   x = (zone->w - w) / 2;
   y = (zone->h - h) / 2;
   
   winlist = e_popup_new(zone, x, y, w, h); 
   if (!winlist) return;
   e_popup_layer_set(winlist, 255);
   evas_event_freeze(winlist->evas);
   o = edje_object_add(winlist->evas);
   bg_object = o;
   e_theme_edje_object_set(o, "base/theme/winlist",
			   "widgets/winlist/main");
   evas_object_move(o, 0, 0);
   evas_object_resize(o, w, h);
   evas_object_show(o);
   e_popup_edje_bg_object_set(winlist, o);

   o = e_box_add(winlist->evas);
   list_object = o;
   e_box_align_set(o, 0.5, 0.0);
   e_box_orientation_set(o, 0);
   e_box_homogenous_set(o, 1);
   edje_object_part_swallow(bg_object, "list_swallow", o);
   evas_object_show(o);

   desk = e_desk_current_get(winlist->zone);
   e_box_freeze(list_object);
   for (l = e_border_focus_stack_get(); l; l = l->next)
     {
	E_Border *bd;
	
	bd = l->data;
	_e_winlist_border_add(bd, winlist->zone, desk);
     }
   e_box_thaw(list_object);
   bd = e_border_focused_get();
   if (bd) e_border_focus_set(bd, 0, 0);
   _e_winlist_activate_nth(0);
   evas_event_thaw(winlist->evas);
   _e_winlist_size_adjust();

   input_window = ecore_x_window_input_new(zone->container->win, 0, 0, 1, 1);
   ecore_x_window_show(input_window);
   ecore_x_pointer_grab(input_window);
   ecore_x_keyboard_grab(input_window);
   handlers = evas_list_append
     (handlers, ecore_event_handler_add
      (E_EVENT_BORDER_ADD, _e_winlist_cb_event_border_add, NULL));
   handlers = evas_list_append
     (handlers, ecore_event_handler_add
      (E_EVENT_BORDER_REMOVE, _e_winlist_cb_event_border_remove, NULL));
   handlers = evas_list_append
     (handlers, ecore_event_handler_add
      (ECORE_X_EVENT_KEY_DOWN, _e_winlist_cb_key_down, NULL));
   handlers = evas_list_append
     (handlers, ecore_event_handler_add
      (ECORE_X_EVENT_KEY_UP, _e_winlist_cb_key_up, NULL));
   
   e_popup_show(winlist);
   return 1;
}

void
e_winlist_hide(void)
{
   if (!winlist) return;
   
   /* FIXME: ensure whatever window is selected is focused after we finish cleanup */
   
   evas_event_freeze(winlist->evas);
   e_popup_hide(winlist);
   e_box_freeze(list_object);
   while (wins)
     {
	E_Winlist_Win *ww;
	
	ww = wins->data;
	evas_object_del(ww->bg_object);
	evas_object_del(ww->icon_object);
	free(ww);
	wins = evas_list_remove_list(wins, wins);
     }
   e_box_thaw(list_object);
   win_selected = NULL;
   evas_object_del(list_object);
   list_object = NULL;
   evas_object_del(bg_object);
   bg_object = NULL;
   evas_event_thaw(winlist->evas);
   e_object_del(E_OBJECT(winlist));
   winlist = NULL;
   hold_count = 0;
   hold_mod = 0;
   while (handlers)
     {
	ecore_event_handler_del(handlers->data);
	handlers = evas_list_remove_list(handlers, handlers);
     }
   ecore_x_window_del(input_window);
   input_window = 0;
}

void
e_winlist_next(void)
{
   _e_winlist_deactivate();
   win_selected = win_selected->next;
   if (!win_selected) win_selected = wins;
   _e_winlist_show_active();
   _e_winlist_activate();
}

void
e_winlist_prev(void)
{
   _e_winlist_deactivate();
   win_selected = win_selected->prev;
   if (!win_selected) win_selected = evas_list_last(wins);
   _e_winlist_show_active();
   _e_winlist_activate();
}

void
e_winlist_modifiers_set(int mod)
{
   hold_mod = mod;
   hold_count = 0;
   if (hold_mod & ECORE_X_MODIFIER_SHIFT) hold_count++;
   if (hold_mod & ECORE_X_MODIFIER_CTRL) hold_count++;
   if (hold_mod & ECORE_X_MODIFIER_ALT) hold_count++;
   if (hold_mod & ECORE_X_MODIFIER_WIN) hold_count++;
}

/* local subsystem functions */
static void
_e_winlist_size_adjust(void)
{
   Evas_Coord mw, mh;
   int x, y, w, h;   

   e_box_freeze(list_object);
   e_box_min_size_get(list_object, &mw, &mh);
   edje_extern_object_min_size_set(list_object, mw, mh);
   edje_object_part_swallow(bg_object, "list_swallow", list_object);
   edje_object_size_min_calc(bg_object, &mw, &mh);
   edje_extern_object_min_size_set(list_object, -1, -1);
   e_box_thaw(list_object);
   
   /* FIXME: sizes/pos should be config */
   w = winlist->zone->w / 2;
   if (w > 320) w = 320;
   h = mh;
   if (h > 800) h = 800;
   x = (winlist->zone->w - w) / 2;
   y = (winlist->zone->h - h) / 2;
   evas_object_resize(bg_object, w, h);
   e_popup_move_resize(winlist, x, y, w, h);
}

static void
_e_winlist_border_add(E_Border *bd, E_Zone *zone, E_Desk *desk)
{
   if ((bd->zone) &&
       (bd->zone == zone) &&
       ((bd->desk == desk) || (bd->sticky)))
     {
	E_Winlist_Win *ww;
	Evas_Coord mw, mh;
	Evas_Object *o;
	
	ww = calloc(1, sizeof(E_Winlist_Win));
	if (!ww) return;
	ww->border = bd;
	wins = evas_list_append(wins, ww);
	o = edje_object_add(winlist->evas);
	ww->bg_object = o;
	e_theme_edje_object_set(o, "base/theme/winlist",
				"widgets/winlist/item");
	if (bd->client.netwm.name)
	  edje_object_part_text_set(o, "title_text", bd->client.netwm.name);
	else if (bd->client.icccm.title)
	  edje_object_part_text_set(o, "title_text", bd->client.icccm.title);
	evas_object_show(o);
	o = e_border_icon_add(bd, winlist->evas);
	ww->icon_object = o;
	edje_object_part_swallow(ww->bg_object, "icon_swallow", o);
	evas_object_show(o);
	edje_object_size_min_calc(ww->bg_object, &mw, &mh);
	e_box_pack_end(list_object, ww->bg_object);
	e_box_pack_options_set(ww->bg_object, 
			       1, 1, /* fill */
			       1, 0, /* expand */
			       0.5, 0.5, /* align */
			       mw, mh, /* min */
			       9999, mh /* max */
			       );
     }
}

static void
_e_winlist_border_del(E_Border *bd)
{
   Evas_List *l;
   
   for (l = wins; l; l = l->next)
     {
	E_Winlist_Win *ww;
	
	ww = l->data;
	if (ww->border == bd)
	  {
	     evas_object_del(ww->bg_object);
	     evas_object_del(ww->icon_object);
	     free(ww);
	     wins = evas_list_remove_list(wins, l);
	     return;
	  }
     }
}

static void
_e_winlist_activate_nth(int n)
{
   Evas_List *l;
   
   _e_winlist_deactivate();
   l = evas_list_nth_list(wins, n);
   if (l)
     {
	win_selected = l;
	_e_winlist_show_active();
	_e_winlist_activate();
     }
}

static void
_e_winlist_activate(void)
{
   E_Winlist_Win *ww;

   if (!win_selected) return;
   ww = win_selected->data;
   edje_object_signal_emit(ww->bg_object, "active", "");
   e_border_raise(ww->border);
   e_border_focus_set(ww->border, 1, 1);
   edje_object_signal_emit(bg_object, "active", "");
}

static void
_e_winlist_deactivate(void)
{
   E_Winlist_Win *ww;

   if (!win_selected) return;
   ww = win_selected->data;
   edje_object_signal_emit(ww->bg_object, "passive", "");
   e_border_focus_set(ww->border, 0, 0);
}

static void
_e_winlist_show_active(void)
{
   /* FIXME: scroll so the selected win is visible */
}

static int
_e_winlist_cb_event_border_add(void *data, int type,  void *event)
{
   E_Event_Border_Add *ev;

   ev = event;
   _e_winlist_border_add(ev->border, winlist->zone,
			 e_desk_current_get(winlist->zone));
   _e_winlist_size_adjust();
}

static int
_e_winlist_cb_event_border_remove(void *data, int type,  void *event)
{
   E_Event_Border_Remove *ev;

   ev = event;
   _e_winlist_border_del(ev->border);
   _e_winlist_size_adjust();
}

static int
_e_winlist_cb_key_down(void *data, int type, void *event)
{
   Ecore_X_Event_Key_Down *ev;
   
   ev = event;
   if (ev->win != input_window) return 1;
   if      (!strcmp(ev->keysymbol, "Up"))
     e_winlist_prev();
   else if (!strcmp(ev->keysymbol, "Down"))
     e_winlist_next();
   else if (!strcmp(ev->keysymbol, "Left"))
     e_winlist_prev();
   else if (!strcmp(ev->keysymbol, "Right"))
     e_winlist_next();
   else if (!strcmp(ev->keysymbol, "Return"))
     e_winlist_hide();
   else if (!strcmp(ev->keysymbol, "space"))
     e_winlist_hide();
   else if (!strcmp(ev->keysymbol, "Escape"))
     e_winlist_hide();
   else if (!strcmp(ev->keysymbol, "1"))
     _e_winlist_activate_nth(0);
   else if (!strcmp(ev->keysymbol, "2"))
     _e_winlist_activate_nth(1);
   else if (!strcmp(ev->keysymbol, "3"))
     _e_winlist_activate_nth(2);
   else if (!strcmp(ev->keysymbol, "4"))
     _e_winlist_activate_nth(3);
   else if (!strcmp(ev->keysymbol, "5"))
     _e_winlist_activate_nth(4);
   else if (!strcmp(ev->keysymbol, "6"))
     _e_winlist_activate_nth(5);
   else if (!strcmp(ev->keysymbol, "7"))
     _e_winlist_activate_nth(6);
   else if (!strcmp(ev->keysymbol, "8"))
     _e_winlist_activate_nth(7);
   else if (!strcmp(ev->keysymbol, "9"))
     _e_winlist_activate_nth(8);
   else if (!strcmp(ev->keysymbol, "0"))
     _e_winlist_activate_nth(9);
   else
     e_bindings_key_down_event_handle(E_BINDING_CONTEXT_WINLIST, 
				      E_OBJECT(winlist->zone), ev);
   return 1;
}

static int
_e_winlist_cb_key_up(void *data, int type, void *event)
{
   Ecore_X_Event_Key_Up *ev;
   
   ev = event;
   if (ev->win != input_window) return 1;
   if (hold_mod)
     {
	if      ((hold_mod & ECORE_X_MODIFIER_SHIFT) && (!strcmp(ev->keysymbol, "Shift_L"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_SHIFT) && (!strcmp(ev->keysymbol, "Shift_R"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_CTRL) && (!strcmp(ev->keysymbol, "Control_L"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_CTRL) && (!strcmp(ev->keysymbol, "Control_R"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_ALT) && (!strcmp(ev->keysymbol, "Alt_L"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_ALT) && (!strcmp(ev->keysymbol, "Alt_R"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_ALT) && (!strcmp(ev->keysymbol, "Meta_L"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_ALT) && (!strcmp(ev->keysymbol, "Meta_R"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_ALT) && (!strcmp(ev->keysymbol, "Super_L"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_ALT) && (!strcmp(ev->keysymbol, "Super_R"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_WIN) && (!strcmp(ev->keysymbol, "Super_L"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_WIN) && (!strcmp(ev->keysymbol, "Super_R"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_WIN) && (!strcmp(ev->keysymbol, "Mode_switch"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_WIN) && (!strcmp(ev->keysymbol, "Meta_L"))) hold_count--;
	else if ((hold_mod & ECORE_X_MODIFIER_WIN) && (!strcmp(ev->keysymbol, "Meta_R"))) hold_count--;
	if (hold_count <= 0)
	  {
	     e_winlist_hide();
	     return 1;
	  }
     }
   e_bindings_key_up_event_handle(E_BINDING_CONTEXT_WINLIST,
				  E_OBJECT(winlist->zone), ev);
   return 1;
}

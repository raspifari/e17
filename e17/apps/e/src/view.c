#include "e.h"

static Evas_List views = NULL;
static Eevent *current_ev = NULL;

static Ev_Key_Modifiers mulit_select_mod = EV_KEY_MODIFIER_SHIFT;
static Ev_Key_Modifiers range_select_mod = EV_KEY_MODIFIER_CTRL;

static void e_bg_down_cb(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void e_bg_up_cb(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void e_bg_move_cb(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void e_idle(void *data);
static void e_wheel(Eevent * ev);
static void e_key_down(Eevent * ev);
static void e_key_up(Eevent * ev);
static void e_mouse_down(Eevent * ev);
static void e_mouse_up(Eevent * ev);
static void e_mouse_move(Eevent * ev);
static void e_mouse_in(Eevent * ev);
static void e_mouse_out(Eevent * ev);
static void e_window_expose(Eevent * ev);
static void e_view_handle_fs(EfsdEvent *ev);
static void e_view_handle_fs_restart(void *data);

void
e_view_selection_update(E_View *v)
{
   if ((v->select.on) && (!v->select.obj.middle))
     {
	Evas_Gradient grad;
	
	/*create select objects */
	v->select.obj.middle = evas_add_rectangle(v->evas);	
	evas_set_color(v->evas, v->select.obj.middle,
		       v->select.config.middle.r,
		       v->select.config.middle.g,
		       v->select.config.middle.b,
		       v->select.config.middle.a);
	evas_set_layer(v->evas, v->select.obj.middle, 300);
	v->select.obj.edge_l = evas_add_rectangle(v->evas);	
	evas_set_color(v->evas, v->select.obj.edge_l,
		       v->select.config.edge_l.r,
		       v->select.config.edge_l.g,
		       v->select.config.edge_l.b,
		       v->select.config.edge_l.a);
	evas_set_layer(v->evas, v->select.obj.edge_l, 300);
	v->select.obj.edge_r = evas_add_rectangle(v->evas);	
	evas_set_color(v->evas, v->select.obj.edge_r,
		       v->select.config.edge_r.r,
		       v->select.config.edge_r.g,
		       v->select.config.edge_r.b,
		       v->select.config.edge_r.a);
	evas_set_layer(v->evas, v->select.obj.edge_r, 300);
	v->select.obj.edge_t = evas_add_rectangle(v->evas);	
	evas_set_color(v->evas, v->select.obj.edge_t,
		       v->select.config.edge_t.r,
		       v->select.config.edge_t.g,
		       v->select.config.edge_t.b,
		       v->select.config.edge_t.a);
	evas_set_layer(v->evas, v->select.obj.edge_t, 300);
	v->select.obj.edge_b = evas_add_rectangle(v->evas);	
	evas_set_color(v->evas, v->select.obj.edge_b,
		       v->select.config.edge_b.r,
		       v->select.config.edge_b.g,
		       v->select.config.edge_b.b,
		       v->select.config.edge_b.a);
	evas_set_layer(v->evas, v->select.obj.edge_b, 300);
	
	v->select.obj.grad_l = evas_add_gradient_box(v->evas);	
	evas_set_angle(v->evas, v->select.obj.grad_l, 270);
	grad = evas_gradient_new();
	evas_gradient_add_color(grad, 
				v->select.config.grad_l.r,
				v->select.config.grad_l.g,
				v->select.config.grad_l.b,
				v->select.config.grad_l.a, 8);
	evas_gradient_add_color(grad, 
				v->select.config.grad_l.r,
				v->select.config.grad_l.g,
				v->select.config.grad_l.b,
				0, 8);
	evas_set_gradient(v->evas, v->select.obj.grad_l, grad);
	evas_gradient_free(grad);
	evas_set_layer(v->evas, v->select.obj.grad_l, 300);
	v->select.obj.grad_r = evas_add_gradient_box(v->evas);	
	evas_set_angle(v->evas, v->select.obj.grad_r, 90);
	grad = evas_gradient_new();
	evas_gradient_add_color(grad, 
				v->select.config.grad_r.r,
				v->select.config.grad_r.g,
				v->select.config.grad_r.b,
				v->select.config.grad_r.a, 8);
	evas_gradient_add_color(grad, 
				v->select.config.grad_r.r,
				v->select.config.grad_r.g,
				v->select.config.grad_r.b,
				0, 8);
	evas_set_gradient(v->evas, v->select.obj.grad_r, grad);
	evas_gradient_free(grad);
	evas_set_layer(v->evas, v->select.obj.grad_r, 300);
	v->select.obj.grad_t = evas_add_gradient_box(v->evas);	
	evas_set_angle(v->evas, v->select.obj.grad_t, 0);
	grad = evas_gradient_new();
	evas_gradient_add_color(grad, 
				v->select.config.grad_t.r,
				v->select.config.grad_t.g,
				v->select.config.grad_t.b,
				v->select.config.grad_t.a, 8);
	evas_gradient_add_color(grad, 
				v->select.config.grad_t.r,
				v->select.config.grad_t.g,
				v->select.config.grad_t.b,
				0, 8);
	evas_set_gradient(v->evas, v->select.obj.grad_t, grad);
	evas_gradient_free(grad);
	evas_set_layer(v->evas, v->select.obj.grad_t, 300);
	v->select.obj.grad_b = evas_add_gradient_box(v->evas);	
	evas_set_angle(v->evas, v->select.obj.grad_b, 180);
	grad = evas_gradient_new();
	evas_gradient_add_color(grad, 
				v->select.config.grad_b.r,
				v->select.config.grad_b.g,
				v->select.config.grad_b.b,
				v->select.config.grad_b.a, 8);
	evas_gradient_add_color(grad, 
				v->select.config.grad_b.r,
				v->select.config.grad_b.g,
				v->select.config.grad_b.b,
				0, 8);
	evas_set_gradient(v->evas, v->select.obj.grad_b, grad);
	evas_gradient_free(grad);
	evas_set_layer(v->evas, v->select.obj.grad_b, 300);
	v->select.obj.clip = evas_add_rectangle(v->evas);
	evas_set_color(v->evas, v->select.obj.clip, 255, 255, 255, 255);
	evas_set_clip(v->evas, v->select.obj.grad_l, v->select.obj.clip);
	evas_set_clip(v->evas, v->select.obj.grad_r, v->select.obj.clip);
	evas_set_clip(v->evas, v->select.obj.grad_t, v->select.obj.clip);
	evas_set_clip(v->evas, v->select.obj.grad_b, v->select.obj.clip);
     }
   if ((!v->select.on) && (v->select.obj.middle))
     {
	/* destroy select objects */
	evas_del_object(v->evas, v->select.obj.middle);
	evas_del_object(v->evas, v->select.obj.edge_l);
	evas_del_object(v->evas, v->select.obj.edge_r);
	evas_del_object(v->evas, v->select.obj.edge_t);
	evas_del_object(v->evas, v->select.obj.edge_b);
	evas_del_object(v->evas, v->select.obj.grad_l);
	evas_del_object(v->evas, v->select.obj.grad_r);
	evas_del_object(v->evas, v->select.obj.grad_t);
	evas_del_object(v->evas, v->select.obj.grad_b);
	evas_del_object(v->evas, v->select.obj.clip);
	v->select.obj.middle = NULL;
	return;
     }
   if (!v->select.on) return;
   /* move & resize select objects */
     {
	evas_move(v->evas, v->select.obj.edge_l, v->select.x, v->select.y + 1);
	evas_resize(v->evas, v->select.obj.edge_l, 1, v->select.h - 1);
	evas_move(v->evas, v->select.obj.edge_r, v->select.x + v->select.w - 1, v->select.y);
	evas_resize(v->evas, v->select.obj.edge_r, 1, v->select.h - 1);
	evas_move(v->evas, v->select.obj.edge_t, v->select.x, v->select.y);
	evas_resize(v->evas, v->select.obj.edge_t, v->select.w - 1, 1);
	evas_move(v->evas, v->select.obj.edge_b, v->select.x + 1, v->select.y + v->select.h - 1);
	evas_resize(v->evas, v->select.obj.edge_b, v->select.w - 1, 1);
	evas_move(v->evas, v->select.obj.middle, v->select.x + 1, v->select.y + 1);
	evas_resize(v->evas, v->select.obj.middle, v->select.w - 1 - 1, v->select.h - 1 - 1);
	evas_move(v->evas, v->select.obj.grad_l, v->select.x + 1, v->select.y + 1);
	evas_resize(v->evas, v->select.obj.grad_l, v->select.config.grad_size.l, v->select.h - 1 - 1);
	evas_move(v->evas, v->select.obj.grad_r, v->select.x + v->select.w - 1 - v->select.config.grad_size.r, v->select.y + 1);
	evas_resize(v->evas, v->select.obj.grad_r, v->select.config.grad_size.r, v->select.h - 1 - 1);
	evas_move(v->evas, v->select.obj.grad_t, v->select.x + 1, v->select.y + 1);
	evas_resize(v->evas, v->select.obj.grad_t, v->select.w - 1 - 1, v->select.config.grad_size.t);
	evas_move(v->evas, v->select.obj.grad_b, v->select.x + 1, v->select.y + v->select.h - 1 - v->select.config.grad_size.b);
	evas_resize(v->evas, v->select.obj.grad_b, v->select.w - 1 - 1, v->select.config.grad_size.b);
	evas_move(v->evas, v->select.obj.clip, v->select.x + 1, v->select.y + 1);
	evas_resize(v->evas, v->select.obj.clip, v->select.w - 1 - 1, v->select.h - 1 - 1);
     }
   
   evas_show(v->evas, v->select.obj.middle);
   evas_show(v->evas, v->select.obj.edge_l);
   evas_show(v->evas, v->select.obj.edge_r);
   evas_show(v->evas, v->select.obj.edge_t);
   evas_show(v->evas, v->select.obj.edge_b);
   evas_show(v->evas, v->select.obj.grad_l);
   evas_show(v->evas, v->select.obj.grad_r);
   evas_show(v->evas, v->select.obj.grad_t);
   evas_show(v->evas, v->select.obj.grad_b);
   evas_show(v->evas, v->select.obj.clip);
}

static void
e_bg_down_cb(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ev_Mouse_Down          *ev;
   E_View *v;
   
   ev = current_ev->event;
   v = _data;
   if (!(ev->mods & mulit_select_mod))
     {
	e_view_deselect_all();
     }
   if (_b == 1)
     {
	v->select.down.x = _x;
	v->select.down.y = _y;
	v->select.on = 1;
	if (_x < v->select.down.x)
	  {
	     v->select.x = _x;
	     v->select.w = v->select.down.x - v->select.x + 1;
	  }
	else
	  {
	     v->select.x = v->select.down.x;
	     v->select.w = _x - v->select.down.x + 1;
	  }
	if (_y < v->select.down.y)
	  {
	     v->select.y = _y;
	     v->select.h = v->select.down.y - v->select.y + 1;
	  }
	else
	  {
	     v->select.y = v->select.down.y;
	     v->select.h = _y - v->select.down.y + 1;
	  }
	e_view_selection_update(v);
     }
}

static void
e_bg_up_cb(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{    
   Ev_Mouse_Up          *ev;
   E_View *v;
   int dx, dy;
   
   ev = current_ev->event;
   v = _data;
   dx = 0;
   dy = 0;
   if (v->select.on)
     {
	dx = v->select.down.x - _x;
	dy = v->select.down.y - _y;
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;
	if (_b == 1)
	  v->select.on = 0;
	e_view_selection_update(v);
     }
   if ((_b == 1) && ((dx > 3) || (dy > 3)))
     {
	Evas_List l;
	
	for (l = v->icons; l; l = l->next)
	  {
	     E_Icon *ic;
	     
	     ic = l->data;
	     if (INTERSECTS(v->select.x, v->select.y, v->select.w, v->select.h,
			    ic->geom.x, ic->geom.y, ic->geom.w, ic->geom.h))
	       {
		  if (ic->state.visible)
		    {
		       e_view_icon_invert_selection(ic);
		    }
	       }
	  }
	/* errr. select files in box */
     }
   else
     {
	if (_b == 1)
	  {
	     if (!(ev->mods & (mulit_select_mod | range_select_mod)))
	       {
		  static E_Build_Menu *buildmenu = NULL;
		  
		  if (!buildmenu)
		    {
		       char *apps_menu_db;
		       
		       apps_menu_db = e_config_get("apps_menu");
		       if (apps_menu_db) buildmenu = e_build_menu_new_from_db(apps_menu_db);
		    }
		  if (buildmenu)
		    {
		       static E_Menu *menu = NULL;
		       menu = buildmenu->menu;
		       if (menu)
			 e_menu_show_at_mouse(menu, ev->rx, ev->ry, ev->time);
		    }
	       }
	  }
	else if (_b == 3)
	  {
	     static E_Build_Menu *buildmenu = NULL;
	     
	     if (!buildmenu)
	       {		  
		  buildmenu = e_build_menu_new_from_gnome_apps("/usr/share/gnome/apps");
	       }
	     if (buildmenu)
	       {
		  static E_Menu *menu = NULL;
		  menu = buildmenu->menu;
		  if (menu)
		    e_menu_show_at_mouse(menu, ev->rx, ev->ry, ev->time);
	       }
	  }
     }
   if (_b == 1)
     {
	v->select.x = _x;
	v->select.y = _y;
     }
}

static void
e_bg_move_cb(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ev_Mouse_Down          *ev;
   E_View *v;
   
   ev = current_ev->event;
   v = _data;
   if (v->select.on)
     {
	if (_x < v->select.down.x)
	  {
	     v->select.x = _x;
	     v->select.w = v->select.down.x - v->select.x + 1;
	  }
	else
	  {
	     v->select.x = v->select.down.x;
	     v->select.w = _x - v->select.down.x + 1;
	  }
	if (_y < v->select.down.y)
	  {
	     v->select.y = _y;
	     v->select.h = v->select.down.y - v->select.y + 1;
	  }
	else
	  {
	     v->select.y = v->select.down.y;
	     v->select.h = _y - v->select.down.y + 1;
	  }
	e_view_selection_update(v);
     }
}

void
e_view_icon_update_state(E_Icon *ic)
{
   char icon[4096];
   int iw, ih;
   int gw, gh;
   
   if (!ic->info.icon)
     {
	printf("EEEEEEEEEEK %s has no icon\n", ic->file);
	return;
     }
   if (ic->state.clicked)
     {
	sprintf(icon, "%s:/icon/clicked", ic->info.icon);
     }
   else if (ic->state.selected)
     {
	sprintf(icon, "%s:/icon/selected", ic->info.icon);
     }
   else
     {
	sprintf(icon, "%s:/icon/normal", ic->info.icon);
     }
   if ((ic->state.selected) && (!ic->obj.sel.over.icon))
     {
	char file[4096];
	
	sprintf(file, "%s/file.bits.db", e_config_get("selections"));
	ic->obj.sel.over.icon = ebits_load(file);
	sprintf(file, "%s/text.bits.db", e_config_get("selections"));
	ic->obj.sel.over.text = ebits_load(file);
	if (ic->obj.sel.over.icon) 
	  {
	     ebits_add_to_evas(ic->obj.sel.over.icon, ic->view->evas);
	     ebits_set_layer(ic->obj.sel.over.icon, 205);
	  }
	if (ic->obj.sel.over.text)
	  {
	     ebits_add_to_evas(ic->obj.sel.over.text, ic->view->evas);
	     ebits_set_layer(ic->obj.sel.over.text, 205);
	  }
     }
   else if ((!ic->state.selected) && (ic->obj.sel.over.icon))
     {
	if (ic->obj.sel.over.icon) ebits_free(ic->obj.sel.over.icon);
	if (ic->obj.sel.over.text) ebits_free(ic->obj.sel.over.text);
	ic->obj.sel.over.icon = NULL;
	ic->obj.sel.over.text = NULL;
     }
   evas_set_image_file(ic->view->evas, ic->obj.icon, icon);
   evas_get_image_size(ic->view->evas, ic->obj.icon, &iw, &ih);
   gw = ic->geom.icon.w;
   gh = ic->geom.icon.h;
   e_view_icon_apply_xy(ic);
   if ((iw == gw) && (ih == gh)) return;
   e_view_queue_resort(ic->view);
}

void
e_view_deselect_all(void)
{
   Evas_List ll;
   
   for (ll = views; ll; ll = ll->next)
     {
	Evas_List l;
	E_View *v;
	
	v = ll->data;
	for (l = v->icons; l; l = l->next)
	  {
	     E_Icon *ic;
	     
	     ic = l->data;
	     e_view_icon_deselect(ic);
	  }
     }
}

void
e_view_deselect_all_except(E_Icon *not_ic)
{
   Evas_List ll;
   
   for (ll = views; ll; ll = ll->next)
     {
	Evas_List l;
	E_View *v;
	
	v = ll->data;
	for (l = v->icons; l; l = l->next)
	  {
	     E_Icon *ic;
	     
	     ic = l->data;
	     if (ic != not_ic)
	       e_view_icon_deselect(ic);
	  }
     }
}

void
e_view_icon_invert_selection(E_Icon *ic)
{
   if (ic->state.selected) e_view_icon_deselect(ic);
   else e_view_icon_select(ic);
}

void
e_view_icon_select(E_Icon *ic)
{
   if (!ic->state.selected)
     {
	ic->state.selected = 1;
	ic->view->sel_count++;
	e_view_icon_update_state(ic);
     }
}

void
e_view_icon_deselect(E_Icon *ic)
{
   if (ic->state.selected)
     {
	ic->state.selected = 0;
	ic->view->sel_count--;
	e_view_icon_update_state(ic);
     }
}

static void
e_icon_down_cb(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   E_Icon *ic;
   Eevent *ev;
   Ev_Mouse_Down *e;
   
   ev = e_view_get_current_event();
   e = ev->event;
   ic = _data;
   ic->view->select.down.x = _x;
   ic->view->select.down.y = _y;
   ic->state.clicked = 1;
   e_view_icon_update_state(ic);
   if (_b == 1)
     {
	if (e->double_click)
	  {
	     
	  }
	else
	  {
	     if (!ic->state.selected)
	       {
		  if ((e->mods & mulit_select_mod))
		    { 
		       e_view_icon_select(ic);
		    }
		  else
		    {
		       e_view_deselect_all_except(ic);
		       e_view_icon_select(ic);
		    }
		  ic->state.just_selected = 1;
	       }
	  }
     }
   else if (_b == 2)
     {
     }
   else if (_b == 3)
     {
     }
}

static void
e_icon_up_cb(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   E_Icon *ic;
   Eevent *ev;
   Ev_Mouse_Up *e;
   
   ev = e_view_get_current_event();
   e = ev->event;
   ic = _data;
   if (ic->view->drag.started)
     {
	ic->state.clicked = 0;
	ic->state.just_selected = 0;
	e_view_icon_update_state(ic);
	e_window_destroy(ic->view->drag.win);
	ic->view->drag.started = 0;
	return;	
     }
   if (_b == 1)
     {
	if ((e->mods & mulit_select_mod))
	  {
	     if ((ic->state.selected) && (!ic->state.just_selected))
	       e_view_icon_deselect(ic);
/*	     e_view_icon_invert_selection(ic);*/
	  }
	else
	  {
	     e_view_deselect_all_except(ic);
	     e_view_icon_select(ic);
	  }
	ic->state.just_selected = 0;
     }
   ic->state.clicked = 0;
   e_view_icon_update_state(ic);
}

static void
e_icon_move_cb(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   E_Icon *ic;
   Eevent *ev;
   Ev_Mouse_Move *e;
   
   ev = e_view_get_current_event();
   e = ev->event;
   ic = _data;
   if (!ic->state.clicked) return;
   if (!ic->view->drag.started)
     {
	int dx, dy;
	
	dx = ic->view->select.down.x - _x;
	dy = ic->view->select.down.y - _y;
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;
	if ((dx > 3) || (dy > 3))
	  {
	     Pixmap pmap, mask;
	     Evas_List l;
	     int x, y, xx, yy, rw, rh, downx, downy, wx, wy, ww, wh;
	     
	     /* find extents of icons to be dragged */
	     x = 999999999;
	     y = 999999999;
	     xx = -999999999;
	     yy = -999999999;
	     for (l = views; l; l = l->next)
	       {
		  E_View *v;
		  Evas_List ll;
		  
		  v = l->data;
		  printf("sel count %i\n", v->sel_count);
		  if (v->sel_count > 0)
		    {
		       for (ll = v->icons; ll; ll = ll->next)
			 {
			    E_Icon *ic;
			    
			    ic = ll->data;
			    if (ic->state.selected)
			      {
				 int ix, iy, iw, ih;
				 
				 ix = ic->geom.x + v->location.x;
				 iy = ic->geom.y + v->location.y;
				 iw = ic->geom.w;
				 ih = ic->geom.h;
				 if (ix < x) x = ix;
				 if (iy < y) y = iy;
				 if ((ix + iw) > xx) xx = ix + iw;
				 if ((iy + ih) > yy) yy = iy + ih;
			      }
			 }
		    }
	       }
	     e_window_get_geometry(0, NULL, NULL, &rw, &rh);
	     downx = ic->view->select.down.x + ic->view->location.x;
	     downy = ic->view->select.down.y + ic->view->location.y;

	     wx = x;
	     ww = xx - x;
	     if (wx < - (rw - downx)) 
	       {
		  wx = - (rw - downx);
		  ww -= (wx - x);
	       }
	     if ((wx + ww) > (rw + downx))
	       ww = (rw + downx) - wx;
	     wy = y;
	     wh = yy - y;
	     if (wy < - (rh - downy)) 
	       {
		  wy = - (rh - downy);
		  wh -= (wy - y);
	       }
	     if ((wy + wh) > (rh + downy))
	       wh = (rh + downy) - wy;
	     
	     ic->view->drag.x = wx;
	     ic->view->drag.y = wy;
	     ic->view->drag.offset.x = downx - wx;
	     ic->view->drag.offset.y = downy - wy;
	     
	     if ((ww < 1) || (wh < 1)) return;
	     ic->view->drag.win = e_window_override_new(0, wx, wy, ww, wh);
	     pmap = e_pixmap_new(ic->view->drag.win, ww, wh, 0);
	     mask = e_pixmap_new(ic->view->drag.win, ww, wh, 1);
	       {
		  Imlib_Image im;
		  
		  im = imlib_create_image(ww, wh);
		  imlib_context_set_image(im);
		  imlib_image_set_has_alpha(1);
		  imlib_context_set_blend(1);
		  imlib_image_clear();
		  for (l = views; l; l = l->next)
		    {
		       E_View *v;
		       Evas_List ll;
		       
		       v = l->data;
		       if (v->sel_count > 0)
			 {
			    for (ll = v->icons; ll; ll = ll->next)
			      {
				 E_Icon *ic;
				 
				 ic = ll->data;
				 if (ic->state.selected)
				   {
				      double ix, iy;
				      int icx, icy;
				      Imlib_Image im2;
				      char icon[4096];
				      
				      evas_get_geometry(ic->view->evas,
							ic->obj.icon,
							&ix, &iy, NULL, NULL);
				      icx = ix + v->location.x - wx;
				      icy = iy + v->location.y - wy;
				      if (!ic->info.icon)
					{
					   printf("EEEEEEEEEEK %s has no icon\n", ic->file);
					   return;
					}
				      if (ic->state.clicked)
					{
					   sprintf(icon, "%s:/icon/clicked", ic->info.icon);
					}
				      else if (ic->state.selected)
					{
					   sprintf(icon, "%s:/icon/selected", ic->info.icon);
					}
				      else
					{
					   sprintf(icon, "%s:/icon/normal", ic->info.icon);
					}
				      im2 = imlib_load_image(icon);
				      if (im2)
					{
					   int iw, ih;
					   
					   imlib_context_set_image(im2);
					   iw = imlib_image_get_width();
					   ih = imlib_image_get_height();
					   imlib_context_set_image(im);
					   imlib_blend_image_onto_image(im2, 1,
									0, 0, iw, ih,
									icx, icy, iw, ih);
					   imlib_context_set_image(im2);
					   imlib_free_image();
					   imlib_context_set_image(im);
					}
				      else
					{
					   printf("eek cant load\n");
					}
				   }
			      }
			 }
		    }
		  imlib_context_set_image(im);
		  imlib_context_set_dither_mask(0);
		  imlib_context_set_dither(1);
		  imlib_context_set_drawable(pmap);
		  imlib_context_set_mask(mask);
		  imlib_context_set_blend(0);
		  imlib_render_image_on_drawable(0, 0);
		  imlib_free_image();
	       }
	     e_window_set_background_pixmap(ic->view->drag.win, pmap);
	     e_window_set_shape_mask(ic->view->drag.win, mask);
	     e_window_ignore(ic->view->drag.win);
	     e_window_raise(ic->view->drag.win);
	     e_window_show(ic->view->drag.win);
	     e_pixmap_free(pmap);
	     e_pixmap_free(mask);
	     ic->view->drag.started = 1;
	  }
     }
   if (ic->view->drag.started)
     {
	int x, y;
	
	x = _x - ic->view->drag.offset.x;
	y = _y - ic->view->drag.offset.y;
	ic->view->drag.x = x;
	ic->view->drag.y = y;
	ic->view->drag.update = 1;
	ic->view->changed = 1;
     }
}

static void
e_idle(void *data)
{
   Evas_List l;

   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;
	e_view_update(v);
     }
   return;
   UN(data);
}

static void 
e_wheel(Eevent * ev)
{
   Ev_Wheel           *e;
   Evas_List l;
   
   e = ev->event;
   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;
	if (e->win == v->win.main)
	  {
	  }
     }
}

static void
e_key_down(Eevent * ev)
{
   Ev_Key_Down          *e;
   Evas_List l;
   
   e = ev->event;
   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;	
	if ((e->win == v->win.base) ||
	    (e->win == v->win.main))
	  {
	     if (!strcmp(e->key, "Up"))
	       {
	       }
	     else if (!strcmp(e->key, "Down"))
	       {
	       }
	     else if (!strcmp(e->key, "Left"))
	       {
	       }
	     else if (!strcmp(e->key, "Right"))
	       {
	       }
	     else if (!strcmp(e->key, "Escape"))
	       {
	       }
	     else
	       {
		  char *type;
		  
		  type = e_key_press_translate_into_typeable(e);
		  if (type)
		    {
		    }
	       }
	     return;
	  }
     }
}

static void
e_key_up(Eevent * ev)
{
   Ev_Key_Up          *e;
   Evas_List l;
   
   e = ev->event;
   return;
   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;
     }
}

static void
e_mouse_down(Eevent * ev)
{
   Ev_Mouse_Down          *e;
   Evas_List l;
   
   e = ev->event;
   current_ev = ev;
   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;
	if (e->win == v->win.main)
	  {
	     evas_event_button_down(v->evas, e->x, e->y, e->button);
	     current_ev = NULL;
	     return;
	  }
     }
   current_ev = NULL;
}

static void
e_mouse_up(Eevent * ev)
{
   Ev_Mouse_Up          *e;
   Evas_List l;
   
   e = ev->event;
   current_ev = ev;
   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;
	if (e->win == v->win.main)
	  {
	     evas_event_button_up(v->evas, e->x, e->y, e->button);
	     current_ev = NULL;
	     return;
	  }
     }
   current_ev = NULL;
}

static void
e_mouse_move(Eevent * ev)
{
   Ev_Mouse_Move          *e;
   Evas_List l;
   
   e = ev->event;
   current_ev = ev;
   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;
	if (e->win == v->win.main)
	  {
	     evas_event_move(v->evas, e->x, e->y);
	     return;
	  }
     }
   current_ev = NULL;
}

static void
e_mouse_in(Eevent * ev)
{
   Ev_Window_Enter          *e;
   Evas_List l;
   
   e = ev->event;
   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;
	if (e->win == v->win.main)
	  {
	     /* FIXME: temporary for now- should only do this if its a deskop */
	     /* view and desktops accept focus on mouse. */
	     e_focus_to_window(e->win);
	     evas_event_enter(v->evas);
	     return;
	  }
     }
}

static void
e_mouse_out(Eevent * ev)
{
   Ev_Window_Leave          *e;
   Evas_List l;
   
   e = ev->event;
   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;
	if (e->win == v->win.main)
	  {
	     evas_event_leave(v->evas);
	     return;
	  }
     }
}

static void
e_window_expose(Eevent * ev)
{
   Ev_Window_Expose          *e;
   Evas_List l;
   
   e = ev->event;
   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;
	if (e->win == v->win.main)
	  {
	     if (!(v->pmap))
	       evas_update_rect(v->evas, e->x, e->y, e->w, e->h);
	     v->changed = 1;
	     return;
	  }
     }
}

static void
e_view_handle_fs_restart(void *data)
{
   E_View *v;
   Evas_List icons = NULL, l;
   
   v = data;
   
   printf("e_view_handle_fs_restart\n");
   for (l = v->icons; l; l = l->next)
     {
	icons = evas_list_prepend(icons, l->data);
     }
   if (icons)
     {
	for (l = icons; l; l = l->next)
	  {
	     E_Icon *i;
	     
	     i = l->data;
	     e_view_file_deleted(v->monitor_id, i->file);
	  }
	evas_list_free(icons);
     }
   if (e_fs_get_connection())
     v->monitor_id = efsd_start_monitor_dir(e_fs_get_connection(), v->dir,
					    efsd_ops(2, 
						     efsd_op_get_stat(), 
						     efsd_op_get_filetype()
						     )
					);
   printf("restarted monitor id (connection = %p), %i for %s\n", e_fs_get_connection(), v->monitor_id, v->dir);
   v->is_listing = 1;
}

Eevent *
e_view_get_current_event(void)
{
   return current_ev;
}

int
e_view_filter_file(E_View *v, char *file)
{
   if (file[0] == '.') return 0;
   return 1;
}

void
e_view_icon_free(E_Icon *ic)
{
   FREE(ic);
}

E_Icon *
e_view_icon_new(void)
{
   E_Icon *ic;
   
   ic = NEW(E_Icon, 1);
   ZERO(ic, E_Icon, 1);
   OBJ_INIT(ic, e_view_icon_free);
   return ic;
}

E_Icon *
e_view_find_icon_by_file(E_View *view, char *file)
{
   Evas_List l;
   
   for (l = view->icons; l; l = l->next)
     {
	E_Icon *ic;
	
	ic = l->data;
	if ((ic) && (ic->file) && (file) && (!strcmp(ic->file, file)))
	  return ic;
     }
   return NULL;
}

void
e_view_icon_show(E_Icon *ic)
{
   if (ic->state.visible) return;
   ic->state.visible = 1;
   if (!ic->obj.event1)
     {
	ic->obj.event1 = evas_add_rectangle(ic->view->evas);
	ic->obj.event2 = evas_add_rectangle(ic->view->evas);
	evas_set_color(ic->view->evas, ic->obj.event1, 0, 0, 0, 0);
	evas_set_color(ic->view->evas, ic->obj.event2, 0, 0, 0, 0);
	evas_callback_add(ic->view->evas, ic->obj.event1, CALLBACK_MOUSE_DOWN, e_icon_down_cb, ic);
	evas_callback_add(ic->view->evas, ic->obj.event1, CALLBACK_MOUSE_UP, e_icon_up_cb, ic);
	evas_callback_add(ic->view->evas, ic->obj.event1, CALLBACK_MOUSE_MOVE, e_icon_move_cb, ic);
	evas_callback_add(ic->view->evas, ic->obj.event2, CALLBACK_MOUSE_DOWN, e_icon_down_cb, ic);
	evas_callback_add(ic->view->evas, ic->obj.event2, CALLBACK_MOUSE_UP, e_icon_up_cb, ic);
	evas_callback_add(ic->view->evas, ic->obj.event2, CALLBACK_MOUSE_MOVE, e_icon_move_cb, ic);
     }
   evas_set_layer(ic->view->evas, ic->obj.icon, 200);
   evas_set_layer(ic->view->evas, ic->obj.text, 200);
   evas_set_layer(ic->view->evas, ic->obj.event1, 210);
   evas_set_layer(ic->view->evas, ic->obj.event2, 210);
   
   evas_show(ic->view->evas, ic->obj.icon);
   evas_show(ic->view->evas, ic->obj.text);
   evas_show(ic->view->evas, ic->obj.event1);
   evas_show(ic->view->evas, ic->obj.event2);
}

void
e_view_icon_hide(E_Icon *ic)
{
   if (!ic->state.visible) return;
   ic->state.visible = 0;
   evas_hide(ic->view->evas, ic->obj.icon);
   evas_hide(ic->view->evas, ic->obj.text);
   evas_hide(ic->view->evas, ic->obj.event1);
   evas_hide(ic->view->evas, ic->obj.event2);
}

void
e_view_icon_apply_xy(E_Icon *ic)
{
   /* threse calc icon extents for: */
   /*  [I]  */
   /*  Ig   */
   /* [txt] */
   if (ic->geom.text.w > ic->geom.icon.w) ic->geom.w = ic->geom.text.w;
   else ic->geom.w = ic->geom.icon.w;
   ic->geom.h = ic->geom.icon.h + ic->geom.text.h + ic->view->spacing.icon.g;
   
   evas_resize(ic->view->evas, ic->obj.event1, 
	       ic->geom.icon.w, ic->geom.icon.h);
   evas_resize(ic->view->evas, ic->obj.event2, 
	       ic->geom.text.w, ic->geom.text.h);
   evas_move(ic->view->evas, ic->obj.event1,
	     ic->geom.x + ((ic->geom.w - ic->geom.icon.w) / 2),
	                  ic->geom.y);
   evas_move(ic->view->evas, ic->obj.event2,
	     ic->geom.x + ((ic->geom.w - ic->geom.text.w) / 2),
	     ic->geom.y + ic->geom.icon.h + ic->view->spacing.icon.g);	     
   evas_move(ic->view->evas, ic->obj.icon, 
	     ic->geom.x + ((ic->geom.w - ic->geom.icon.w) / 2), 
	     ic->geom.y);
   evas_move(ic->view->evas, ic->obj.text,
	     ic->geom.x + ((ic->geom.w - ic->geom.text.w) / 2), 
	     ic->geom.y + ic->geom.icon.h + ic->view->spacing.icon.g);
   if (ic->obj.sel.over.icon)
     {
	int pl, pr, pt, pb;
	
	ebits_get_insets(ic->obj.sel.over.icon, &pl, &pr, &pt, &pb);
	ebits_move(ic->obj.sel.over.icon, 
		   ic->geom.x + ((ic->geom.w - ic->geom.icon.w) / 2) - pl,
		   ic->geom.y - pt);
	ebits_resize(ic->obj.sel.over.icon,
		     ic->geom.icon.w + pl + pr, ic->geom.icon.h + pt + pb);
	ebits_show(ic->obj.sel.over.icon);
     }
   if (ic->obj.sel.over.text)
     {
	int pl, pr, pt, pb;
	
	ebits_get_insets(ic->obj.sel.over.text, &pl, &pr, &pt, &pb);
	ebits_move(ic->obj.sel.over.text, 
		   ic->geom.x + ((ic->geom.w - ic->geom.text.w) / 2) - pl, 
		   ic->geom.y + ic->geom.icon.h + ic->view->spacing.icon.g - pt);
	ebits_resize(ic->obj.sel.over.text,
		     ic->geom.text.w + pl  + pr, ic->geom.text.h + pt + pb);
	ebits_show(ic->obj.sel.over.text);
     }
}

static int
e_view_restart_alphabetical_qsort_cb(void *data1, void *data2)
{
   E_Icon *ic, *ic2;
   
   ic = *((E_Icon **)data1);
   ic2 = *((E_Icon **)data2);
   return (strcmp(ic->file, ic2->file));
}

void
e_view_resort_alphabetical(E_View *v)
{
   Evas_List icons = NULL, l;
   E_Icon **array;
   int i, count;
   
   if (!v->icons) return;
   for (count = 0, l = v->icons; l; l = l->next) count++;
   array = malloc(sizeof(E_Icon *) * count);
   for (i = 0, l = v->icons; l; l = l->next) array[i++] = l->data;
   printf("qsort %i elements...\n", count);
   qsort(array, count, sizeof(E_Icon *), 
	 e_view_restart_alphabetical_qsort_cb);
   for (i = 0; i < count; i++) icons = evas_list_append(icons, array[i]);
   free(array);
   
   evas_list_free(v->icons);
   v->icons = icons;
   
   printf("done...\n");
}

void
e_view_arrange(E_View *v)
{
   Evas_List l;
   int x, y;
   
   x = v->spacing.window.l;
   y = v->spacing.window.t;
   for (l = v->icons; l; l = l->next)
     {
	E_Icon *ic;
	
	ic = l->data;
	if ((x != v->spacing.window.l) && ((x + ic->geom.w) > v->size.w))
	  {
	     x = v->spacing.window.l;
	     y += ic->geom.h + v->spacing.icon.b;
	  }
	ic->geom.x = x;
	ic->geom.y = y;
	e_view_icon_apply_xy(ic);
	x += ic->geom.w + v->spacing.icon.s;
     }
}

void
e_view_resort(E_View *v)
{
   e_view_resort_alphabetical(v);
   e_view_arrange(v);
}

static void
e_view_resort_timeout(int val, void *data)
{
   E_View *v;
   
   v = data;
   e_view_resort(v);
   v->have_resort_queued = 0;
}

void
e_view_queue_resort(E_View *v)
{
   char name[4096];
   
   if (v->have_resort_queued) return;
   v->have_resort_queued = 1;
   sprintf(name, "resort_timer.%s", v->dir);
   e_add_event_timer(name, 1.0, e_view_resort_timeout, 0, v);
}

void
e_view_icon_initial_show(E_Icon *ic)
{
   /* check if we have enuf info and we havent been shown yet */
   if (!ic->info.icon) return;
   if (ic->state.visible) return;
   
   /* first. lets figure out the size of the icon */
   evas_get_image_size(ic->view->evas, ic->obj.icon, 
		       &(ic->geom.icon.w), &(ic->geom.icon.h));
   ic->geom.text.w = (int)evas_get_text_width(ic->view->evas, ic->obj.text);
   ic->geom.text.h = (int)evas_get_text_height(ic->view->evas, ic->obj.text);
   
   /* now lets allocate space for it if we need to */
   ic->geom.x = 999999;
   ic->geom.y = 999999;
   
   /* if needed queue a tiemout for a resort */
   e_view_queue_resort(ic->view);
   
   /* actually show the icon */
   e_view_icon_apply_xy(ic);
   e_view_icon_show(ic);
}

void
e_view_icon_set_mime(E_Icon *ic, char *base, char *mime)
{
   int diff = 0;
   
   if (!ic->info.mime.base) diff = 1;
   if (!ic->info.mime.type) diff = 1;
   if ((ic->info.mime.base) && (strcmp(ic->info.mime.base, base))) diff = 1;
   if ((ic->info.mime.type) && (strcmp(ic->info.mime.base, mime))) diff = 1;
   if (!diff) return;
   if (ic->info.mime.base) free(ic->info.mime.base);
   if (ic->info.mime.type) free(ic->info.mime.type);
   ic->info.mime.base = NULL;
   ic->info.mime.type = NULL;
   ic->info.mime.base = strdup(base);
   ic->info.mime.type = strdup(mime);
   
   printf("%40s: %s/%s\n", ic->file, base, mime);
   
   /* effect changes here */
   if (ic->info.custom_icon) 
     {
	if (ic->info.icon) free(ic->info.icon);
	ic->info.icon = NULL;
	ic->info.icon = strdup(ic->info.custom_icon);
	evas_set_image_file(ic->view->evas, ic->obj.icon, ic->info.custom_icon);
	e_view_queue_resort(ic->view);	
	return;
     }
   /* find an icon */
     {
	char icon[4096];
	char type[4096];	
	char *p;
	int done = 0;
	
	strcpy(type, ic->info.mime.type);
	for (p = type; *p; p++) 
	  {
	     if (*p == '/') *p = '-';
	  }
	do 
	  {
	     sprintf(icon, "%s/data/icons/%s/%s.db", PACKAGE_DATA_DIR,
		     ic->info.mime.base, type);
	     if (e_file_exists(icon))
	       {
		  done = 1;
		  break;
	       }
	     p = strrchr(type, '-');
	     if (p) *p = 0;
	  }
	while (p);
	if (!done)
	  {
	     if (!e_file_exists(icon))
	       {
		  sprintf(icon, "%s/data/icons/%s/default.db", PACKAGE_DATA_DIR,
			  ic->info.mime.base);
		  if (!e_file_exists(icon))
		    sprintf(icon, "%s/data/icons/unknown/default.db", PACKAGE_DATA_DIR);
	       }
	  }
	ic->info.icon = strdup(icon);
     }
   e_view_icon_update_state(ic);
}

void
e_view_icon_set_link(E_Icon *ic, char *link)
{
   if ((!link) && (ic->info.link))
     {
	free(ic->info.link);
	ic->info.link = NULL;
	/* effect changes here */
     }
   else if (link)
     {
	if ((ic->info.link) && (!strcmp(ic->info.link, link)))
	  {
	     free(ic->info.link);
	     ic->info.link = strdup(link);
	     /* effect changes here */
	  }
     }
}

void
e_view_file_added(int id, char *file)
{
   E_View *v;

   /* if we get a path - ignore it - its not a file in the a dir */
   if (!file) return;
   if (file[0] == '/') return;
   v = e_view_find_by_monitor_id(id);
   if (!v) return;
   /* filter files here */
   if (!e_view_filter_file(v, file)) return;
   if (!e_view_find_icon_by_file(v, file))
     {
	E_Icon *ic;
	
	ic = e_view_icon_new();
	ic->view = v;
	ic->file = strdup(file);
	ic->changed = 1;
	ic->obj.icon = evas_add_image_from_file(ic->view->evas, NULL);
	ic->obj.text = evas_add_text(ic->view->evas, "borzoib", 8, ic->file);
	evas_set_color(ic->view->evas, ic->obj.text, 0, 0, 0, 255);
	v->icons = evas_list_append(v->icons, ic);
     }
}

void
e_view_file_deleted(int id, char *file)
{
   E_View *v;

   if (!file) return;
   if (file[0] == '/') return;
   v = e_view_find_by_monitor_id(id);
   if (!v) return;
   
     {
	E_Icon *ic;
	
	ic = e_view_find_icon_by_file(v, file);
	if (ic)
	  {
	     e_view_icon_hide(ic);
	     OBJ_UNREF(ic);
	     v->icons = evas_list_remove(v->icons, ic);
	     v->changed = 1;
	     e_view_queue_resort(v);
	  }
     }
}

void
e_view_file_changed(int id, char *file)
{
   E_View *v;

   if (!file) return;
   if (file[0] == '/') return;
   v = e_view_find_by_monitor_id(id);
   if (!v) return;

     {
	E_Icon *ic;
	
	ic = e_view_find_icon_by_file(v, file);
	if (ic)
	  {
	  }
     }
}

void
e_view_file_moved(int id, char *file)
{
   E_View *v;

   /* never gets called ? */
   if (!file) return;
   printf(".!WOW!. e_view_file_moved(%i, %s);\n", id, file);
   if (file[0] == '/') return;
   v = e_view_find_by_monitor_id(id);
   if (!v) return;
   
     {
	E_Icon *ic;
	
	ic = e_view_find_icon_by_file(v, file);
	if (ic)
	  {
	  }
     }
}

E_View *
e_view_find_by_monitor_id(int id)
{
   Evas_List l;
   
   for (l = views; l; l = l->next)
     {
	E_View *v;
	
	v = l->data;
	if (v->monitor_id == id) return v;
     }
   return NULL;
}

void
e_view_free(E_View *v)
{
   char name[4096];
   
   sprintf(name, "resort_timer.%s", v->dir);
   e_del_event_timer(name);
   
   views = evas_list_remove(views, v);
   if (v->restarter)
     e_fs_del_restart_handler(v->restarter);
   v->restarter = NULL;
   FREE(v);
}

E_View *
e_view_new(void)
{
   E_View *v;
   
   v = NEW(E_View, 1);
   ZERO(v, E_View, 1);
   OBJ_INIT(v, e_view_free);
#define SOFT_DESK
/* #define X_DESK */
/* #define GL_DESK */
   
#ifdef SOFT_DESK
   /* ONLY alpha software can be "backing stored" */
   v->options.render_method = RENDER_METHOD_ALPHA_SOFTWARE;
   v->options.back_pixmap = 1;
#else
#ifdef X_DESK
   v->options.render_method = RENDER_METHOD_BASIC_HARDWARE;
   v->options.back_pixmap = 0;
#else
   v->options.render_method = RENDER_METHOD_3D_HARDWARE;
   v->options.back_pixmap = 0;
#endif   
#endif
   v->select.config.grad_size.l = 8;
   v->select.config.grad_size.r = 8;
   v->select.config.grad_size.t = 8;
   v->select.config.grad_size.b = 8;
#define SETCOL(_member, _r, _g, _b, _a) \
_member.r = _r; _member.g = _g; _member.b = _b; _member.a = _a;
   SETCOL(v->select.config.edge_l, 0, 0, 0, 255);
   SETCOL(v->select.config.edge_r, 0, 0, 0, 255);
   SETCOL(v->select.config.edge_t, 0, 0, 0, 255);
   SETCOL(v->select.config.edge_b, 0, 0, 0, 255);
   SETCOL(v->select.config.middle, 255, 255, 255, 100);
   SETCOL(v->select.config.grad_l, 255, 255, 255, 100);
   SETCOL(v->select.config.grad_r, 255, 255, 255, 100);
   SETCOL(v->select.config.grad_t, 255, 255, 255, 100);
   SETCOL(v->select.config.grad_b, 255, 255, 255, 100);

   v->spacing.window.l = 3;
   v->spacing.window.r = 3;
   v->spacing.window.t = 3;
   v->spacing.window.b = 3;
   v->spacing.icon.s = 5;
   v->spacing.icon.g = 5;
   v->spacing.icon.b = 5;
   views = evas_list_append(views, v);
   return v;   
}

void
e_view_set_background(E_View *v)
{
   v->changed = 1;
}

void
e_view_set_dir(E_View *v, char *dir)
{
   /* stop monitoring old dir */
   IF_FREE(v->dir);
   v->dir = e_file_real(dir);
   /* start monitoring new dir */

   v->restarter = e_fs_add_restart_handler(e_view_handle_fs_restart, v);
   if (e_fs_get_connection())
     {
	v->monitor_id = efsd_start_monitor_dir(e_fs_get_connection(), v->dir,
					       efsd_ops(2, 
							efsd_op_get_stat(), 
							efsd_op_get_filetype()
							)
					       );
	v->is_listing = 1;
	v->changed = 1;
     }
}

void
e_view_realize(E_View *v)
{
   int max_colors = 216;
   int font_cache = 1024 * 1024;
   int image_cache = 8192 * 1024;
   char *font_dir;
   
   if (v->evas) return;
   v->win.base = e_window_override_new(0, 
				       v->location.x, v->location.y, 
				       v->size.w, v->size.h);   
   font_dir = e_config_get("fonts");
   v->evas = evas_new_all(e_display_get(),
			  v->win.base,
			  0, 0, v->size.w, v->size.h,
			  v->options.render_method,
			  max_colors,
			  font_cache,
			  image_cache,
			  font_dir);
   v->win.main = evas_get_window(v->evas);
   evas_event_move(v->evas, -999999, -999999);
   e_add_child(v->win.base, v->win.main);   
   if (v->options.back_pixmap)
     {
	v->pmap = e_pixmap_new(v->win.main, v->size.w, v->size.h, 0);
	evas_set_output(v->evas, e_display_get(), v->pmap,
			evas_get_visual(v->evas), 
			evas_get_colormap(v->evas));
        e_window_set_background_pixmap(v->win.main, v->pmap);
     }
   if (v->bg)
     {
	v->bg->geom.w = v->size.w;
	v->bg->geom.h = v->size.h;
	e_background_realize(v->bg, v->evas);
     }
   v->obj_bg = evas_add_rectangle(v->evas);
   evas_callback_add(v->evas, v->obj_bg, CALLBACK_MOUSE_DOWN, e_bg_down_cb, v);
   evas_callback_add(v->evas, v->obj_bg, CALLBACK_MOUSE_UP, e_bg_up_cb, v);
   evas_callback_add(v->evas, v->obj_bg, CALLBACK_MOUSE_MOVE, e_bg_move_cb, v);
   evas_set_layer(v->evas, v->obj_bg, 100);
   evas_move(v->evas, v->obj_bg, 0, 0);
   evas_resize(v->evas, v->obj_bg, 999999, 999999);
   evas_set_color(v->evas, v->obj_bg, 0, 0, 0, 0);
   evas_show(v->evas, v->obj_bg);
   
   e_window_set_events(v->win.main, 
		       XEV_EXPOSE | XEV_MOUSE_MOVE | 
		       XEV_BUTTON | XEV_IN_OUT | XEV_KEY);
   e_window_show(v->win.main);
   
     {
	char *dir;
	
	dir = v->dir;
	v->dir = NULL;
	e_view_set_dir(v, dir);
	IF_FREE(dir);
     }
   
   v->changed = 1;
}

void
e_view_update(E_View *v)
{
   Evas_List l;
   
   if (v->changed)
     {
	for (l = v->icons; l; l = l->next)
	  {
	     E_Icon *icon;
	     
	     icon = l->data;
	  }
	if (v->drag.update)
	  {
	     e_window_move(v->drag.win, v->drag.x, v->drag.y);
	     v->drag.update = 0;
	  }
     }
   if (v->options.back_pixmap)
     {
	Imlib_Updates up;
	
	up = evas_render_updates(v->evas);
	/* special code to handle if we are double buffering to a pixmap */
	/* and clear sections of the window if they got updated */
	if (up)
	  {
	     Imlib_Updates u;
	     
	     for (u = up; u; u = imlib_updates_get_next(u))
	       {
		  int x, y, w, h;
		  
		  imlib_updates_get_coordinates(u, &x, &y, &w, &h);
		  e_window_clear_area(v->win.main, x, y, w, h);
	       }
	     imlib_updates_free(up);
	  }
     }
   else
     evas_render(v->evas);
   v->changed = 0;
}


static void
e_view_handle_fs(EfsdEvent *ev)
{
   switch (ev->type)
     {
      case EFSD_EVENT_FILECHANGE:
	switch (ev->efsd_filechange_event.changetype)
	  {
	   case EFSD_FILE_CREATED:
/*	     printf("EFSD_FILE_CREATED: %i %s\n",
		    ev->efsd_filechange_event.id,
		    ev->efsd_filechange_event.file);	     
*/	     e_view_file_added(ev->efsd_filechange_event.id, 
			       ev->efsd_filechange_event.file);
	     break;
	   case EFSD_FILE_EXISTS:
/*	     printf("EFSD_FILE_EXISTS: %i %s\n",
		    ev->efsd_filechange_event.id,
		    ev->efsd_filechange_event.file);	     
*/	     e_view_file_added(ev->efsd_filechange_event.id, 
			       ev->efsd_filechange_event.file);
	     break;
	   case EFSD_FILE_DELETED:
/*	     printf("EFSD_FILE_DELETED: %i %s\n",
		    ev->efsd_filechange_event.id,
		    ev->efsd_filechange_event.file);	     
*/	     e_view_file_deleted(ev->efsd_filechange_event.id, 
				 ev->efsd_filechange_event.file);
	     break;
	   case EFSD_FILE_CHANGED:
/*	     printf("EFSD_CHANGE_CHANGED: %i %s\n",
		    ev->efsd_filechange_event.id,
		    ev->efsd_filechange_event.file);	     
*/	     e_view_file_changed(ev->efsd_filechange_event.id, 
				 ev->efsd_filechange_event.file);
	     break;
	   case EFSD_FILE_MOVED:
/*	     printf("EFSD_CHANGE_MOVED: %i %s\n",
		    ev->efsd_filechange_event.id,
		    ev->efsd_filechange_event.file);	     
*/	     e_view_file_moved(ev->efsd_filechange_event.id, 
			       ev->efsd_filechange_event.file);
	     break;
	   case EFSD_FILE_END_EXISTS:
	       {
		  E_View *v;
		  
		  v = e_view_find_by_monitor_id(ev->efsd_filechange_event.id);
		  if (v) v->is_listing = 0;
/*		  printf("EFSD_CHANGE_END_EXISTS: %i %s\n",
			 ev->efsd_filechange_event.id,
			 ev->efsd_filechange_event.file);	     
*/	       }
	     break;
	   default:
	     break;
	  }
	break;
      case EFSD_EVENT_REPLY:
	switch (ev->efsd_reply_event.command.type)
	  {
	   case EFSD_CMD_REMOVE:
	     break;
	   case EFSD_CMD_MOVE:
	     break;
	   case EFSD_CMD_SYMLINK:
	     break;
	   case EFSD_CMD_LISTDIR:
	     break;
	   case EFSD_CMD_MAKEDIR:
	     break;
	   case EFSD_CMD_CHMOD:
	     break;
	   case EFSD_CMD_GETFILETYPE:
/*	     printf("Getmime event %i\n",
		    ev->efsd_reply_event.command.efsd_file_cmd.id);
*/	     if (ev->efsd_reply_event.status == SUCCESS)
	       {
		  E_Icon *ic;
		  E_View *v;
		  char *file, *file2;
		  
		  file = NULL;
		  if (ev->efsd_reply_event.command.efsd_file_cmd.files)
		    file = ev->efsd_reply_event.command.efsd_file_cmd.files[0];
		  if (file) 
		    {
		       file2 = strrchr(file, '/');
		       if (file2) file = file2 + 1;
		    }
		  v = e_view_find_by_monitor_id(ev->efsd_reply_event.command.efsd_file_cmd.id);
		  if ((v) && (file))
		    {
		       ic = e_view_find_icon_by_file(v, file);
		       if ((ic) &&
			   (ev->efsd_reply_event.data))
			 {
			    char *m, *p;
			    char mime[4096], base[4096];
			    
			    m = ev->efsd_reply_event.data;
			    p = strchr(m, '/');
			    if (p)
			      {
				 strcpy(base, m);
				 strcpy(mime, p + 1);
				 p = strchr(base, '/');
				 *p = 0;
			      }
			    else
			      {
				 strcpy(base, m);
				 strcpy(mime, "unknown");
			      }
/*			    printf("MIME: %s\n", m);
*/			    e_view_icon_set_mime(ic, base, mime);
			    e_view_icon_initial_show(ic);
			 }
		    }
	       }
	     break;
	   case EFSD_CMD_STAT:
/*	     printf("Stat event %i\n",
		    ev->efsd_reply_event.command.efsd_file_cmd.id);
*/	       {
	       }
	     break;
	   case EFSD_CMD_READLINK:
	     if (ev->efsd_reply_event.status == SUCCESS)
	       {
		  E_Icon *ic;
		  E_View *v;
		  
		  char *file, *file2;
		  
		  file = NULL;
		  if (ev->efsd_reply_event.command.efsd_file_cmd.files)
		    file = ev->efsd_reply_event.command.efsd_file_cmd.files[0];
		  if (file) 
		    {
		       file2 = strrchr(file, '/');
		       if (file2) file = file2 + 1;
		    }
		  v = e_view_find_by_monitor_id(ev->efsd_reply_event.command.efsd_file_cmd.id);
		  if ((v) && (file))
		    {
		       ic = e_view_find_icon_by_file(v, file);
		       if ((ic) &&
			   (ev->efsd_reply_event.data))
			 e_view_icon_set_link(ic, (char*)ev->efsd_reply_event.data);
		       e_view_icon_initial_show(ic);
		    }
	       }
	     break;
	   case EFSD_CMD_CLOSE:
	     break;
	   case EFSD_CMD_SETMETA:
	     break;
	   case EFSD_CMD_GETMETA:
	     break;
	   case EFSD_CMD_STARTMON_DIR:
/*	     printf("Startmon event %i\n",
		    ev->efsd_reply_event.command.efsd_file_cmd.id);	     
*/	     break;
	   case EFSD_CMD_STARTMON_FILE:
/*	     printf("Startmon file event %i\n",
		    ev->efsd_reply_event.command.efsd_file_cmd.id);	     
*/	     break;
	   case EFSD_CMD_STOPMON:
	     break;
	   default:
	     break;
	  }
	break;
      default:
	break;
     }
}

void
e_view_init(void)
{
   e_event_filter_handler_add(EV_MOUSE_DOWN,               e_mouse_down);
   e_event_filter_handler_add(EV_MOUSE_UP,                 e_mouse_up);
   e_event_filter_handler_add(EV_MOUSE_MOVE,               e_mouse_move);
   e_event_filter_handler_add(EV_MOUSE_IN,                 e_mouse_in);
   e_event_filter_handler_add(EV_MOUSE_OUT,                e_mouse_out);
   e_event_filter_handler_add(EV_WINDOW_EXPOSE,            e_window_expose);
   e_event_filter_handler_add(EV_KEY_DOWN,                 e_key_down);
   e_event_filter_handler_add(EV_KEY_UP,                   e_key_up);
   e_event_filter_handler_add(EV_MOUSE_WHEEL,              e_wheel);
   e_event_filter_idle_handler_add(e_idle, NULL);
   e_fs_add_event_handler(e_view_handle_fs);
}

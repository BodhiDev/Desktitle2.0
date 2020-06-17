#include <e.h>
#include "e_mod_main.h"

EINTERN int _e_desktitle_log_dom = -1;
Eina_List *edit_global = NULL;

typedef struct _Instance Instance;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object     *ut_obj;
   Eina_List       *handlers;
   E_Menu          *menu;
   Config_Item     *ci;
};

typedef struct _V_Desk V_Desk;

struct _V_Desk
{
   const char *name;
   int         x;
   int         y;
};

static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name,
                                 const char *id, const char *style);
static void             _gc_shutdown(E_Gadcon_Client *gcc);
static void             _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient __UNUSED__);
static const char      *_gc_label(const E_Gadcon_Client_Class *client_class __UNUSED__);
static Evas_Object     *_gc_icon(const E_Gadcon_Client_Class *client_class __UNUSED__, Evas *evas);
static const char      *_gc_id_new(const E_Gadcon_Client_Class *client_class __UNUSED__);
static void             _desktitle_cb_mouse_down(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__,
                                                 void *event_info);
static void             _desktitle_menu_cb_post(void *data, E_Menu *m __UNUSED__);
static void             _desktitle_cb_menu_configure(void *data, E_Menu *m __UNUSED__, E_Menu_Item *mi __UNUSED__);
static void             _eval_instance_size(Instance *inst);
static void             _desktitle_config_apply(void *data, Config_Item *ci __UNUSED__);
static Config_Item     *_desktitle_config_item_get(const char *id);
static Eina_Bool        _desktitle_cb_check(void *data);

static Eina_List *instances = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *desktitle_config = NULL;

static const E_Gadcon_Client_Class _gc_class = {
   GADCON_CLIENT_CLASS_VERSION, "desktitle",
   {
      _gc_init,                 _gc_shutdown,_gc_orient,_gc_label, _gc_icon, _gc_id_new, NULL,
      NULL
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};

static Eina_Bool
_e_zone_cb_desk_after_show(void *data, int type __UNUSED__, void *event  __UNUSED__)
{
   _desktitle_cb_check(data);
   _eval_instance_size(data);
   return ECORE_CALLBACK_PASS_ON;
}

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Instance *inst;
   char buf[PATH_MAX];

   inst = E_NEW(Instance, 1);

   inst->ci = _desktitle_config_item_get(id);

   inst->ut_obj = edje_object_add(gc->evas);
   snprintf(buf, sizeof(buf), "%s/desktitle.edj", e_module_dir_get(desktitle_config->module));
   if (!e_theme_edje_object_set(inst->ut_obj, "base/theme/modules/desktitle",
                                "modules/desktitle/main"))
      edje_object_file_set(inst->ut_obj, buf, "modules/desktitle/main");

   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->ut_obj);
   inst->gcc->data = inst;

   inst->handlers = eina_list_append(inst->handlers, ecore_event_handler_add(
                                        E_EVENT_DESK_AFTER_SHOW, _e_zone_cb_desk_after_show, inst));
   inst->handlers = eina_list_append(inst->handlers, ecore_event_handler_add(
                                        E_EVENT_DESK_NAME_CHANGE, _e_zone_cb_desk_after_show, inst));

   evas_object_event_callback_add(inst->ut_obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  _desktitle_cb_mouse_down, inst);
   instances = eina_list_append(instances, inst);

   _desktitle_cb_check(inst);
   _desktitle_config_apply(inst, inst->ci);

   return inst->gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst = NULL;

   if (!(inst = gcc->data)) return;

   instances = eina_list_remove(instances, inst);

   if (inst->ut_obj)
      {
         evas_object_event_callback_del(inst->ut_obj, EVAS_CALLBACK_MOUSE_DOWN,
                                        _desktitle_cb_mouse_down);
         evas_object_del(inst->ut_obj);
      }

   if (inst->menu)
      {
         e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
         e_object_del(E_OBJECT(inst->menu));
         inst->menu = NULL;
      }

   while (inst->handlers)
      {
         ecore_event_handler_del(inst->handlers->data);
         inst->handlers = eina_list_remove_list(inst->handlers, inst->handlers);
      }

   E_FREE(inst);
}

static void
_eval_instance_size(Instance *inst)
{
   EINA_SAFETY_ON_NULL_RETURN(inst);
   Evas_Coord mw, mh, omw, omh;

   edje_object_size_min_get(inst->ut_obj, &mw, &mh);

   omw = mw;
   omh = mh;

   if ((mw < 1) || (mh < 1))
      {
         Evas_Coord x, y, sw = 0, sh = 0, ow, oh;
         Eina_Bool horiz;

         switch (inst->gcc->gadcon->orient)
            {
             case E_GADCON_ORIENT_TOP:
             case E_GADCON_ORIENT_CORNER_TL:
             case E_GADCON_ORIENT_CORNER_TR:
             case E_GADCON_ORIENT_BOTTOM:
             case E_GADCON_ORIENT_CORNER_BL:
             case E_GADCON_ORIENT_CORNER_BR:
             case E_GADCON_ORIENT_HORIZ:
                horiz = EINA_TRUE;
                break;

             case E_GADCON_ORIENT_LEFT:
             case E_GADCON_ORIENT_CORNER_LB:
             case E_GADCON_ORIENT_CORNER_LT:
             case E_GADCON_ORIENT_RIGHT:
             case E_GADCON_ORIENT_CORNER_RB:
             case E_GADCON_ORIENT_CORNER_RT:
             case E_GADCON_ORIENT_VERT:
                horiz = EINA_FALSE;
                break;

             default:
                horiz = EINA_TRUE;
            }

         if (inst->gcc->gadcon->shelf)
            {
               if (horiz)
                  sh = inst->gcc->gadcon->shelf->h;
               else
                  sw = inst->gcc->gadcon->shelf->w;
            }

         evas_object_geometry_get(inst->ut_obj, NULL, NULL, &ow, &oh);
         evas_object_resize(inst->ut_obj, sw, sh);
         edje_object_parts_extends_calc(inst->ut_obj, &x, &y, &mw, &mh);
         evas_object_resize(inst->ut_obj, mw + 10, mh);
      }

   if (mw < 10) mw = 10;
   if (mh < 10) mh = 10;

   if (mw < omw) mw = omw;
   if (mh < omh) mh = omh;

   e_gadcon_client_min_size_set(inst->gcc, mw + 10, mh);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient __UNUSED__)
{
   _eval_instance_size(gcc->data);
}

static const char *
_gc_label(const E_Gadcon_Client_Class *client_class __UNUSED__)
{
   return D_("DeskTitle");
}

static Evas_Object *
_gc_icon(const E_Gadcon_Client_Class *client_class __UNUSED__, Evas *evas)
{
   Evas_Object *o;

   char buf[PATH_MAX];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-desktitle.edj",
            e_module_dir_get(desktitle_config->module));
   edje_object_file_set(o, buf, "icon");

   return o;
}

static const char *
_gc_id_new(const E_Gadcon_Client_Class *client_class __UNUSED__)
{
   Config_Item *ci;

   ci = _desktitle_config_item_get(NULL);
   return ci->id;
}

V_Desk *
_v_desk_current(Instance *inst)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(inst, NULL);

   E_Desk *desk;
   desk = e_desk_current_get(inst->gcc->gadcon->zone);
   V_Desk *cur = E_NEW(V_Desk, 1);
   cur->name = strdup(desk->name);
   cur->x = desk->x;
   cur->y = desk->y;
   return cur;
}

int
_deskcmp(const void *data1, const void *data2)
{
   const V_Desk *vd1 = data1;
   const V_Desk *vd2 = data2;
   int ret;
   ret = strcmp(vd1->name, vd2->name);
   if (vd1->x != vd2->x || vd1->y != vd2->y)
      ret = 1;
   return ret;
}

static void
_entry_cleanup(Instance *inst)
{
   EINA_SAFETY_ON_NULL_RETURN(inst);
   V_Desk *cur = _v_desk_current(inst);
   V_Desk *exist = (V_Desk *) eina_list_search_unsorted(edit_global, (Eina_Compare_Cb) _deskcmp, cur);
   edit_global = eina_list_remove(edit_global, exist);
   E_FREE(exist);
   E_FREE(cur);
}

static void
_cb_entry_ok(void *data, char *text)
{
   EINA_SAFETY_ON_NULL_RETURN(data);
   Instance *inst = data;
   E_Zone *zone = inst->gcc->gadcon->zone;
   E_Desk *desk = e_desk_current_get(zone);

   _entry_cleanup(inst);
   e_desk_name_del(zone->container->num, zone->num, desk->x, desk->y);
   e_desk_name_add(zone->container->num, zone->num, desk->x, desk->y, text);
   e_desk_name_update();
   _eval_instance_size(inst);
   e_config_save_queue();
}

static void
_cb_entry_cancel(void *data)
{
   EINA_SAFETY_ON_NULL_RETURN(data);

   _entry_cleanup((Instance *) data);
}

static void
_cb_entry_del(void *obj)
{
   EINA_SAFETY_ON_NULL_RETURN(obj);

   _entry_cleanup((Instance *) e_object_data_get(obj));
}

static void
_desktitle_cb_menu_configure(void *data, E_Menu *m __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
   EINA_SAFETY_ON_NULL_RETURN(data);
   Instance *inst = data;

   _config_desktitle_module(inst->ci);
}

static void
_desktitle_cb_mouse_down(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   EINA_SAFETY_ON_NULL_RETURN(data);
   Instance *inst = data;
   Evas_Event_Mouse_Down *ev;
   E_Menu_Item *mi = NULL;
   E_Entry_Dialog *ed = NULL;
   ev = event_info;

   if (ev->button == 1 && ev->flags & EVAS_BUTTON_DOUBLE_CLICK)
      {
         V_Desk *cur = _v_desk_current(inst);

         if (eina_list_search_unsorted(edit_global, (Eina_Compare_Cb) _deskcmp, cur))
            return;
         edit_global = eina_list_append(edit_global, cur);
         ed = e_entry_dialog_show(D_("Edit Desktop Name"), "preferences-desktop",
                                  D_("Enter a name for this desktop:"), cur->name,
                                  D_("Save"), NULL, _cb_entry_ok, _cb_entry_cancel, inst);
         e_object_data_set(E_OBJECT(ed), inst);
         e_object_del_attach_func_set(E_OBJECT(ed), _cb_entry_del);
      }
   if ((ev->button == 3) && (!inst->menu))
      {
         E_Menu *m;
         int x, y;

         m = e_menu_new();
         mi = e_menu_item_new(m);
         e_menu_item_label_set(mi, D_("Settings"));
         e_util_menu_item_theme_icon_set(mi, "preferences-system");
         e_menu_item_callback_set(mi, _desktitle_cb_menu_configure, inst);

         m = e_gadcon_client_util_menu_items_append(inst->gcc, m, 0);
         e_menu_post_deactivate_callback_set(m, _desktitle_menu_cb_post, inst);
         inst->menu = m;
         e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, NULL, NULL);
         e_menu_activate_mouse(m,
                               e_util_zone_current_get(e_manager_current_get()),
                               x + ev->output.x, y + ev->output.y, 1, 1,
                               E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
         evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
                                  EVAS_BUTTON_NONE, ev->timestamp, NULL);
      }
}

static void
_desktitle_menu_cb_post(void *data, E_Menu *m __UNUSED__)
{
   EINA_SAFETY_ON_NULL_RETURN(data);
   Instance *inst = data;
   if (!inst->menu) return;

   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

static Config_Item *
_desktitle_config_item_get(const char *id)
{
   Eina_List *l;
   Config_Item *ci;
   char buf[128];

   if (!id)
      {
         int num = 0;

         /* Create id */
         if (desktitle_config->items)
            {
               const char *p;

               ci = eina_list_last(desktitle_config->items)->data;
               p = strrchr(ci->id, '.');
               if (p)
                  {
                     num = atoi(p + 1) + 1;
                  }
            }
         snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, num);
         id = buf;
      }
   else
      {
         for (l = desktitle_config->items; l; l = l->next)
            {
               ci = l->data;
               if (!ci->id)
                  continue;
               if (strcmp(ci->id, id) == 0)
                  return ci;
            }
      }

   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->color_r = 255;
   ci->color_g = 255;
   ci->color_b = 255;
   ci->color_a = 255;

   desktitle_config->items = eina_list_append(desktitle_config->items, ci);

   return ci;
}

static void
_desktitle_config_apply(void *data, Config_Item *ci __UNUSED__)
{
   EINA_SAFETY_ON_NULL_RETURN(data);
   Instance *inst = data;

   edje_object_color_class_set
      (inst->ut_obj, "border_title_active", inst->ci->color_r, inst->ci->color_g, inst->ci->color_b,
      inst->ci->color_a, 0, 0, 0, 255, 0, 0, 0, 255);
}

void
_desktitle_config_updated(Config_Item *ci)
{
   Eina_List *l;

   if (!desktitle_config)
      return;

   for (l = instances; l; l = l->next)
      {
         Instance *inst = l->data;
         
         if (inst->ci != ci) continue;
         _desktitle_config_apply(inst, ci);
      }
}

EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION, "DeskTitle"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[PATH_MAX];

   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, color_r, DOUBLE);
   E_CONFIG_VAL(D, T, color_g, DOUBLE);
   E_CONFIG_VAL(D, T, color_b, DOUBLE);
   E_CONFIG_VAL(D, T, color_a, DOUBLE);

   conf_edd = E_CONFIG_DD_NEW("Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   desktitle_config = e_config_domain_load("module.desktitle", conf_edd);
   if (!desktitle_config)
      {
         Config_Item *ci;

         desktitle_config = E_NEW(Config, 1);
         ci = E_NEW(Config_Item, 1);
         ci->id = eina_stringshare_add("0");
         ci->color_r = 255;
         ci->color_g = 255;
         ci->color_b = 255;
         ci->color_a = 255;
         desktitle_config->items = eina_list_append(desktitle_config->items, ci);
      }
   /* Initialize Einna_log for developers */
   _e_desktitle_log_dom = eina_log_domain_register("Desktitle", EINA_COLOR_ORANGE);
   eina_log_domain_level_set("Desktitle", EINA_LOG_LEVEL_DBG);
   desktitle_config->module = m;
   e_gadcon_provider_register(&_gc_class);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m __UNUSED__)
{
   e_gadcon_provider_unregister(&_gc_class);

   if (desktitle_config->config_dialog)
      e_object_del(E_OBJECT(desktitle_config->config_dialog));

   desktitle_config->module = NULL;

   while (desktitle_config->items)
      {
         Config_Item *ci;

         ci = desktitle_config->items->data;
         desktitle_config->items = eina_list_remove_list(desktitle_config->items,
                                                         desktitle_config->items);
         if (ci->id)
            eina_stringshare_del(ci->id);
         E_FREE(ci);
      }

   E_FREE(desktitle_config);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   /* Shutdown Logger */
   eina_log_domain_unregister(_e_desktitle_log_dom);
   _e_desktitle_log_dom = -1;
   return 1;
}

EAPI int
e_modapi_save(E_Module *m __UNUSED__)
{
   e_config_domain_save("module.desktitle", conf_edd, desktitle_config);
   return 1;
}

static Eina_Bool
_desktitle_cb_check(void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(data, EINA_FALSE);
   Instance *inst = data;
   E_Desk *desk;

   desk = e_desk_current_get(inst->gcc->gadcon->zone);

   if (desk->name != NULL)
      {
         edje_object_part_text_set(inst->ut_obj, "desktitle", desk->name);
         return EINA_TRUE;
      }

   return EINA_TRUE;
}

#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   E_Color color[3];
};

/* Protos */
static void        *_create_data(E_Config_Dialog *cfd);
static void         _free_data(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd __UNUSED__, Evas *evas,
                                          E_Config_Dialog_Data *cfdata);
static int          _basic_apply_data(E_Config_Dialog *cfd,
                                      E_Config_Dialog_Data *cfdata);
static void         _fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata);

/* Config Calls */
void
_config_desktitle_module(Config_Item *ci)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;
   char buf[4096];

   if (e_config_dialog_find("DeskTitle", "_e_modules_desktitle_config_dialog"))
      return;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return;

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   snprintf(buf, sizeof(buf), "%s/e-module-desktitle.edj",
            e_module_dir_get(desktitle_config->module));
   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con, D_("DeskTitle Settings"), "DeskTitle", "_e_modules_desktitle_config_dialog", buf, 0, v, ci);
   desktitle_config->config_dialog = cfd;
}

static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{
   cfdata->color->r = ci->color_r;
   cfdata->color->g = ci->color_g;
   cfdata->color->b = ci->color_b;
   cfdata->color->a = ci->color_a;
   //_color_cb_change(cfdata, NULL);
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata = NULL;
   Config_Item *ci = NULL;

   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(ci, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata)
{
   if (!desktitle_config) return;
   desktitle_config->config_dialog = NULL;
   E_FREE(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd __UNUSED__, Evas *evas,
                      E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("DeskTitle Settings"), 0);

   of = e_widget_framelist_add(evas, D_("Label color"), 1);

   ob = e_widget_label_add(evas, D_("Click for the color selector"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_color_well_add_full(evas, cfdata->color, 1, 1);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Config_Item *ci = NULL;

   ci = cfd->data;
   ci->color_r = cfdata->color->r;
   ci->color_g = cfdata->color->g;
   ci->color_b = cfdata->color->b;
   ci->color_a = cfdata->color->a;

   e_config_save_queue();
   _desktitle_config_updated(ci);

   return 1;
}

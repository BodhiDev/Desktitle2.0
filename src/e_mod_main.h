#include <libintl.h>
#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config      Config;
typedef struct _Config_Item Config_Item;

struct _Config
{
   E_Module        *module;
   E_Config_Dialog *config_dialog;
   Eina_List       *items;
};

struct _Config_Item
{
   const char *id;
   double      color_r, color_g, color_b, color_a;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module * m __UNUSED__);
EAPI int e_modapi_save(E_Module * m __UNUSED__);

void       _config_desktitle_module(Config_Item *ci);
void       _desktitle_config_updated(Config_Item *ci);

extern Config *desktitle_config;

#endif

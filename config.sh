MODID=ai-furiku
MODPATH=$MOUNTPATH/$MODID
AUTOMOUNT=true
REPLACE="
/system/app/Youtube
/system/priv-app/SystemUI
/system/priv-app/Settings
/system/framework
"
REPLACE="
"

ui_print "******************************"
ui_print "      AI-Furiku Installer     "
ui_print "******************************"
ui_print "- Support Snapdragon/MediaTek"
ui_print "- Version 1.0"

mkdir -p $MODPATH 2>/dev/null
cd $MODPATH
unzip -o "$3" 'system/*' 'module/*' 'common/*' 'config/*' 'obj/*' 'scripts/*' 'module.prop' 'service.sh' 'post-fs-data.sh' -d $MODPATH >&2

set_permissions() {
set_perm_recursive $MODPATH 0 0 0755 0644
set_perm_recursive $MODPATH/system/bin 0 0 0755 0755
set_perm_recursive $MODPATH/scripts 0 0 0755 0755
}
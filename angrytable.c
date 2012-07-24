/*
 * SendButton - Add a Send button to the conversation window entry area.
 * Copyright (C) 2008 Etan Reisner <deryni@pidgin.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <glib.h>

#include "internal.h"

#include "version.h"

#include "pidgin.h"
#include "debug.h"
#include "gtkconv.h"
#include "plugin.h"
#include "gtkplugin.h"

#define PLUGIN_ID "flip-table"

struct fData {
    PidginConversation *conv;
    gchar *send;
} sdata;

struct fData *d = &sdata;

static void
sending_msg_cb(GtkButton *button, PidginConversation *gtkconv) {
    GtkTextBuffer *buffer;
    GtkTextIter start_iter, end_iter;
    // GtkTextIter start, end;
    // PidginConversation *gtkconv;
    GtkTextView *view;
    gchar *tmp;
    gchar *oldmsg;

    view = GTK_TEXT_VIEW(gtkconv->entry);

    buffer = gtk_text_view_get_buffer(view);
        
    // create iter
    // pass it here to instatntiate it
    gtk_text_buffer_get_end_iter(buffer, &end_iter);
    gtk_text_buffer_get_start_iter(buffer, &start_iter);

    // grab all contents
    oldmsg = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);

    // create the message we want to insert
    tmp = g_strdup_printf(" (╯°□°）╯︵ ┻━┻" );

    // insert text and send
    gtk_text_buffer_set_text(buffer, tmp, -1);
    //    gtk_text_buffer_insert(buffer, &end_iter, tmp, -1);
    g_signal_emit_by_name(gtkconv->entry, "message_send");
    
    // refresh end_iter
    gtk_text_buffer_get_end_iter(buffer, &end_iter);

    gtk_text_buffer_set_text(buffer, oldmsg, -1);

}

// this isn't needed, it should always be true
static void
input_buffer_changed(GtkTextBuffer *text_buffer, GtkWidget *tableflip_button) {
    gtk_widget_set_sensitive(tableflip_button, TRUE);
}

static void
popup_menu(GtkAction* action, GtkMenu *menu) {
    gtk_menu_popup(menu, NULL, NULL, NULL, NULL, 1, 
        gtk_get_current_event_time());
}

static void
send_flip_cb(GtkWidget *button, struct fData *data) 
{
    PidginConversation *gtkconv;
    GtkTextBuffer *buffer;  // holds the text of the conv
    GtkTextIter start_iter, end_iter;   // start and end of buffer
    GtkTextView *view;  // view where buffer is
    gchar *tmp; // tmp storage for buffer
    gchar *oldmsg; // holds message while we send stuff
    //fData *mydata = &data;


    gtkconv = (data->conv);
    
	purple_debug_info(PLUGIN_ID,
		"&GTKCONV - %i\n, ", &gtkconv);

    view = GTK_TEXT_VIEW(gtkconv->entry);   // PROBLEMATIC

    // grab buffer from view
    buffer = gtk_text_view_get_buffer(view);
    // create iter
    // pass it here to instatntiate it
    gtk_text_buffer_get_end_iter(buffer, &end_iter);
    gtk_text_buffer_get_start_iter(buffer, &start_iter);

    // grab all contents
    oldmsg = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);

    // create the message we want to insert
    tmp = g_strdup_printf("%s", (data->send));

    // insert text and send
    tmp = g_strdup_printf("ffffff" );
    gtk_text_buffer_set_text(buffer, tmp, -1);

/*
    g_signal_emit_by_name((data->conv)->entry, "message_send");
    
    // refresh end_iter
    gtk_text_buffer_get_end_iter(buffer, &end_iter);

    // put old msg back
    gtk_text_buffer_set_text(buffer, oldmsg, -1);
*/
}

static void 
add_item_to_menu(gchar *button_text, gchar *to_send, GtkWidget *menu, 
    PidginConversation *gtkconv)
{
    GtkWidget *new_flip;
    gchar *testtext;

    
    struct fData dataToSend,*dataToSendp;

    dataToSend.send = to_send;
    dataToSend.conv = gtkconv;

    dataToSendp = &dataToSend;

//    dataToSend->send = &to_send;
//    dataToSend->conv = &gtkconv;
//    sdata.send = to_send;

    // create new menu item with label
    new_flip = gtk_menu_item_new_with_label(button_text);

    // connect to call back with args
    g_signal_connect(G_OBJECT(new_flip), "activate", G_CALLBACK(send_flip_cb),
        &dataToSendp);

    // append to menu
    gtk_menu_shell_append(menu, new_flip);

    // show new button
    gtk_widget_show(new_flip);
}

// creates the actual button on the toolbar in the conversation
static void
create_tableflip_button_pidgin(PidginConversation *gtkconv)
{
    GtkWidget *menu;
    GtkWidget *filemenu;
    GtkWidget *tableflip_button;
    GtkWidget *flip1;
    GtkWidget *where;

    GtkAction *action;
    GtkTextBuffer *buf;
    gint signal_id;

    where = gtkconv->toolbar;
//    sdata.conv = gtkconv;

    tableflip_button = g_object_get_data(G_OBJECT(where),
                                    "tableflip_button");
    
    if (tableflip_button != NULL)
        return;

    // create drop down arrow/button thing
    tableflip_button = gtk_menu_tool_button_new(NULL, NULL);

    // this is the menu for the drop down?
    menu = gtk_menu_new();
    gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(tableflip_button), 
        menu);    

    // action for the toolbar button
    action = gtk_action_new("dropdown-menu", "Dropdown", "(╯°□°）╯︵ ┻━┻",
        GTK_STOCK_PREFERENCES);
    gtk_action_connect_proxy(action, GTK_WIDGET(tableflip_button));
    g_signal_connect(action, "activate", G_CALLBACK(sending_msg_cb), gtkconv);

    // add menuitems
    add_item_to_menu(" (ﾉಥ益ಥ）ﾉ﻿ ︵┻━┻", " (ﾉಥ益ಥ）ﾉ﻿ ︵┻━┻", menu, gtkconv);
    add_item_to_menu(" (ノಠ益ಠ)ノ彡┻━┻", " (ノಠ益ಠ)ノ彡┻━┻", menu, gtkconv);
    add_item_to_menu(" (╯°□°）╯︵ ┻━┻", " (╯°□°）╯︵ ┻━┻", menu, gtkconv);
    add_item_to_menu(" (╯°□°）╯︵ /(.□./)", " (╯°□°）╯︵ /(.□./)", menu, gtkconv);
    add_item_to_menu(" ┬─┬ノ( º _ ºノ)", " ┬─┬ノ( º _ ºノ)", menu, gtkconv);
    add_item_to_menu(" ┬──┬﻿ ¯\_(ツ)", " ┬──┬﻿ ¯\_(ツ)", menu, gtkconv);
    add_item_to_menu(" ┻━┻ ︵ヽ(`□´)ﾉ︵﻿ ┻━┻", " ┻━┻ ︵ヽ(`□´)ﾉ︵﻿ ┻━┻", menu, gtkconv);
    add_item_to_menu(" ┻━┻ ︵ヽ(`Д´)ﾉ︵﻿ ┻━┻", " ┻━┻ ︵ヽ(`Д´)ﾉ︵﻿ ┻━┻", menu, gtkconv);
    add_item_to_menu(" ┻━┻ ︵﻿ ¯\(ツ)/¯ ︵ ┻━┻", " ┻━┻ ︵﻿ ¯\(ツ)/¯ ︵ ┻━┻", menu, gtkconv);
    // add_item_to_menu(" X", " X", menu, gtkconv); // placeholder


    // where to put the button
    gtk_box_pack_end(GTK_BOX(where), tableflip_button, FALSE,
                     FALSE, 0);

    // show the button
    gtk_widget_show(tableflip_button);
    
    // append to menubar
    gtk_menu_bar_append(GTK_MENU_BAR(where), menu);

    // set data of button
    g_object_set_data(G_OBJECT(where), "tableflip_button",
                      tableflip_button);

}

static void
remove_tableflip_button_pidgin(PidginConversation *gtkconv)
{
    GtkWidget *tableflip_button = NULL;

    tableflip_button = g_object_get_data(G_OBJECT(gtkconv->toolbar),
                                    "tableflip_button");
    if (tableflip_button != NULL) {
        GtkTextBuffer *buf;
        guint signal_id;

        buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtkconv->entry));
        signal_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(tableflip_button),
                                                      "buffer-signal"));
        if (buf && signal_id)
            g_signal_handler_disconnect(G_OBJECT(buf), signal_id);

        gtk_widget_destroy(tableflip_button);
        g_object_set_data(G_OBJECT(gtkconv->toolbar),
                          "tableflip_button", NULL);
    }
}

static void
conversation_displayed_cb(PidginConversation *gtkconv)
{
    GtkWidget *tableflip_button = NULL;

    tableflip_button = g_object_get_data(G_OBJECT(gtkconv->toolbar),
                                    "tableflip_button");
    if (tableflip_button == NULL) {
        create_tableflip_button_pidgin(gtkconv);
    }
}

static gboolean
plugin_load(PurplePlugin *plugin)
{
    GList *convs = purple_get_conversations();
    void *gtk_conv_handle = pidgin_conversations_get_handle();

    purple_signal_connect(gtk_conv_handle, "conversation-displayed", plugin,
                          PURPLE_CALLBACK(conversation_displayed_cb), NULL);

    while (convs) {

        PurpleConversation *conv = (PurpleConversation *)convs->data;

        /* Setup Send button */
        if (PIDGIN_IS_PIDGIN_CONVERSATION(conv)) {
            create_tableflip_button_pidgin(PIDGIN_CONVERSATION(conv));
        }

        convs = convs->next;
    }

    return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin)
{
    GList *convs = purple_get_conversations();

    while (convs) {
        PurpleConversation *conv = (PurpleConversation *)convs->data;

        /* Remove Send button */
        if (PIDGIN_IS_PIDGIN_CONVERSATION(conv)) {
            remove_tableflip_button_pidgin(PIDGIN_CONVERSATION(conv));
        }

        convs = convs->next;
    }

    return TRUE;
}

static PurplePluginInfo info =
{
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,                           /**< major version */
    PURPLE_MINOR_VERSION,                           /**< minor version */
    PURPLE_PLUGIN_STANDARD,                         /**< type */
    PIDGIN_PLUGIN_TYPE,                             /**< ui_requirement */
    0,                                              /**< flags */
    NULL,                                           /**< dependencies */
    PURPLE_PRIORITY_DEFAULT,                        /**< priority */

    "gtkflipbutton",                                /**< id */
    N_("Flip Table Button"),                              /**< name */
    DISPLAY_VERSION,                                /**< version */
    N_("Conversation Window Flip Table Button."),         /**< summary */
    N_("Adds a Flip Table button to the toolbar area of "
       "the conversation window."),    /**< description */
    "Nick Caley <potatoz@gmail.com>",              /**< author */
    PURPLE_WEBSITE,                                 /**< homepage */
    plugin_load,                                    /**< load */
    plugin_unload,                                  /**< unload */
    NULL,                                           /**< destroy */
    NULL,                                           /**< ui_info */
    NULL,                                           /**< extra_info */
    NULL,                                           /**< prefs_info */
    NULL,                                           /**< actions */

    /* padding */
    NULL,
    NULL,
    NULL,
    NULL
};

static void
init_plugin(PurplePlugin *plugin)
{
}

PURPLE_INIT_PLUGIN(sendbutton, init_plugin, info)

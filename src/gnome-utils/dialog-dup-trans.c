/********************************************************************\
 * dialog-dup-trans.c -- duplicate transaction dialog               *
 * Copyright (C) 2001 Gnumatic, Inc.                                *
 * Author: Dave Peticolas <dave@krondo.com>                         *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 51 Franklin Street, Fifth Floor    Fax:    +1-617-542-2652       *
 * Boston, MA  02110-1301,  USA       gnu@gnu.org                   *
\********************************************************************/

#include "config.h"

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <time.h>
#include <stdlib.h>

#include "dialog-dup-trans.h"
#include "dialog-utils.h"
#include "gnc-date-edit.h"
#include "qof.h"

/* This static indicates the debugging module that this .o belongs to.  */
G_GNUC_UNUSED static QofLogModule log_module = G_LOG_DOMAIN;

typedef struct
{
    GtkWidget * dialog;

    gboolean focus_out;

    GtkWidget * date_edit;
    GtkWidget * num_edit;

    GtkWidget *duplicate_title_label; // GtkLabel
    GtkWidget *duplicate_table; // GtkTable
    GtkWidget *num_label; // GtkLabel
} DupTransDialog;

/* Parses the string value and returns true if it is a
 * number. In that case, *num is set to the value parsed.
 * Copied from numcell.c */
static gboolean
parse_num (const char *string, long int *num)
{
    long int number;

    if (string == NULL)
        return FALSE;

    if (!gnc_strisnum(string))
        return FALSE;

    number = strtol(string, NULL, 10);

    if ((number == LONG_MIN) || (number == LONG_MAX))
        return FALSE;

    if (num != NULL)
        *num = number;

    return TRUE;
}

static gboolean
gnc_dup_trans_output_cb(GtkSpinButton *spinbutton,
                        gpointer user_data)
{
    gboolean is_number;
    long int num;
    gchar *txt = gtk_editable_get_chars(GTK_EDITABLE(spinbutton), 0, -1);
    is_number = parse_num(txt, &num);
    g_free(txt);
    if (!is_number)
        gtk_entry_set_text(GTK_ENTRY(spinbutton), "");
    return !is_number;
}

static void
gnc_dup_trans_dialog_create (GtkWidget * parent, DupTransDialog *dt_dialog,
                             time_t date, const char *num_str)
{
    GtkWidget *dialog;
    GtkBuilder  *builder;

    builder = gtk_builder_new();
    gnc_builder_add_from_file (builder, "gnc-plugin-page-register.glade", "adjustment1");
    gnc_builder_add_from_file (builder, "gnc-plugin-page-register.glade", "Duplicate Transaction Dialog");

    dialog = GTK_WIDGET(gtk_builder_get_object (builder, "Duplicate Transaction Dialog"));
    dt_dialog->dialog = dialog;

    /* parent */
    if (parent != NULL)
        gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (parent));

    /* date widget */
    {
        GtkWidget *date_edit;
        GtkWidget *hbox;
        GtkWidget *label;

        date_edit = gnc_date_edit_new (date, FALSE, FALSE);
        gnc_date_activates_default(GNC_DATE_EDIT(date_edit), TRUE);
        hbox = GTK_WIDGET(gtk_builder_get_object (builder, "date_hbox"));
        gtk_widget_show (date_edit);

        label = GTK_WIDGET(gtk_builder_get_object (builder, "date_label"));
        gnc_date_make_mnemonic_target (GNC_DATE_EDIT(date_edit), label);

        gtk_box_pack_end (GTK_BOX (hbox), date_edit, TRUE, TRUE, 0);
        dt_dialog->date_edit = date_edit;
    }

    dt_dialog->duplicate_title_label = GTK_WIDGET(gtk_builder_get_object (builder, "duplicate_title_label"));
    dt_dialog->duplicate_table = GTK_WIDGET(gtk_builder_get_object (builder, "duplicate_table"));
    dt_dialog->num_label = GTK_WIDGET(gtk_builder_get_object (builder, "num_label"));

    {
        GtkWidget *num_spin;
        long int num;

        num_spin = GTK_WIDGET(gtk_builder_get_object (builder, "num_spin"));
        dt_dialog->num_edit = num_spin;

        gtk_entry_set_activates_default(GTK_ENTRY(num_spin), TRUE);
        g_signal_connect(num_spin, "output",
                         G_CALLBACK(gnc_dup_trans_output_cb), dt_dialog);

        if (num_str && parse_num (num_str, &num))
            gtk_spin_button_set_value (GTK_SPIN_BUTTON (num_spin), num + 1);
        else
            gtk_entry_set_text (GTK_ENTRY (num_spin), "");
    }

    gtk_builder_connect_signals_full (builder, gnc_builder_connect_full_func, dt_dialog);

    g_object_unref(G_OBJECT(builder));
}

static gboolean
gnc_dup_trans_dialog_internal (GtkWidget * parent, const char* title,
                               time_t *date_p, GDate *gdate_p,
                               const char *num, char **out_num)
{
    DupTransDialog *dt_dialog;
    GNCDateEdit *gde;
    GtkWidget *entry;
    gboolean ok;
    gint result;

    if (!date_p)
        return FALSE;

    dt_dialog = g_new0 (DupTransDialog, 1);

    gnc_dup_trans_dialog_create (parent, dt_dialog, *date_p, num);

    gde = GNC_DATE_EDIT (dt_dialog->date_edit);
    entry = gde->date_entry;

    gtk_widget_grab_focus (entry);

    if (title)
    {
        gchar *full_text = g_strdup_printf("<b>%s</b>", title);
        gtk_label_set_markup(GTK_LABEL (dt_dialog->duplicate_title_label), full_text);
        g_free(full_text);
    }

    if (!out_num)
    {
        // The "num" field isn't being asked for, so we make the widgets invisible
        gtk_widget_set_visible(dt_dialog->num_label, FALSE);
        gtk_widget_set_visible(dt_dialog->num_edit, FALSE);
    }

    result = gtk_dialog_run (GTK_DIALOG (dt_dialog->dialog));

    if (result == GTK_RESPONSE_OK)
    {
        if (date_p)
            *date_p = gnc_date_edit_get_date (GNC_DATE_EDIT (dt_dialog->date_edit));
        if (gdate_p)
            gnc_date_edit_get_gdate(GNC_DATE_EDIT (dt_dialog->date_edit), gdate_p);
        if (out_num)
            *out_num = g_strdup (gtk_entry_get_text (GTK_ENTRY (dt_dialog->num_edit)));
        ok = TRUE;
    }
    else
        ok = FALSE;

    gtk_widget_destroy(GTK_WIDGET(dt_dialog->dialog));
    g_free (dt_dialog);

    return ok;
}

gboolean
gnc_dup_trans_dialog (GtkWidget * parent, time_t *date_p,
                      const char *num, char **out_num)
{
    return gnc_dup_trans_dialog_internal(parent, NULL, date_p, NULL, num, out_num);
}

gboolean
gnc_dup_trans_dialog_gdate (GtkWidget * parent, GDate *gdate_p,
                            const char *num, char **out_num)
{
    time_t tmp_time;
    g_assert(gdate_p);

    tmp_time = timespecToTime_t(gdate_to_timespec(*gdate_p));
    return gnc_dup_trans_dialog_internal(parent, NULL, &tmp_time, gdate_p, num, out_num);
}

gboolean
gnc_dup_date_dialog (GtkWidget * parent, const char* title, GDate *gdate_p)
{
    time_t tmp_time;
    g_assert(gdate_p);

    tmp_time = timespecToTime_t(gdate_to_timespec(*gdate_p));
    return gnc_dup_trans_dialog_internal(parent, title, &tmp_time, gdate_p, NULL, NULL);
}

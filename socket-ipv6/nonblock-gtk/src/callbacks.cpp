#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <gtk/gtk.h>
#include "callbacks.h"
#include "client.h"

// Callback
void on_file_quit(GSimpleAction* action, GVariant* parameter,
                  gpointer user_data)
{
    g_application_quit( G_APPLICATION(user_data) );
}


void on_help_about(GSimpleAction* action, GVariant* parameter,
                   gpointer user_data)
{
    gtk_show_about_dialog(top_window->self,
                "program-name", "GTK4 Socket Sample",
                "version", g_strdup_printf("%s,\nRunning against GTK %d.%d.%d",
                                           PACKAGE_VERSION,
                                           gtk_get_major_version (),
                                           gtk_get_minor_version (),
                                           gtk_get_micro_version ()),
                "copyright", "(c) 2022 Hisashi Horikawa",
                "website", "https://www.nslabs.jp/",
                "title", "About GTK4 Socket Sample",
                NULL);
}


#ifdef USE_THREAD
// 簡単のため、都度、スレッドを作る
GThread* worker_thread = nullptr;
#endif // USE_THREAD

Param worker_param;


extern bool start_connect_to_server_nonblock(const char* hostname, int port);

// Callback
void on_get_button_clicked(GtkButton* button, gpointer user_data)
{
#ifdef USE_THREAD
    if (worker_thread)
        return;
#endif

    // GTK4: gtk_entry_get_text() がいきなり廃止。
    GtkEntryBuffer* b = gtk_entry_get_buffer( GTK_ENTRY(top_window->hostEntry) );
    worker_param.hostname = gtk_entry_buffer_get_text(b);
    b = gtk_entry_get_buffer( GTK_ENTRY(top_window->portEntry) );
    const char* port_str = gtk_entry_buffer_get_text(b);
    worker_param.port = atoi(port_str);
    if ( worker_param.port <= 0 || worker_param.port > 65535 ) {
        GtkDialog* dialog = GTK_DIALOG( gtk_message_dialog_new(top_window->self,
                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                GTK_MESSAGE_ERROR,
                                GTK_BUTTONS_CLOSE,
                                "Port error") );
        g_signal_connect(dialog, "response",
                         G_CALLBACK(gtk_window_destroy), NULL);

        //GTK4: gtk_dialog_run() は廃止.
        gtk_widget_show( GTK_WIDGET(dialog) );
        return;
    }

#ifdef USE_THREAD
    // g_thread_new() は使うな. glib 2.32以降.
    // g_thread_create() は廃れた.
    GError* error = nullptr;
    worker_thread = g_thread_try_new("worker", start_get_fortune, &worker_param,
                                     &error);
    if ( !worker_thread ) {
        g_printerr("Error creating thread: %s\n", error->message);
        g_clear_error(&error);
        return;
    }
#else
    if (!start_connect_to_server_nonblock(
                    worker_param.hostname.c_str(), worker_param.port)) {
        return;
    }
#endif

    gtk_widget_set_sensitive( GTK_WIDGET(top_window->hostEntry), FALSE );
    gtk_widget_set_sensitive( GTK_WIDGET(top_window->portEntry), FALSE );
    gtk_widget_set_sensitive( GTK_WIDGET(button), FALSE );
}


// Callback
void enable_get_button(GtkEntry* /*entry*/, gpointer user_data)
{
    bool enable;
#ifdef USE_THREAD
    if (worker_thread)
        enable = false;
    else {
#endif
        enable = gtk_entry_get_text_length(top_window->hostEntry) &&
                 gtk_entry_get_text_length(top_window->portEntry);
#ifdef USE_THREAD
    }
#endif

    gtk_widget_set_sensitive( GTK_WIDGET(top_window->getButton), enable );
}


// Callback. g_idle_add() 経由 -> メインスレッドで呼び出される.
gboolean on_thread_finished(gpointer user_data)
{
#ifdef USE_THREAD
    char* fortune = (char*) user_data;

    GtkTextBuffer* buffer = gtk_text_view_get_buffer(top_window->textView);
    gtk_text_buffer_set_text(buffer, fortune, -1);
    free(fortune);

    g_thread_join(worker_thread);
    g_thread_unref(worker_thread);
    worker_thread = NULL;
#endif // USE_THREAD

    gtk_widget_set_sensitive( GTK_WIDGET(top_window->hostEntry), TRUE );
    gtk_widget_set_sensitive( GTK_WIDGET(top_window->portEntry), TRUE );
    enable_get_button(NULL, NULL);

    return FALSE; // to be removed.
}


// Callback. g_idle_add() 経由
gboolean on_connect_error(gpointer user_data)
{
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(top_window->textView);
    gtk_text_buffer_set_text(buffer, "error occurred.", -1);

#ifdef USE_THREAD
    g_thread_join(worker_thread);
    g_thread_unref(worker_thread);
    worker_thread = NULL;
#endif

    gtk_widget_set_sensitive( GTK_WIDGET(top_window->hostEntry), TRUE );
    gtk_widget_set_sensitive( GTK_WIDGET(top_window->portEntry), TRUE );
    enable_get_button(NULL, NULL);

    return FALSE; // to be removed.
}


// GTK4 サンプル. 盛大に非互換になっている。ヒドい。

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gtk/gtk.h>
#include "callbacks.h"

MyWindow* top_window = new MyWindow();

// メニューのコールバック
static const GActionEntry app_entries[] = {
    // name, 'activate' callback, parameter_type, state, 'change_state' callback
    // .interface ファイルでは 'app.quit' なのに, こちらは 'quit' にする.
    {"quit",  on_file_quit,  NULL, NULL, NULL},
    {"about", on_help_about, NULL, NULL, NULL},
};

static void add_menubar(GtkApplication* app)
{
    // 埋め込みリソースから構築するときは, gtk_builder_new_from_resource()

    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;
    if ( !gtk_builder_add_from_file(builder, PACKAGE_DATA_DIR "/menu.interface", &error) ) {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        exit(1);
    }

    GMenuModel* menubar = G_MENU_MODEL(
                        gtk_builder_get_object(builder, "menubar") );
    gtk_application_set_menubar(app, menubar);

    // アクションの登録
    g_action_map_add_action_entries(G_ACTION_MAP(app),     // self
                                    app_entries,           // entries
                                    G_N_ELEMENTS(app_entries), // n_entries
                                    app);                  // user_data
    g_object_unref(builder);
}


// Callback
static void on_app_activate(GtkApplication* app, gpointer user_data)
{
    // 'interface' XML ファイルを読み込む
    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;
    if ( !gtk_builder_add_from_file(builder,
                PACKAGE_DATA_DIR "/nonblock-socket-sample-gtk4.interface", &error) ) {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        exit(1);
    }

    // window と application を結びつける
    top_window->self = GTK_WINDOW( gtk_builder_get_object(builder, "window") );
    gtk_window_set_application( top_window->self, app );

    add_menubar(app);

    // シグナルとコールバック
    // gtk_main_quit() は GTK4 で廃止. g_main_quit() も廃れた.
    g_signal_connect( top_window->self, "destroy",
                      G_CALLBACK(g_application_quit), app );

    // TODO: 必要なときにlookupできないの?
    top_window->textView  = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "textView"));
    top_window->hostEntry = GTK_ENTRY(gtk_builder_get_object(builder, "hostEntry"));
    top_window->portEntry = GTK_ENTRY(gtk_builder_get_object(builder, "portEntry"));
    top_window->getButton = GTK_BUTTON(gtk_builder_get_object(builder, "getButton"));
    g_signal_connect(top_window->hostEntry, "changed",
                     G_CALLBACK(enable_get_button), NULL);
    g_signal_connect(top_window->portEntry, "changed",
                     G_CALLBACK(enable_get_button), NULL);
    g_signal_connect(top_window->getButton, "clicked", G_CALLBACK(on_get_button_clicked), NULL);

    g_object_unref( builder );
    gtk_widget_show( GTK_WIDGET(top_window->self) );
}


int main(int argc, char *argv[])
{
    // gtk_init() は廃れた. GtkApplication を使え.
    GtkApplication* app;
    int status;

    // G_APPLICATION_FLAGS_NONE is deprecated
    app = gtk_application_new("jp.nslabs.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);

    // アプリケィションを実行. Since GIO 2.28
    // gtk_main() は廃れた.
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}

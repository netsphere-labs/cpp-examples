
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MyWindow
{
    GtkWindow* self;

    GtkTextView* textView;
    GtkEntry* hostEntry;
    GtkEntry* portEntry;

    GtkButton* getButton;
};

extern MyWindow* top_window;

// メニューアイテム
void on_file_quit(GSimpleAction* action, GVariant* parameter,
                  gpointer user_data);

void on_help_about(GSimpleAction* action, GVariant* parameter,
                   gpointer user_data);

void on_get_button_clicked(GtkButton* button, gpointer user_data);

void enable_get_button(GtkEntry* entry, gpointer user_data);


// 子スレッド -> メインスレッド GSourceFunc
gboolean on_thread_finished(gpointer user_data);

gboolean on_connect_error(gpointer user_data);


#ifdef __cplusplus
}
#endif


#include <gtk/gtk.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

struct Param
{
    std::string hostname;
    int port;
};
  
// GThreadFunc
gpointer start_get_fortune(gpointer data);

#ifdef __cplusplus
}
#endif
